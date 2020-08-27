#include "scene/components/fanMotionState.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void MotionState::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::RIGIDBODY16;
		_info.group = EngineGroups::ScenePhysics;
		_info.destroy = &MotionState::Destroy;
		_info.save = &MotionState::Save;
		_info.load = &MotionState::Load;
		_info.onGui = &MotionState::OnGui;
		_info.editorPath = "/";
		_info.name = "motion state";
	}

	//========================================================================================================
	//========================================================================================================
	void MotionState::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		// clear
		MotionState& motionState = static_cast<MotionState&>( _component );
		motionState.mMotionState = new btDefaultMotionState();
	}

	//========================================================================================================
	//========================================================================================================
	void MotionState::Destroy( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		MotionState& motionState = static_cast<MotionState&>( _component );
		assert( motionState.mMotionState != nullptr );
		delete motionState.mMotionState;
		motionState.mMotionState = nullptr;
	}

	//========================================================================================================
	//========================================================================================================
	void MotionState::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& /*_component*/ ){}
}