#include "game/fanGameServer.hpp"

#include "core/time/fanProfiler.hpp"
#include "core/time/fanTime.hpp"
#include "game/network/fanPacket.hpp"

#include "scene/systems/fanSynchronizeMotionStates.hpp"
#include "scene/systems/fanRegisterPhysics.hpp"
#include "scene/systems/fanUpdateParticles.hpp"
#include "scene/systems/fanEmitParticles.hpp"
#include "scene/systems/fanGenerateParticles.hpp"
#include "scene/systems/fanUpdateBounds.hpp"
#include "scene/systems/fanUpdateTimers.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanParticle.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanExpirationTime.hpp"
#include "scene/components/fanFollowTransform.hpp"
#include "scene/components/ui/fanFollowTransformUI.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/singletonComponents/fanRenderWorld.hpp"
#include "scene/singletonComponents/fanScenePointers.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/fanSceneTags.hpp"
#include "game/fanGameTags.hpp"

#include "game/singletonComponents/fanCollisionManager.hpp"
#include "game/singletonComponents/fanSolarEruption.hpp"
#include "game/singletonComponents/fanGameCamera.hpp"
#include "game/singletonComponents/fanSunLight.hpp"
#include "game/singletonComponents/fanGame.hpp"

#include "game/systems/fanUpdatePlanets.hpp"
#include "game/systems/fanUpdateSpaceships.hpp"
#include "game/systems/fanUpdateGameCamera.hpp"
#include "game/systems/fanUpdateWeapons.hpp"
#include "game/systems/fanUpdatePlayerInput.hpp"
#include "game/systems/fanUpdateEnergy.hpp"
#include "game/systems/fanUpdateGameUI.hpp"
#include "game/systems/fanParticlesOcclusion.hpp"

