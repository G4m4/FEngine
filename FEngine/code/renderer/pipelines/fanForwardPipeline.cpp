#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanImageView.h" 
#include "renderer/core/fanBuffer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanSampler.h"
#include "renderer/core/fanDescriptor.h"
#include "renderer/fanRenderer.h"
#include "renderer/util/fanVertex.h"
#include "renderer/fanMesh.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::ForwardPipeline(Device& _device ) :
		Pipeline(_device)
		, m_sceneDescriptor(nullptr){

		m_sampler = new Sampler( m_device );
		m_sampler->CreateSampler( 0, 8 );

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = m_device.GetDeviceProperties().limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformsVert );
		size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
		if ( minUboAlignment > 0 ) {
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformsVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}
		dynamicUniformsVert.Resize( s_maximumNumModels * dynamicAlignmentVert, dynamicAlignmentVert );
		dynamicUniformsMaterial.Resize( s_maximumNumModels * dynamicAlignmentFrag, dynamicAlignmentFrag );

		for ( int uniformIndex = 0; uniformIndex < s_maximumNumModels; uniformIndex++ ) {
			dynamicUniformsMaterial[uniformIndex].color = glm::vec3( 1 );
			dynamicUniformsMaterial[uniformIndex].textureIndex = 0;
			dynamicUniformsMaterial[uniformIndex].shininess = 1;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::~ForwardPipeline() {
		delete m_texturesDescriptor;
		delete m_sceneDescriptor;
		delete m_sampler;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Resize( const VkExtent2D _extent) {
		Pipeline::Resize(_extent);

		delete m_texturesDescriptor;
		m_texturesDescriptor = nullptr;
		CreateTextureDescriptor();
		DeletePipeline();
		CreatePipeline();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::CreateDescriptors( const size_t /*_numSwapchainImages*/ ) {
		m_sceneDescriptor = new Descriptor( m_device, 1 );
		m_sceneDescriptor->SetUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, sizeof( VertUniforms ) );
		m_sceneDescriptor->SetDynamicUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, dynamicUniformsVert.GetTotalSize(), dynamicUniformsVert.GetAlignment() );
		m_sceneDescriptor->SetUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		m_sceneDescriptor->SetDynamicUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, dynamicUniformsMaterial.GetTotalSize(), dynamicUniformsMaterial.GetAlignment() );
		m_sceneDescriptor->SetUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );
		m_sceneDescriptor->Create();

		CreateTextureDescriptor();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ForwardPipeline::CreateTextureDescriptor() {
		delete m_texturesDescriptor;
		m_texturesDescriptor = new  Descriptor( m_device, 1 );
		SetTextureDescriptor();

		return m_texturesDescriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ReloadShaders() {
		Pipeline::ReloadShaders();
		SetTextureDescriptor(0);
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::SetTextureDescriptor( const int _index ) {
		std::vector< Texture * > & textures = Renderer::Get().GetRessourceManager()->GetTextures();
		std::vector< VkImageView > imageViews( textures.size() );
		for ( int textureIndex = 0; textureIndex < textures.size(); textureIndex++ ) {
			imageViews[textureIndex] = textures[textureIndex]->GetImageView();
		}
		m_texturesDescriptor->SetImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT, imageViews, m_sampler->GetSampler(), _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::UpdateUniformBuffers( const size_t /*_index*/ ) {
		m_sceneDescriptor->SetBinding( 0, 0, &vertUniforms,					sizeof( VertUniforms ),					0 );
		m_sceneDescriptor->SetBinding( 1, 0, &dynamicUniformsVert[0],		dynamicUniformsVert.GetTotalSize(),		0 );
		m_sceneDescriptor->SetBinding( 2, 0, &fragUniforms,					sizeof( FragUniforms ),					0 );
		m_sceneDescriptor->SetBinding( 3, 0, &dynamicUniformsMaterial[0],	dynamicUniformsMaterial.GetTotalSize(), 0 );
		m_sceneDescriptor->SetBinding( 4, 0, &lightUniforms,				sizeof( LightsUniforms ),				0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const uint32_t _indexOffset ) {
		
		std::vector<VkDescriptorSet> descriptors = {
			m_sceneDescriptor->GetSet()
			, m_texturesDescriptor->GetSet()
		}; 
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset  * static_cast<uint32_t>( dynamicUniformsVert.GetAlignment() )
			,_indexOffset  * static_cast<uint32_t>( dynamicUniformsMaterial.GetAlignment() )
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			static_cast<uint32_t>( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ConfigurePipeline() {
		m_bindingDescription = Vertex::GetBindingDescription();
		m_attributeDescriptions = Vertex::GetAttributeDescriptions();
		m_descriptorSetLayouts = {
			m_sceneDescriptor->GetLayout()
			, m_texturesDescriptor->GetLayout()
		};
	}
}