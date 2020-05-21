#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// Draw the bounds of all scene nodes 
	//==============================================================================================================================================================
	struct S_DrawDebugBounds : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw the normals of all models 
	//==============================================================================================================================================================
	struct S_DrawDebugNormals : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the models in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugWireframe : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the models convex hull in wireframe
	//==============================================================================================================================================================
	struct S_DrawDebugHull : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the point lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugPointLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw all the directional lights gizmos
	//==============================================================================================================================================================
	struct S_DrawDebugDirectionalLights : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};

	//==============================================================================================================================================================
	// Draw physics shapes in wireframe (box, sphere, etc )
	//==============================================================================================================================================================
	struct S_DrawDebugCollisionShapes : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities );
	};
}