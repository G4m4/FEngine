#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/math/shapes/fanAABB.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// axis aligned bounding box
	//==============================================================================================================================================================
	struct Bounds : public EcsComponent
	{
		ECS_COMPONENT( Bounds )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );

		AABB aabb;
	};
	static constexpr size_t sizeof_bounds = sizeof( Bounds );
}