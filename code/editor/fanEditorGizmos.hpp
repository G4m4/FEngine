#pragma once

#include <map>
#include <cassert>
#include "bullet/LinearMath/btVector3.h"
#include "bullet/LinearMath/btTransform.h"

#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	class EcsWorld;

	//================================================================================================================================
	// allows displaying of the translation manipulator
	// @todo make this a singleton component in the editor ecs world
	//================================================================================================================================	
	struct EditorGizmos : EcsSingleton
	{
		ECS_SINGLETON( EditorGizmos );

		//================================================================
		//================================================================	
		struct GizmoCacheData
		{
			int axisIndex;
			bool pressed = false;
			btVector3 offset;
		};

		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

		bool DrawMoveGizmo( const btTransform _transform, const size_t _uniqueID, btVector3& _newPosition );

		std::map< size_t, GizmoCacheData > m_gizmoCacheData;
		EcsWorld* m_world;
	};
}