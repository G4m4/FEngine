#pragma once

#include <vector>

#include "Util.h"
#include "Device.h"

namespace vk
{	
	/// Encapsulates access to a Vulkan buffer backed up by device memory
	class Buffer
	{
	public:
		Buffer(vk::Device& device);
		~Buffer();

		void Destroy();

		vk::Device& m_device;

		VkBuffer m_buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo descriptor;
		VkDeviceSize m_size = 0;
		VkDeviceSize alignment = 0;
		void* mappedData = nullptr;

		VkBufferUsageFlags m_usageFlags;
		VkMemoryPropertyFlags m_memoryPropertyFlags;

		/// Map a memory range of this buffer. If successful, mapped points to the specified buffer range
		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		/// Unmap a mapped memory range
		void Unmap();

		/// Attach the allocated memory block to the buffer
		VkResult Bind(VkDeviceSize offset = 0);
		
		/// Setup the default descriptor for this buffer
		void SetupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		VkResult CreateBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size);

		/// Flush a memory range of the buffer to make it visible to the device
		VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = memory;
			mappedRange.offset = offset;
			mappedRange.size = size;
			return vkFlushMappedMemoryRanges(m_device.device, 1, &mappedRange);
		}
	};
}