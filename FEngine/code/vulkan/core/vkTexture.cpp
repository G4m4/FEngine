#include "fanIncludes.h"

#include "vulkan/core/vkTexture.h"
#include "vulkan/core/vkDevice.h"
#include "vulkan/core/vkBuffer.h"
#include "vulkan/vkRenderer.h"

#pragma warning(push, 0)   
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#pragma warning(pop)

namespace vk {

	//================================================================================================================================
	//================================================================================================================================
	Texture::Texture(Device & _device) :
		m_device(_device) {

	}

	//================================================================================================================================
	//================================================================================================================================
	Texture::~Texture() {
		Destroy();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::CopyBufferToImage(VkCommandBuffer _commandBuffer, VkBuffer _buffer, uint32_t _width, uint32_t _height) {

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
	void Texture::GenerateMipmaps(VkCommandBuffer _commandBuffer, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels) {
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(m_device.vkPhysicalDevice, _imageFormat, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
			throw std::runtime_error("texture image format does not support linear blitting!");

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

		for (uint32_t i = 1; i < _mipLevels; ++i)
		{
			// This transition will wait for level i - 1 to be filled
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(_commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			// Specify the regions that will be used in the blit operation
			VkImageBlit blit = {};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			// Record the blit command
			vkCmdBlitImage(_commandBuffer,
				m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			// This transition waits on the current blit command to finish
			vkCmdPipelineBarrier(_commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		// Transitions the last mip level from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		barrier.subresourceRange.baseMipLevel = _mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(_commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::Destroy() {
		if (m_deviceMemory != VK_NULL_HANDLE) {
			vkFreeMemory(m_device.vkDevice, m_deviceMemory, nullptr);
			m_deviceMemory = VK_NULL_HANDLE;
		}

		if (m_imageView != VK_NULL_HANDLE) {
			vkDestroyImageView(m_device.vkDevice, m_imageView, nullptr);
			m_imageView = VK_NULL_HANDLE;
		}

		if (m_image != VK_NULL_HANDLE) {
			vkDestroyImage(m_device.vkDevice, m_image, nullptr);
			m_image = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::CreateImage(VkExtent2D _extent, uint32_t _mipLevels, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties)
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

		if (vkCreateImage(m_device.vkDevice, &imageInfo, nullptr, &m_image) != VK_SUCCESS)
			throw std::runtime_error("failed to create image!");

		std::cout << std::hex << "VkImage \t\t" << m_image << std::dec << std::endl;

		// Allocate memory for the image
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device.vkDevice, m_image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = m_device.FindMemoryType(memRequirements.memoryTypeBits, _properties);

		if (vkAllocateMemory(m_device.vkDevice, &allocInfo, nullptr, &m_deviceMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate image memory!");
		std::cout << std::hex << "VkDeviceMemory \t\t" << m_deviceMemory << std::dec << std::endl;

		vkBindImageMemory(m_device.vkDevice, m_image, m_deviceMemory, 0);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::CreateImageView(VkFormat _format, VkImageViewType _viewType, VkImageSubresourceRange _subresourceRange)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_image;
		viewInfo.viewType = _viewType;
		viewInfo.format = _format;
		viewInfo.subresourceRange = _subresourceRange;

		if (vkCreateImageView(m_device.vkDevice, &viewInfo, nullptr, &m_imageView) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture image view!");

		std::cout << std::hex << "VkImageView \t\t" << m_imageView << std::dec << std::endl;

	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::TransitionImageLayout(VkCommandBuffer _commandBuffer, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkImageSubresourceRange _subresourceRange)
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

		if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
			throw std::invalid_argument("unsupported layout transition!");


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
	bool Texture::CreateTextureImage(VkCommandBuffer _commandBuffer, std::vector<std::string> _paths) {
		std::vector< stbi_uc*> pixels;
		std::vector< glm::ivec3 > sizes;
		uint_fast32_t totalSize = 0;

		glm::ivec3 defaultSize;
		int defaultTexChannels;
		stbi_uc* defaultPixelData = stbi_load(_paths[0].c_str(), &defaultSize.x, &defaultSize.y, &defaultTexChannels, STBI_rgb_alpha);
		assert(defaultPixelData);

		// Load pixel data
		for (std::string path : _paths)
		{
			glm::ivec3 size;
			int texChannels;
			stbi_uc* pixelData = stbi_load(path.c_str(), &size.x, &size.y, &texChannels, STBI_rgb_alpha);
			if (!pixelData)
			{
				pixelData = defaultPixelData;
				size = defaultSize;
				texChannels = defaultTexChannels;
			}

			m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.x, size.y)))) + 1;
			m_mipLevels = 1;

			pixels.push_back(pixelData);
			sizes.push_back(size);
			totalSize += size.x * size.y * 4;
		}

		m_layerCount = static_cast<uint32_t>(_paths.size());

		// Create a buffer in host visible memory
		vk::Buffer stagingBuffer(m_device);
		stagingBuffer.Create(totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		// Copy the pixel values from the image loading library to the buffer
		stagingBuffer.Map(totalSize);

		size_t offset = 0;
		for (unsigned i = 0; i < m_layerCount; ++i)
		{
			glm::ivec3 size = sizes[i];
			stbi_uc* pixelData = pixels[i];
			uint32_t imageSize = size.x * size.y * 4;

			memcpy((char*)stagingBuffer.GetMappedData() + offset, pixelData, imageSize);
			offset += imageSize;
			stbi_image_free(pixelData);
		}

		/////////////////////////////////// Image ///////////////////////////////////

		// Setup buffer copy regions for array layers
		std::vector<VkBufferImageCopy> bufferCopyRegions;
		offset = 0;
		for (uint32_t layer = 0; layer < m_layerCount; layer++)
		{
			glm::ivec3 size = sizes[layer];
			uint32_t imageSize = size.x * size.y * 4;

			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = size.x;
			bufferCopyRegion.imageExtent.height = size.y;
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = offset;

			bufferCopyRegions.push_back(bufferCopyRegion);

			// Increase offset into staging buffer for next level / face
			offset += imageSize;
		}

		// Create optimal tiled target image
		glm::ivec3 size = sizes[0];
		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.extent = { static_cast<uint32_t>(size.x),  static_cast<uint32_t>(size.y), 1 };
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.arrayLayers = m_layerCount;

		if (vkCreateImage(m_device.vkDevice, &imageCreateInfo, nullptr, &m_image) != VK_SUCCESS)
			throw std::runtime_error("failed to vkCreateImage!");

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(m_device.vkDevice, m_image, &memReqs);

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = m_device.FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(m_device.vkDevice, &memAllocInfo, nullptr, &m_deviceMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to create vkAllocateMemory!");
		if (vkBindImageMemory(m_device.vkDevice, m_image, m_deviceMemory, 0) != VK_SUCCESS)
			throw std::runtime_error("failed to create vkBindImageMemory!");

		// Image barrier for optimal image (target)
		// Set initial layout for all array layers (faces) of the optimal (target) tiled texture
		VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT , 0, 1, 0, m_layerCount };
		TransitionImageLayout(_commandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

		// Copy the cube map faces from the staging buffer to the optimal tiled image
		vkCmdCopyBufferToImage(
			_commandBuffer,
			stagingBuffer.GetBuffer(),
			m_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			static_cast<uint32_t>(bufferCopyRegions.size()),
			bufferCopyRegions.data()
		);

		TransitionImageLayout(_commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);

		/////////////////////////////////// image view ///////////////////////////////////

		CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_2D_ARRAY, { VK_IMAGE_ASPECT_COLOR_BIT, 0,1,0,m_layerCount });

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Texture::LoadTexture(std::string _path) {
		m_path = _path;

		// Load image from disk
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(_path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (pixels == nullptr) {
			return false;
		}

		m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

		Load(pixels, texWidth, texHeight, m_mipLevels);

		stbi_image_free(pixels);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Texture::Load(void* _data, int _width, int _height, uint32_t _mipLevels) {
		m_mipLevels = _mipLevels;
		VkDeviceSize imageSize = _width * _height * 4 * sizeof(char);

		// Create a buffer in host visible memory
		Buffer stagingBuffer(m_device);
		stagingBuffer.Create(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.SetData(_data, imageSize);

		// Create the image in Vulkan
		VkExtent2D extent = { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height) };
		CreateImage(extent, m_mipLevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// Transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

		VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT , 0, m_mipLevels, 0, 1 };

		VkCommandBuffer cmd = Renderer::GetRenderer().BeginSingleTimeCommands();
		TransitionImageLayout(cmd, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
		CopyBufferToImage(cmd, stagingBuffer.GetBuffer(), static_cast<uint32_t>(_width), static_cast<uint32_t>(_height));
		
		if (m_mipLevels > 1){
			GenerateMipmaps(cmd, VK_FORMAT_R8G8B8A8_UNORM, _width, _height, m_mipLevels);
		} else {
			TransitionImageLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
		}

		// Creates the image View
		CreateImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_VIEW_TYPE_2D, { VK_IMAGE_ASPECT_COLOR_BIT, 0, m_mipLevels, 0, 1 });

		Renderer::GetRenderer().EndSingleTimeCommands(cmd);
	}
}