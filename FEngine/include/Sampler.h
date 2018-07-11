#pragma once

#include "Device.h"

class Sampler
{
public:
	Device& m_device;
	VkSampler sampler;

	Sampler(Device& device, uint32_t mipLevels);
	~Sampler();
	void createSampler(uint32_t mipLevels);

};