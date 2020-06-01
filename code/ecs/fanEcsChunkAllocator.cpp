#include "ecs/fanEcsChunkAllocator.hpp"

#include <cassert>

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsChunkAllocator::~EcsChunkAllocator()
	{
		//assert( m_size == 0 );
		for( void* chunk : m_freeChunks )
		{
			delete chunk;
		}
		m_freeChunks.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void* EcsChunkAllocator::Alloc()
	{
		void* chunk = nullptr;
		if( m_freeChunks.empty() )
		{
			chunk = new uint8_t[chunkSize];
			m_size++;
		}
		else
		{
			chunk = *m_freeChunks.rbegin();
			m_freeChunks.pop_back();
		}
		return chunk;
	}

	//================================================================================================================================
	//================================================================================================================================
	void EcsChunkAllocator::Free( void* _chunk )
	{
		assert( _chunk != nullptr );
		m_freeChunks.push_back( _chunk );
	}
}