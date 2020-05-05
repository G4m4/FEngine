#pragma  once

#include "ecs/fanComponent.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// A particle that disappears after some time
	// Is emmited from a particle emmiter
	//==============================================================================================================================================================
	struct Particle : public Component
	{
		DECLARE_COMPONENT( Particle )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );

		Color		color;
		glm::vec3	position;
		glm::vec3	speed;
		float		durationLeft;
	};
	static constexpr size_t sizeof_particle = sizeof( Particle );
}