#include "game/components/fanPlanet.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/components/fanBullet.hpp"
#include "game/components/fanBattery.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"
#include "game/components/fanSpaceshipUI.hpp"
#include "game/components/fanDamage.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	GameServer::GameServer( const std::string _name )
	{
		// base components
		world.AddComponentType<SceneNode>();
		world.AddComponentType<Transform>();
		world.AddComponentType<DirectionalLight>();
		world.AddComponentType<PointLight>();
		world.AddComponentType<MeshRenderer>();
		world.AddComponentType<Material>();
		world.AddComponentType<Camera>();
		world.AddComponentType<ParticleEmitter>();
		world.AddComponentType<Particle>();
		world.AddComponentType<Rigidbody>();
		world.AddComponentType<MotionState>();
		world.AddComponentType<BoxShape>();
		world.AddComponentType<SphereShape>();
		world.AddComponentType<TransformUI>();
		world.AddComponentType<UIRenderer>();
		world.AddComponentType<Bounds>();
		world.AddComponentType<ExpirationTime>();
		world.AddComponentType<FollowTransform>();
		world.AddComponentType<ProgressBar>();
		world.AddComponentType<FollowTransformUI>();

		// game components
		world.AddComponentType<Planet>();
		world.AddComponentType<SpaceShip>();
		world.AddComponentType<PlayerInput>();
		world.AddComponentType<Weapon>();
		world.AddComponentType<Bullet>();
		world.AddComponentType<Battery>();
		world.AddComponentType<SolarPanel>();
		world.AddComponentType<Health>();
		world.AddComponentType<SpaceshipUI>();
		world.AddComponentType<Damage>();

		// base singleton components
		world.AddSingletonComponentType<Scene>();
		world.AddSingletonComponentType<RenderWorld>();
		world.AddSingletonComponentType<PhysicsWorld>();
		world.AddSingletonComponentType<ScenePointers>();

		// game singleton components
		world.AddSingletonComponentType<SunLight>();
		world.AddSingletonComponentType<GameCamera>();
		world.AddSingletonComponentType<CollisionManager>();
		world.AddSingletonComponentType<Game>();
		world.AddSingletonComponentType<SolarEruption>();

		world.AddTagType<tag_boundsOutdated>();
		world.AddTagType<tag_sunlight_occlusion>();

		Game& game = world.GetSingletonComponent<Game>();
		game.gameServer = this;
		game.name = _name;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Start()
	{
		Game& gameData = world.GetSingletonComponent<Game>();

		// init network
		socket.setBlocking( false );
		Debug::Log() << gameData.name << " bind on port " << serverPort << Debug::Endl();
		if( socket.bind( serverPort ) != sf::Socket::Done )
		{
			Debug::Error() << gameData.name << " bind failed on port " << serverPort << Debug::Endl();
		}

		// init game
		S_RegisterAllRigidbodies::Run( world, world.Match( S_RegisterAllRigidbodies::GetSignature( world ) ) );
		GameCamera::CreateGameCamera( world );
		SolarEruption::Start( world );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameServer::Stop()
	{
		// clears the physics world
		PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
		S_UnregisterAllRigidbodies::Run( world, world.Match( S_UnregisterAllRigidbodies::GetSignature( world ) ) );
		physicsWorld.rigidbodiesHandles.clear();

		// clears the particles mesh
		RenderWorld& renderWorld = world.GetSingletonComponent<RenderWorld>();
		renderWorld.particlesMesh.LoadFromVertices( {} );

		GameCamera::DeleteGameCamera( world );

		// clears the network
		socket.unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Pause()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::Resume()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void  GameServer::Step( const float _delta )
	{
		{
			SCOPED_PROFILE( scene_update );

			NetworkReceive();

			// physics & transforms
			PhysicsWorld& physicsWorld = world.GetSingletonComponent<PhysicsWorld>();
			S_SynchronizeMotionStateFromTransform::Run( world, world.Match( S_SynchronizeMotionStateFromTransform::GetSignature( world ) ), _delta );
			physicsWorld.dynamicsWorld->stepSimulation( _delta, 10, Time::Get().GetPhysicsDelta() );
			S_SynchronizeTransformFromMotionState::Run( world, world.Match( S_SynchronizeTransformFromMotionState::GetSignature( world ) ), _delta );
			S_MoveFollowTransforms::Run( world, world.Match( S_MoveFollowTransforms::GetSignature( world ) ) );
			S_MoveFollowTransformsUI::Run( world, world.Match( S_MoveFollowTransformsUI::GetSignature( world ) ) );

			// update
			S_MoveSpaceships::Run( world, world.Match( S_MoveSpaceships::GetSignature( world ) ), _delta );
			S_FireWeapons::Run( world, world.Match( S_FireWeapons::GetSignature( world ) ), _delta );
			S_MovePlanets::Run( world, world.Match( S_MovePlanets::GetSignature( world ) ), _delta );
			S_GenerateLightMesh::Run( world, world.Match( S_GenerateLightMesh::GetSignature( world ) ), _delta );
			S_UpdateSolarPannels::Run( world, world.Match( S_UpdateSolarPannels::GetSignature( world ) ), _delta );
			S_RechargeBatteries::Run( world, world.Match( S_RechargeBatteries::GetSignature( world ) ), _delta );
			S_UpdateExpirationTimes::Run( world, world.Match( S_UpdateExpirationTimes::GetSignature( world ) ), _delta );
			S_EruptionDamage::Run( world, world.Match( S_EruptionDamage::GetSignature( world ) ), _delta );
			S_UpdateGameUiValues::Run( world, world.Match( S_UpdateGameUiValues::GetSignature( world ) ), _delta );
			S_UpdateGameUiPosition::Run( world, world.Match( S_UpdateGameUiPosition::GetSignature( world ) ), _delta );

			SolarEruption::Step( world, _delta );

			S_PlayerDeath::Run( world, world.Match( S_PlayerDeath::GetSignature( world ) ), _delta );

			// late update
			S_ParticlesOcclusion::Run( world, world.Match( S_ParticlesOcclusion::GetSignature( world ) ), _delta );
			S_UpdateParticles::Run( world, world.Match( S_UpdateParticles::GetSignature( world ) ), _delta );
			S_EmitParticles::Run( world, world.Match( S_EmitParticles::GetSignature( world ) ), _delta );
			S_GenerateParticles::Run( world, world.Match( S_GenerateParticles::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromRigidbody::Run( world, world.Match( S_UpdateBoundsFromRigidbody::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromModel::Run( world, world.Match( S_UpdateBoundsFromModel::GetSignature( world ) ), _delta );
			S_UpdateBoundsFromTransform::Run( world, world.Match( S_UpdateBoundsFromTransform::GetSignature( world ) ), _delta );

			S_UpdateGameCamera::Run( world, world.Match( S_UpdateGameCamera::GetSignature( world ) ), _delta );

			NetworkSend();
		}

		{
			// end frame
			SCOPED_PROFILE( scene_endFrame );
			world.SortEntities();
			world.RemoveDeadEntities();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::NetworkReceive()
	{
		// receive
		sf::Packet		packet;
		sf::IpAddress	receiveIP;
		unsigned short	receivePort;

		const sf::Socket::Status socketStatus = socket.receive( packet, receiveIP, receivePort );
		if( receivePort != serverPort )
		{
			switch( socketStatus )
			{
			case sf::UdpSocket::Done:
			{
				// create / get client
				Client* client = FindClient( receiveIP, receivePort );
				if( client == nullptr )
				{
					Client newClient;
					newClient.ip = receiveIP;
					newClient.port = receivePort;
					newClient.state = Client::CONNECTING;
					newClient.name = "Unknown";					
					clients.push_back( newClient );
					client = &clients[clients.size() - 1];
				}

				// update client time
				const double currentTime = Time::Get().ElapsedSinceStartup();
				client->lastResponse = currentTime;

				sf::Uint16 intType;
				packet >> intType;
				const PacketType type = PacketType( intType );

				switch( type )
				{
				case PacketType::LOGIN:
				{
					// first connection
					if( client->state == Client::CONNECTING )
					{
						PacketLogin login( packet );
						client->state = Client::CONNECTED_NEED_ACK;
						client->name = login.name;
					}
					// client didn't receive the connection ack yet
					else if( client->state == Client::CONNECTED )
					{
						client->state = Client::CONNECTED_NEED_ACK;
					}

				} break;
				case PacketType::ACK:
				{
// 					PacketACK packetAck( packet );
// 					switch( packetAck.ackType )
// 					{
// 					case PING:
// 						break;
// 					default:
// 						assert( false );
// 						break;
// 					}
				} break;
				case PacketType::PING:
				{
					PacketPing packetPing( packet );
					client->roundTripDelay = (float)( currentTime - packetPing.time );
					break;
				} break;
				// 				case PacketType::START_GAME:
				// 					Debug::Log() << m_socket.GetName() << " start game " << Debug::Endl();
				// 					m_playersManager->SpawnSpaceShips();
				// 					break;
				default:
					Debug::Warning() << " strange packet received with id: " << intType << Debug::Endl();
					break;
				}
			} break;
			case sf::UdpSocket::Error: 
				Debug::Warning() << "socket.receive: an unexpected error happened " << Debug::Endl();
				break;
			case sf::UdpSocket::Partial:
			case sf::UdpSocket::NotReady:
			{
				// do nothing
			} break;
			case sf::UdpSocket::Disconnected:
			{
				// disconnect client
			} break;
			default:
				assert( false );
				break;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameServer::NetworkSend()
	{
		const double currentTime = Time::Get().ElapsedSinceStartup();

		// send
		for( int i = (int)clients.size() - 1; i >= 0; i-- )
		{
			Client& client = clients[i];

			switch( client.state )
			{
			case Client::CONNECTING:
			{
				PacketLogin packetLogin;
				packetLogin.name = "please login potato";
				socket.send( packetLogin.ToPacket(), client.ip, client.port );
			} break;
			case Client::CONNECTED_NEED_ACK:
			{
				PacketACK packetAck;
				packetAck.ackType = PacketType::LOGIN;
				socket.send( packetAck.ToPacket(), client.ip, client.port );
				client.state = Client::CONNECTED;
			} break;
			case Client::CONNECTED:
			{

			} break;
			default:
				assert( false );
				break;
			}


			// client timeout 
			if( currentTime - client.lastResponse > timeoutDuration )
			{
				Debug::Log() << "timeout" << client.name << Debug::Endl();
				clients.erase( clients.begin() + i );
			}

			// ping client & send a status
			if( currentTime - client.lastPingTime > pingDuration )
			{
				PacketPing packetPing;
				packetPing.time = Time::ElapsedSinceStartup();
				socket.send( packetPing.ToPacket(), client.ip, client.port );
				client.lastPingTime = currentTime;

				PacketStatus packetStatus;
				packetStatus.roundTripDelay = client.roundTripDelay;
				socket.send( packetStatus.ToPacket(), client.ip, client.port );
			}
		}
	}

	//================================================================================================================================
	// returns the client data associated with an ip/port, returns nullptr if it doesn't exists
	//================================================================================================================================
	Client* GameServer::FindClient( const sf::IpAddress _ip, const unsigned short _port )
	{
		for ( Client& client : clients )
		{
			if( client.ip == _ip && client.port == _port )
			{
				return &client;
			}
		}
		return nullptr;
	}
}