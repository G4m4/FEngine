#pragma once

#include <vector>
#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/fanUniforms.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"

namespace fan
{
	//================================================================================================================================
	// NDT = no depth test
	//================================================================================================================================
	struct DrawDebug
	{
		Pipeline			mPipelineLines;
		Pipeline			mPipelineLinesNDT;
		Pipeline			mPipelineTriangles;
		Shader				mFragmentShaderLines;
		Shader				mVertexShaderLines;
		Shader				mFragmentShaderLinesNDT;
		Shader				mVertexShaderLinesNDT;
		Shader				mFragmentShaderTriangles;
		Shader				mVertexShaderTriangles;
		DebugUniforms		mUniformsLines;
		DebugUniforms		mUniformsLinesNDT;
		DebugUniforms		mUniformsTriangles;
		DescriptorUniforms	mDescriptorLines;
		DescriptorUniforms	mDescriptorLinesNDT;
		DescriptorUniforms	mDescriptorTriangles;
		CommandBuffer		mCommandBuffers;

		std::vector<Buffer>	mVertexBuffersLines;
		std::vector<Buffer>	mVertexBuffersLinesNDT;
		std::vector<Buffer>	mVertexBuffersTriangles;
		int mNumLines = 0;
		int mNumLinesNDT = 0;
		int mNumTriangles = 0;

		void				Create( Device& _device, uint32_t _imagesCount );
		void				Destroy( Device& _device );
		void				UpdateUniformBuffers( Device& _device, const size_t _index );
		void				BindDescriptorsLines( VkCommandBuffer _commandBuffer, const size_t _index );
		void				BindDescriptorsLinesNDT( VkCommandBuffer _commandBuffer, const size_t _index );
		void				BindDescriptorsTriangles( VkCommandBuffer _commandBuffer, const size_t _index );
		bool				HasNothingToDraw() const { return mNumLines == 0 && mNumLinesNDT == 0 && mNumTriangles == 0; }
		PipelineConfig		GetPipelineConfigLines() const;
		PipelineConfig		GetPipelineConfigLinesNDT() const;
		PipelineConfig		GetPipelineConfigTriangles() const;
	};
}