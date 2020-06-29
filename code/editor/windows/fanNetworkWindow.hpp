#pragma once

#include "editor/windows/fanEditorWindow.hpp"

#include "game/fanGameServer.hpp"
#include "game/fanGameClient.hpp"

namespace fan
{
	class EcsWorld;
	struct GameServer;
	struct GameClient;

	//================================================================================================================================
	// Draw gui for the client game or the server game
	//================================================================================================================================
	class NetworkWindow : public EditorWindow
	{
	public:
		NetworkWindow();

	protected:
		void OnGui( EcsWorld& _world ) override;

	private:
		void OnGuiServer( GameServer& _game );
		void OnGuiClient( GameClient& _game );
	};
}