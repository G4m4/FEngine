#pragma once

#include "scene/fanScenePrecompiled.hpp"
#include "ecs/fanEcsWorld.hpp"

#include "SFML/System.hpp"
#include "SFML/Network.hpp"

namespace fan
{
	//================================================================================================================================
	// 
	//================================================================================================================================
	struct GameClient
	{
		GameClient( const std::string _name );

		void Start();
		void Stop();
		void Pause();
		void Resume();
		void Step( const float _delta );

		EcsWorld		world;
		sf::UdpSocket	socket;
		unsigned short	listenPort = 53001;
	};
}
