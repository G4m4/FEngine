#include "network/components/fanHostPersistentHandle.hpp"
#include "core/ecs/fanEcsWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void HostPersistentHandle::SetInfo( EcsComponentInfo& /*_info*/ )
	{
	}

	//========================================================================================================
	//========================================================================================================
	void HostPersistentHandle::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostPersistentHandle& networkHandle = static_cast<HostPersistentHandle&>( _component );
		networkHandle.mHandle = 0;
	}
}