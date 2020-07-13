#include "render/pipelines/fanUIPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//================================================================================================================
	//================================================================================================================================
	UIPipeline::UIPipeline( Device& _device, DescriptorImages* _textures, DescriptorSampler* _sampler ) :
		Pipeline( _device )
		, m_textures( _textures )
		, m_sampler( _sampler )
	{

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)m_device.mDeviceProperties.limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformUIVert );
		if ( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformUIVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}
		m_dynamicUniformsVert.SetAlignement( dynamicAlignmentVert );
		m_dynamicUniformsVert.Resize( 256 );
	}

	//================================================================================================================================
	//================================================================================================================================
	UIPipeline::~UIPipeline()
	{
		m_transformDescriptor.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			m_transformDescriptor.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast< uint32_t >( m_dynamicUniformsVert.Alignment() )
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
	void UIPipeline::ConfigurePipeline()
	{
		m_bindingDescription = UIVertex::GetBindingDescription();
		m_attributeDescriptions = UIVertex::GetAttributeDescriptions();

		m_descriptorSetLayouts = {
			 m_transformDescriptor.mDescriptorSetLayout
			,m_textures->mDescriptorSetLayout
			, m_sampler->mDescriptorSetLayout
		};
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::SetUniformsData( const size_t _index )
	{
		m_transformDescriptor.SetData( m_device, 0, _index, &m_dynamicUniformsVert[ 0 ], m_dynamicUniformsVert.Alignment() * m_dynamicUniformsVert.Size(), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::CreateDescriptors( const uint32_t _numSwapchainImages )
	{
		Debug::Log() << "UI pipeline : create descriptors" << Debug::Endl();
		m_transformDescriptor.AddDynamicUniformBinding( m_device, _numSwapchainImages, VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment() );
		m_transformDescriptor.Create( m_device, _numSwapchainImages );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::ResizeDynamicDescriptors( const uint32_t _count, const size_t _newSize )
	{
		m_dynamicUniformsVert.Resize( _newSize );
		m_transformDescriptor.ResizeDynamicUniformBinding( m_device, _count, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment(), 1 );
	}
}