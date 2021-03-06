#pragma once

#include <vector>
#include <iostream>
#include "game/fanLaunchSettings.hpp"

namespace fan
{
	//========================================================================================================
	// CommandLine generates EngineSettings from command line arguments 
	//========================================================================================================
	struct LaunchArguments
	{
		//===============================================================================
		//===============================================================================
		struct LaunchCommand
		{
		    using Command = bool (*)( const std::vector < std::string >& _args, LaunchSettings& _settings );
            Command     mRunCommand = nullptr; // run the command ( parse arguments & modify the settings )
			std::string mName       = "";	// command string
			std::string mUsage      = "";	// usage helper string
		};

		LaunchArguments();
		LaunchSettings			Parse( const std::vector< std::string >& _args );
		const LaunchCommand*	FindCommand( const std::string& _name );
		bool					IsCommand( const std::string& _text );

		std::vector< LaunchCommand >	commands;	// the full list of commands of the engine

		// commands
		static bool CMD_EnableLivePP( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_OpenScene( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_SetWindow( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_AutoPlay( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_RunEditorClient( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_RunEditorServer( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_RunEditorClientServer( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_RunGameClient( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_RunGameServer( const std::vector < std::string >& _args, LaunchSettings& _settings );
		static bool CMD_MainLoopSleep( const std::vector < std::string >& _args, LaunchSettings& _settings );
	};

	//========================================================================================================
	// generates the list of all commands
	//========================================================================================================
	LaunchArguments::LaunchArguments() :
            commands( { {
                                &LaunchArguments::CMD_EnableLivePP,
                                "-livepp",
                                "usage: -livepp <0-1>"
                        },
                        {
                                &LaunchArguments::CMD_OpenScene,
                                "-scene",
                                "usage: -scene \"scene/path.scene\""
                        },
                        {
                                &LaunchArguments::CMD_SetWindow,
                                "-window",
                                "usage: -window <x> <y> <width> <height>"
                        },
                        {
                                &LaunchArguments::CMD_AutoPlay,
                                "-autoplay",
                                "usage: -autoplay <0-1>"
                        },
                        {
                                &LaunchArguments::CMD_RunEditorClient,
                                "-editor_client",
                                "usage: -editor_client"
                        },
                        {
                                &LaunchArguments::CMD_RunEditorServer,
                                "-editor_server",
                                "usage: -editor_server"
                        },
                        {
                                &LaunchArguments::CMD_RunEditorClientServer,
                                "-editor_client_server",
                                "usage: -editor_client_server"
                        },
                        {
                                &LaunchArguments::CMD_RunGameClient,
                                "-client",
                                "usage: -client"
                        },
                        {
                                &LaunchArguments::CMD_RunGameServer,
                                "-server",
                                "usage: -client"
                        },
                        {
                                &LaunchArguments::CMD_MainLoopSleep,
                                "-main_loop_sleep",
                                "usage: -main_loop_sleep <0-1>"
                        },
                      } ) {}

	//========================================================================================================
	// returns the command with a name matching _arg
	//========================================================================================================
	const LaunchArguments::LaunchCommand* LaunchArguments::FindCommand( const std::string& _arg )
	{
		for( int cmdIndex = 0; cmdIndex < (int)commands.size(); cmdIndex++ )
		{
			const LaunchCommand& cmd = commands[cmdIndex];
			if( _arg == cmd.mName )
			{
				return &cmd;
			}
		}
		return nullptr;
	}


    //========================================================================================================
    //========================================================================================================
    bool LaunchArguments::IsCommand( const std::string& _text )
    {
	    return FindCommand( _text ) != nullptr;
    }

	//========================================================================================================
	// parse command line arguments and returns EngineSettings
	//========================================================================================================
	LaunchSettings LaunchArguments::Parse( const std::vector< std::string >& _args )
	{
		LaunchSettings settings;

		// live++ default settings
#ifndef NDEBUG		
		settings.enableLivepp = true;
#else				
		settings.enableLivepp = false;
#endif // !NDEBUG

		// parse commands & arguments
		int argIndex = 1;
		while( argIndex < (int)_args.size() )
		{
			const LaunchCommand* command = FindCommand( _args[argIndex] );
			if( command != nullptr )
			{

				// find the arguments of this command
				std::vector < std::string > commandArguments;
				while( ++argIndex < (int)_args.size() && !IsCommand( _args[argIndex] ) )
				{
					commandArguments.push_back( _args[argIndex] );
				}

				// execute the command
				if( !( *command->mRunCommand )( commandArguments, settings ) )
				{
					std::cout << "invalid command line arguments for command " << command->mName << std::endl;
					std::cout << command->mUsage << std::endl;
				}
			}
			else
			{
				std::cout << "invalid command line" << std::endl;
				break;
			}
		}
		return settings;
	}

	//========================================================================================================
	// command: -livepp <0-1>
	// forces the activation of live++ 
	//========================================================================================================
	bool LaunchArguments::CMD_EnableLivePP( const std::vector < std::string >& _args, LaunchSettings& _settings )
	{
		if( _args.size() != 1 ) { return false; }

		const int value = std::atoi( _args[0].c_str() );
		if( value != 1 && value != 0 ) { return false; }

		_settings.enableLivepp = value == 1 ? true : false;

		std::cout << "cmd : live++ " << ( value == 1 ? "enabled" : "disabled" ) << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -scene "scene/path.scene"
	// auto opens a scene at startup
	//========================================================================================================
	bool LaunchArguments::CMD_OpenScene( const std::vector < std::string >& _args, LaunchSettings& _settings )
	{
		if( _args.size() != 1 ) { return false; }

		_settings.loadScene = _args[0];

		std::cout << "cmd : open scene " << _settings.loadScene << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -window <x <y> <width> <height>"
	// moves the engine window it position (x,y) and resizes it to (width,height)
	//========================================================================================================
	bool LaunchArguments::CMD_SetWindow( const std::vector < std::string >& _args, LaunchSettings& _settings )
	{
		if( _args.size() != 4 ) { return false; }

		_settings.window_position.x = std::atoi( _args[0].c_str() );
		_settings.window_position.y = std::atoi( _args[1].c_str() );
		_settings.window_size.x = std::atoi( _args[2].c_str() );
		_settings.window_size.y = std::atoi( _args[3].c_str() );
        _settings.mForceWindowDimensions = true;

		std::cout << "cmd : window position(" << _settings.window_position.x
		          << "," << _settings.window_position.y << ")";
		std::cout << ", size(" << _settings.window_size.x
		          << "," << _settings.window_size.y << ")" << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -autoplay <0-1>"
	// if a scene is loaded at startup, plays it directly
	//========================================================================================================
    bool LaunchArguments::CMD_AutoPlay( const std::vector<std::string>& _args,
                                        LaunchSettings& _settings )
	{
		if( _args.size() != 1 ) { return false; }

		const int value = std::atoi( _args[0].c_str() );
		if( value != 1 && value != 0 ) { return false; }

		_settings.autoPlay = value == 1 ? true : false;

		std::cout << "cmd : autoplay " << ( value == 1 ? "enabled" : "disabled" ) << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -editor_client"
	// runs the game in client mode with the editor
	//========================================================================================================
    bool LaunchArguments::CMD_RunEditorClient( const std::vector<std::string>& _args,
                                               LaunchSettings& _settings )
	{
		if( _args.size() != 0 ) { return false; }

		_settings.launchMode = LaunchSettings::Mode::EditorClient;
        _settings.launchEditor = true;

		std::cout << "cmd : launch editor client" << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -editor_server"
	// runs the game in server mode with the editor
    //========================================================================================================
    bool LaunchArguments::CMD_RunEditorServer( const std::vector<std::string>& _args,
                                               LaunchSettings& _settings )
	{
		if( _args.size() != 0 ) { return false; }

		_settings.launchMode = LaunchSettings::Mode::EditorServer;
        _settings.launchEditor = true;

		std::cout << "cmd : launch editor server" << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -editor_client_server"
    // runs two instances of the game in client and server mode with the editor
    //========================================================================================================
    bool LaunchArguments::CMD_RunEditorClientServer( const std::vector<std::string>& _args,
                                                     LaunchSettings& _settings )
	{
		if( _args.size() != 0 ) { return false; }

		_settings.launchMode = LaunchSettings::Mode::EditorClientServer;
        _settings.launchEditor = true;

		std::cout << "cmd : launch editor client_server" << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -server"
    // runs the game in server mode with the editor
    //========================================================================================================
    bool LaunchArguments::CMD_RunGameServer( const std::vector<std::string>& _args,
                                             LaunchSettings& _settings )
    {
		if( _args.size() != 0 ) { return false; }

		_settings.launchMode = LaunchSettings::Mode::Server;
        _settings.launchEditor = false;

		std::cout << "cmd : launch game server" << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -client"
	// runs the game in server mode with the editor
    //========================================================================================================
    bool LaunchArguments::CMD_RunGameClient( const std::vector<std::string>& _args,
                                             LaunchSettings& _settings )
    {
		if( _args.size() != 0 ) { return false; }

		_settings.launchMode = LaunchSettings::Mode::Client;
        _settings.launchEditor = false;

		std::cout << "cmd : launch game client" << std::endl;
		return true;
	}

	//========================================================================================================
	// command: -main_loop_sleep"
	// makes the main loop sleep et the end of the frame
	//========================================================================================================
    bool LaunchArguments::CMD_MainLoopSleep( const std::vector<std::string>& _args,
                                             LaunchSettings& _settings )
	{
		if( _args.size() != 1 ) { return false; }

		const int value = std::atoi( _args[0].c_str() );
		if( value != 1 && value != 0 ) { return false; }

		_settings.mainLoopSleep = value == 1 ? true : false;

		std::cout << "cmd : main loop sleep " << ( value == 1 ? "enabled" : "disabled" ) << std::endl;
		return true;
	}
}