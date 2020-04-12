#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"

namespace fan
{
	struct SceneNode;
	struct Transform;
	struct Camera;
	class EcsWorld;

	//================================================================================================================================
	// Editor camera data (transform, camera, speed, sensitivity )
	//================================================================================================================================
	struct EditorCamera : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );

		SceneNode* cameraNode;
		Transform* transform;
		Camera* camera;

		float	  speed = 10.f;
		float	  speedMultiplier = 3.f;
		btVector2 xySensitivity = btVector2( 0.005f, 0.005f );

		static void Update( EditorCamera& camera, const float _delta );
		static void CreateEditorCamera( EcsWorld& _world );
	};
}