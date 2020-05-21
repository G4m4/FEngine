#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;

	//==============================================================================================================================================================
	// updates the ui sliders from the player/world variables ( health, energy, etc. )
	//==============================================================================================================================================================
	struct S_UpdateGameUiValues : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};

	//==============================================================================================================================================================
	// moves the ui status bars above the player spaceship 
	//==============================================================================================================================================================
	struct S_UpdateGameUiPosition : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta );
	};
}