#include "render/pipelines/fanForwardPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanImage.hpp"
#include "render/core/fanImageView.hpp" 
#include "render/core/fanBuffer.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanSampler.hpp"
#include "render/fanVertex.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::ForwardPipeline( Device& _device, DescriptorImages* _textures, DescriptorSampler* _sampler ) :
		Pipeline( _device )
		, m_textures( _textures )
		, m_sampler( _sampler )
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)m_device.mDeviceProperties.limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformsVert );
		size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
		if ( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformsVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}

		m_dynamicUniformsVert.SetAlignement( dynamicAlignmentVert );
		m_dynamicUniformsMaterial.SetAlignement( dynamicAlignmentFrag );

		m_dynamicUniformsVert.Resize( 256 );
		m_dynamicUniformsMaterial.Resize( 256 );

		for ( int uniformIndex = 0; uniformIndex < m_dynamicUniformsMaterial.Size(); uniformIndex++ )
		{
			m_dynamicUniformsMaterial[ uniformIndex ].color = glm::vec4( 1 );
			m_dynamicUniformsMaterial[ uniformIndex ].shininess = 1;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	ForwardPipeline::~ForwardPipeline()
	{
		m_sceneDescriptor.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::Resize( const VkExtent2D _extent )
	{
		Pipeline::Resize( _extent );
		DeletePipeline();
		CreatePipeline();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::CreateDescriptors( const uint32_t _numSwapchainImages )
	{
		Debug::Log() << "Forward pipeline : create descriptors" << Debug::Endl();
		m_sceneDescriptor.AddUniformBinding( m_device, _numSwapchainImages, VK_SHADER_STAGE_VERTEX_BIT, sizeof( VertUniforms ) );
		m_sceneDescriptor.AddDynamicUniformBinding( m_device, _numSwapchainImages, VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment() );
		m_sceneDescriptor.AddUniformBinding( m_device, _numSwapchainImages, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		m_sceneDescriptor.AddDynamicUniformBinding( m_device, _numSwapchainImages, VK_SHADER_STAGE_FRAGMENT_BIT, m_dynamicUniformsMaterial.Size(), m_dynamicUniformsMaterial.Alignment() );
		m_sceneDescriptor.AddUniformBinding( m_device, _numSwapchainImages, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );
		m_sceneDescriptor.Create( m_device, _numSwapchainImages );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ResizeDynamicDescriptors( const uint32_t _count, const size_t _newSize )
	{
		m_dynamicUniformsVert.Resize( _newSize );
		m_dynamicUniformsMaterial.Resize( _newSize );
		m_sceneDescriptor.ResizeDynamicUniformBinding( m_device, _count, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment(), 1 );
		m_sceneDescriptor.ResizeDynamicUniformBinding( m_device, _count, m_dynamicUniformsMaterial.Size(), m_dynamicUniformsMaterial.Alignment(), 3 );
		m_sceneDescriptor.UpdateDescriptorSets( m_device, _count );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ReloadShaders()
	{
		Pipeline::ReloadShaders();

	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::SetUniformsData( const size_t _index )
	{
		m_sceneDescriptor.SetData( m_device, 0, _index, &m_vertUniforms, sizeof( VertUniforms ), 0 );
		m_sceneDescriptor.SetData( m_device, 1, _index, &m_dynamicUniformsVert[0], m_dynamicUniformsVert.Alignment() * m_dynamicUniformsVert.Size(), 0 );
		m_sceneDescriptor.SetData( m_device, 2, _index, &m_fragUniforms, sizeof( FragUniforms ), 0 );
		m_sceneDescriptor.SetData( m_device, 3, _index, &m_dynamicUniformsMaterial[0], m_dynamicUniformsMaterial.Alignment() * m_dynamicUniformsMaterial.Size(), 0 );
		m_sceneDescriptor.SetData( m_device, 4, _index, &m_lightUniforms, sizeof( LightsUniforms ), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{

		std::vector<VkDescriptorSet> descriptors = {
			m_sceneDescriptor.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast< uint32_t >( m_dynamicUniformsVert.Alignment() )
			,_indexOffset* static_cast< uint32_t >( m_dynamicUniformsMaterial.Alignment() )
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			static_cast< uint32_t >( descriptors.size() ),
			descriptors.data(),
			static_cast< uint32_t >( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void ForwardPipeline::ConfigurePipeline()
	{
		m_bindingDescription = Vertex::GetBindingDescription();
		m_attributeDescriptions = Vertex::GetAttributeDescriptions();
		m_descriptorSetLayouts = {
			m_sceneDescriptor.mDescriptorSetLayout
			, m_textures->mDescriptorSetLayout
			, m_sampler->mDescriptorSetLayout
		};
	}
}