#include "engine/project/fanLiveppMain.hpp"

#include "project_spaceships/game/fanGameServer.hpp"
#include "project_spaceships/game/fanGameClient.hpp"
#include "engine/project/fanGameProjectContainer.hpp"
#include "editor/fanEditorProjectContainer.hpp"

//============================================================================================================
//============================================================================================================
int main( int _argc, char* _argv[] )
{
    (void) _argc; (void) _argv;


/*	std::vector< std::string > args; // command line arguments
	args.push_back( _argv[0] );

// force arguments into the command line
// 	args.push_back( "-livepp" );
// 	args.push_back( "0" );
//	args.push_back( "-autoplay" );
//	args.push_back( "1" );
 	args.push_back( "-scene" );
    args.push_back( "content/scenes/game00.scene" );
	args.push_back( "-editor_client_server" );
//	args.push_back( "-client" );

	// generates a list of strings from the command line arguments
	for( int i = 1; i < _argc; i++ ){	args.push_back( _argv[i] );	}

	// Parse the arguments & run the engine
	fan::LaunchArguments launchArguments;
	fan::LaunchSettings settings = launchArguments.Parse( args );
	fan::LPPMain main( settings ); */

    fan::LaunchSettings settings;
    settings.windowName = "takala";
#ifdef FAN_EDITOR
    fan::GameClient client;
    fan::GameServer server;
    fan::EditorProjectContainer editor( settings, { &client, &server } );
    editor.Run();
#else
    fan::GameClient client;
    fan::GameProjectContainer   game( settings, client );
    game.Run();
#endif

	return 0;
}

int WinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPSTR _lpCmdLine, int _nShowCmd )
{
    (void)_hInstance;
    (void)_hPrevInstance;
    (void)_lpCmdLine;
    (void)_nShowCmd;

    main( __argc, __argv );
}