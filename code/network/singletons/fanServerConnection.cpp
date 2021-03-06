#include "network/singletons/fanServerConnection.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ServerConnection::SetInfo( EcsSingletonInfo& /*_info*/ )
	{
	}

	//========================================================================================================
	//========================================================================================================
	void ServerConnection::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		ServerConnection& connection = static_cast<ServerConnection&>( _component );
		connection.mServerPort = 53000;
	}
}