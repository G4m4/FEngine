#include "render/core/fanShader.hpp"
#include <filesystem>
#include <fstream>
#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/fanSpirvCompiler.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	bool Shader::Create( Device& _device, const std::string _path )
	{
        fanAssert( mShaderModule == VK_NULL_HANDLE );

		std::vector<unsigned int> spirvCode = SpirvCompiler::Compile( _path );
		if ( spirvCode.empty() )
		{
			Debug::Error() << "Could not create shader module: " << _path << Debug::Endl();

			std::filesystem::directory_entry path( _path );
			std::string extension = path.path().extension().generic_string();
            std::string tmpPath = ( extension == ".frag" ?
                    RenderGlobal::sDefaultFragmentShader :
                    RenderGlobal::sDefaultVertexShader );
			Debug::Log() << "loading default shader " << tmpPath << Debug::Endl();
			spirvCode = SpirvCompiler::Compile( tmpPath );

			if ( spirvCode.empty() )
			{
				return false;
			}
		}

		VkShaderModuleCreateInfo shaderModuleCreateInfo;
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = spirvCode.size() * sizeof( unsigned int );
		shaderModuleCreateInfo.pCode = spirvCode.data();

        if( vkCreateShaderModule( _device.mDevice,
                                  &shaderModuleCreateInfo,
                                  nullptr,
                                  &mShaderModule ) != VK_SUCCESS )
		{
			Debug::Error()  << "Could not create shader module: " << _path << Debug::Endl();
			return false;
		}
		Debug::Log() << std::hex << "VkShaderModule        " << mShaderModule << std::dec << Debug::Endl();
		
		_device.AddDebugName( (uint64_t)mShaderModule, _path );

		return true;
	}

	//========================================================================================================
	//========================================================================================================
	void Shader::Destroy( Device& _device )
	{
		if( mShaderModule != VK_NULL_HANDLE )
		{
			vkDestroyShaderModule( _device.mDevice, mShaderModule, nullptr );
			_device.RemoveDebugName( (uint64_t)mShaderModule );
			mShaderModule = VK_NULL_HANDLE;
		}
	}

	//========================================================================================================
	//========================================================================================================
	std::vector<char> Shader::ReadFile( const std::string& _filename )
	{
        //ate -> seek to the end of stream immediately after open
		std::ifstream file( _filename, std::ios::ate | std::ios::binary );

		if ( file.is_open() == false )
		{
			Debug::Get() << Debug::Severity::error << "failed to open file: " << _filename << Debug::Endl();
			return {};
		}

		//Allocate the buffer
		size_t fileSize = ( size_t ) file.tellg(); // tellg -> position in input sequence
		std::vector<char> buffer( fileSize );

		//Read the file
		file.seekg( 0 );
		file.read( buffer.data(), fileSize );

		file.close();

		return buffer;
	}
}