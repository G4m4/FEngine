#include "network/singletonComponents/fanServerConnection.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ServerConnection );

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnection::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ServerConnection::Init;
		_info.onGui = &ServerConnection::OnGui;
		_info.name = "server connection";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnection::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ServerConnection& connection = static_cast<ServerConnection&>( _component );
		connection.serverPort = 53000;
		connection.socket.Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerConnection::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ServerConnection& connection = static_cast<ServerConnection&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{			
			ImGui::Text( "Server" );
			ImGui::Spacing();
			ImGui::Text( "port: %u", connection.serverPort );
		}ImGui::Unindent(); ImGui::Unindent();
	}
}