#include "network/components/fanClientRollback.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ClientRollback::SetInfo( EcsComponentInfo& _info )
	{
		_info.save        = &ClientRollback::Save;
		_info.load        = &ClientRollback::Load;
	}

	//========================================================================================================
	//========================================================================================================
	void ClientRollback::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ClientRollback& clientRollback = static_cast<ClientRollback&>( _component );
		clientRollback.mRollbackDatas.clear();
	}

	//========================================================================================================
	//========================================================================================================
	void ClientRollback::Save( const EcsComponent& /*_component*/, Json& /*_json*/ ) {}
	void ClientRollback::Load( EcsComponent& /*_component*/, const Json& /*_json*/ ) {}
}