#include "render/core/fanTexture.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"

#pragma warning(push, 0)   
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning(pop)

namespace fan
{
	TextureManager Texture::s_resourceManager;

	//================================================================================================================================
	//================================================================================================================================
	Texture::~Texture() 
	{ 
		if( !s_resourceManager.IsCleared() ) // @hack
		{
			DeleteGpuData( s_resourceManager.GetDevice() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::CopyBufferToImage( VkCommandBuffer _commandBuffer, VkBuffer _buffer, uint32_t _width, uint32_t _height )
	{

		// Specify which part of the buffer is going to be copied to which part of the image
		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			_width,
			_height,
			1
		};

		//Execute
		vkCmdCopyBufferToImage(
			_commandBuffer,
			_buffer,
			m_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::GenerateMipmaps( Device& _device, VkCommandBuffer _commandBuffer, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels )
	{
		// Check if image format supports linear bitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties( _device.vkPhysicalDevice, _imageFormat, &formatProperties );
		if ( !( formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT ) )
		{
			throw std::runtime_error( "texture image format does not support linear blitting!" );
		}

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = m_image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		// Record each of the VkCmdBlitImage commands
		int32_t mipWidth = _texWidth;
		int32_t mipHeight = _texHeight;

		for ( uint32_t i = 1; i < _mipLevels; ++i )
		{
			// This transition will wait for level i - 1 to be filled
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier( _commandBuffer,
								  VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
								  0, nullptr,
								  0, nullptr,
								  1, &barrier );

			// Specify the regions that will be used in the blit operation
			VkImageBlit blit = {};
			blit.srcOffsets[ 0 ] = { 0, 0, 0 };
			blit.srcOffsets[ 1 ] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[ 0 ] = { 0, 0, 0 };
			blit.dstOffsets[ 1 ] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			// Record the blit command
			vkCmdBlitImage( _commandBuffer,
							m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							1, &blit,
							VK_FILTER_LINEAR );

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			// This transition waits on the current blit command to finish
			vkCmdPipelineBarrier( _commandBuffer,
								  VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
								  0, nullptr,
								  0, nullptr,
								  1, &barrier );

			if ( mipWidth > 1 ) mipWidth /= 2;
			if ( mipHeight > 1 ) mipHeight /= 2;
		}

		// Transitions the last mip level from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		barrier.subresourceRange.baseMipLevel = _mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier( _commandBuffer,
							  VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
							  0, nullptr,
							  0, nullptr,
							  1, &barrier );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::DeleteGpuData( Device& _device )
	{
		vkDeviceWaitIdle( _device.vkDevice );
		Debug::Highlight( "Renderer idle1" );

		if ( m_deviceMemory != VK_NULL_HANDLE )
		{
			vkFreeMemory( _device.vkDevice, m_deviceMemory, nullptr );
			m_deviceMemory = VK_NULL_HANDLE;
		}

		if ( m_imageView != VK_NULL_HANDLE )
		{
			vkDestroyImageView( _device.vkDevice, m_imageView, nullptr );
			m_imageView = VK_NULL_HANDLE;
		}

		if ( m_image != VK_NULL_HANDLE )
		{
			vkDestroyImage( _device.vkDevice, m_image, nullptr );
			m_image = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::CreateImage( Device& _device, VkExtent2D _extent, uint32_t _mipLevels, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties )
	{
		m_mipLevels = _mipLevels;

		// VK image info struct
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = _extent.width;
		imageInfo.extent.height = _extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = _mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = _format;
		imageInfo.tiling = _tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = _usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if ( vkCreateImage( _device.vkDevice, &imageInfo, nullptr, &m_image ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create image!" );

		Debug::Get() << Debug::Severity::log << std::hex << "VkImage               " << m_image << std::dec << Debug::Endl();

		// Allocate memory for the image
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements( _device.vkDevice, m_image, &memRequirements );

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = _device.FindMemoryType( memRequirements.memoryTypeBits, _properties );

		if ( vkAllocateMemory( _device.vkDevice, &allocInfo, nullptr, &m_deviceMemory ) != VK_SUCCESS )
			throw std::runtime_error( "failed to allocate image memory!" );
		Debug::Get() << Debug::Severity::log << std::hex << "VkDeviceMemory        " << m_deviceMemory << std::dec << Debug::Endl();

		vkBindImageMemory( _device.vkDevice, m_image, m_deviceMemory, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::CreateImageView( Device& _device, VkFormat _format, VkImageViewType _viewType, VkImageSubresourceRange _subresourceRange )
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_image;
		viewInfo.viewType = _viewType;
		viewInfo.format = _format;
		viewInfo.subresourceRange = _subresourceRange;

		if ( vkCreateImageView( _device.vkDevice, &viewInfo, nullptr, &m_imageView ) != VK_SUCCESS )
			throw std::runtime_error( "failed to create texture image view!" );

		Debug::Get() << Debug::Severity::log << std::hex << "VkImageView           " << m_imageView << std::dec << Debug::Endl();

	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::TransitionImageLayout( VkCommandBuffer _commandBuffer, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkImageSubresourceRange _subresourceRange )
	{
		// Synchronize access to resources
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = _oldLayout;
		barrier.newLayout = _newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_image;
		barrier.subresourceRange = _subresourceRange;

		// Set the access masks and pipeline stages based on the layouts in the transition.
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if ( _oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if ( _oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL )
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if ( _oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
			throw std::invalid_argument( "unsupported layout transition!" );


		vkCmdPipelineBarrier(
			_commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Texture::LoadFromFile( const std::string& _path )
	{
		m_path = _path;

		// Load image from disk
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load( _path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha );

		if ( pixels == nullptr )
		{
			Debug::Get() << Debug::Severity::error << "Unable to load texture: " << _path << Debug::Endl();
			return false;
		}

		m_mipLevels = static_cast< uint32_t >( std::floor( std::log2( std::max( texWidth, texHeight ) ) ) ) + 1;
		m_mipLevels = 1;

		SetData( pixels, texWidth, texHeight, m_mipLevels );

		stbi_image_free( pixels );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::SetData( const unsigned char* _data, const uint32_t _width, const uint32_t _height, const uint32_t _mipLevels )
	{
		m_mipLevels = _mipLevels;
		m_width = _width;
		m_height = _height;
		m_data = new unsigned char[ _width * _height * 4 ];
		memcpy( m_data, _data, _width * _height * 4 * sizeof( unsigned char ) );
		GenerateGpuData( s_resourceManager.GetDevice() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::GenerateGpuData( Device& _device )
	{
		VkDeviceSize imageSize = m_width * m_height * 4 * sizeof( unsigned char );

		// Create a buffer in host visible memory
		Buffer stagingBuffer( _device );
		stagingBuffer.Create( imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
		stagingBuffer.SetData( m_data, imageSize );

		// Create the image in Vulkan
		VkExtent2D extent = { m_width, m_height };
		CreateImage( _device, extent, m_mipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

		// Transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
		VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT , 0, m_mipLevels, 0, 1 };

		VkCommandBuffer cmd = _device.BeginSingleTimeCommands();
		TransitionImageLayout( cmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange );
		CopyBufferToImage( cmd, stagingBuffer.GetBuffer(), static_cast< uint32_t >( m_width ), static_cast< uint32_t >( m_height ) );

		if ( m_mipLevels > 1 )
		{
			GenerateMipmaps( _device, cmd, VK_FORMAT_R8G8B8A8_UNORM, m_width, m_height, m_mipLevels );
		}
		else
		{
			TransitionImageLayout( cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange );
		}

		// Creates the image View
		CreateImageView( _device, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_2D, { VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipLevels, 0, 1 } );

		_device.EndSingleTimeCommands( cmd );
	}
}