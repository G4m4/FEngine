#include "core/ecs/fanEcsChunkAllocator.hpp"
#include "core/fanAssert.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	EcsChunkAllocator::~EcsChunkAllocator()
	{
		for( void* chunk : mFreeChunks )
		{
			delete chunk;
		}
		mFreeChunks.clear();
	}

	//========================================================================================================
	//========================================================================================================
	void* EcsChunkAllocator::Alloc()
	{
		void* chunk = nullptr;
		if( mFreeChunks.empty() )
		{
			chunk = new uint8_t[sChunkSize];
			mSize++;
		}
		else
		{
			chunk = *mFreeChunks.rbegin();
			mFreeChunks.pop_back();
		}
		return chunk;
	}

	//========================================================================================================
	//========================================================================================================
	void EcsChunkAllocator::Free( void* _chunk )
	{
		fanAssert( _chunk != nullptr );
		mFreeChunks.push_back( _chunk );
	}
}