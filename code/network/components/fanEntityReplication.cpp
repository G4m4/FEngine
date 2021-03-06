#include "network/components/fanEntityReplication.hpp"
#include "core/ecs/fanEcsWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EntityReplication::SetInfo( EcsComponentInfo& /*_info*/ )
	{
	}

	//========================================================================================================
	//========================================================================================================
	void EntityReplication::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		EntityReplication& replication = static_cast<EntityReplication&>( _component );
		replication.mExclude = 0;
		replication.mComponentTypes.clear();
	}
}