#include "ecs/fanEcsSystem.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/fanRandom.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanParticleEmitter.hpp"
#include "scene/components/fanParticle.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Spawns particles for all particle emitters in the scene
	//==============================================================================================================================================================
	struct S_EmitParticles : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Transform>() | _world.GetSignature<ParticleEmitter>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			SCOPED_PROFILE( S_EmitParticles );

			if( _delta == 0.f ) { return; }

			auto transformIt = _view.begin<Transform>();
			auto particleEmitterIt = _view.begin<ParticleEmitter>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++particleEmitterIt )
			{
				const Transform& emitterTransform = *transformIt;
				ParticleEmitter& emitter = *particleEmitterIt;

				if( emitter.mParticlesPerSecond > 0.f && emitter.mEnabled )
				{
					emitter.mTimeAccumulator += _delta;
					float particleSpawnDelta = 1.f / emitter.mParticlesPerSecond;

					const glm::vec3 origin = ToGLM( emitterTransform.GetPosition() );

					// Spawn particles
					while( emitter.mTimeAccumulator > particleSpawnDelta )
					{
						emitter.mTimeAccumulator -= particleSpawnDelta;

						EcsEntity entity = _world.CreateEntity();
						Particle& particle = _world.AddComponent<Particle>( entity );

						particle.mSpeed        = glm::normalize( glm::vec3( Random::FloatClip(), Random::FloatClip(), Random::FloatClip() ) );
						particle.mSpeed *= emitter.mSpeed;
						particle.mPosition     = origin;
						particle.mDurationLeft = emitter.mDuration;
						particle.mColor        = emitter.mColor;

						_world.AddTagsFromSignature( entity, emitter.mTagsSignature );
					}
				}
			}
		}
	};
}