#pragma once

namespace vk
{
	class Device;
	class Buffer;

	//================================================================================================================================
	//================================================================================================================================
	class Texture
	{
	public:
		Texture(Device & _device);
		~Texture();

		void Load( const void * _data, const uint32_t _width, const uint32_t _height, const uint32_t _mipLevels);
		bool LoadTexture( const std::string _path);

		std::string GetPath() const { return m_path;		}
		VkImageView GetImageView()	{ return m_imageView;	}

	private:
		Device &		m_device;
		VkImage			m_image;
		VkDeviceMemory	m_deviceMemory;
		VkImageView		m_imageView;
		
		std::string		m_path;
		uint32_t		m_mipLevels = 1;
		uint32_t		m_width;
		uint32_t		m_height;
		uint32_t		m_layerCount = 1;

		void Destroy();
		void CreateImage	( VkExtent2D _extent, uint32_t _mipLevels, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties);
		void CreateImageView( VkFormat _format, VkImageViewType _viewType, VkImageSubresourceRange _subresourceRange);
		
		void CopyBufferToImage		( VkCommandBuffer _commandBuffer, VkBuffer _buffer, uint32_t _width, uint32_t _height);
		void GenerateMipmaps		( VkCommandBuffer _commandBuffer, VkFormat _imageFormat, int32_t _texWidth, int32_t _texHeight, uint32_t _mipLevels);
		void TransitionImageLayout	( VkCommandBuffer _commandBuffer, VkImageLayout _oldLayout, VkImageLayout _newLayout, VkImageSubresourceRange _subresourceRange);
		bool CreateTextureImage		( VkCommandBuffer _commandBuffer, std::vector<std::string> _paths);

	};
}