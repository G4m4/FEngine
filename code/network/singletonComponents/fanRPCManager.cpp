#include "network/singletonComponents/fanRPCManager.hpp"

#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( RPCManager );

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::JOYSTICK16;
		_info.init = &RPCManager::Init;
		_info.onGui = &RPCManager::OnGui;
		_info.name = "RPC manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		RPCManager& rpc = static_cast<RPCManager&>( _component );
		rpc.nameToRPCTable.clear();

		rpc.RegisterRPCs();
	}

	//================================================================================================================================
	// Registers all available RPC unwrap functions
	//================================================================================================================================
	void RPCManager::RegisterRPCs( )
	{
		RegisterUnwrapFunction( 'SYCF', &RPCManager::UnwrapSynchClientFrame );
	}

	//================================================================================================================================
	// Registers a RPC unwrap function
	// A unwrap function must decode a packet and run the corresponding procedure
	//================================================================================================================================
	void RPCManager::RegisterUnwrapFunction( const RpcId _id, const RpcUnwrapFunc _rpcUnwrapFunc )
	{
		assert( nameToRPCTable.find( _id ) == nameToRPCTable.end() );
		nameToRPCTable[_id] = _rpcUnwrapFunc;
	}

	//================================================================================================================================
	// Runs a procedure from an incoming rpc packet data
	//================================================================================================================================
	void RPCManager::TriggerRPC( sf::Packet& _packet )
	{
		RpcId rpcID;
		_packet >> rpcID;

		const RpcUnwrapFunc function =  nameToRPCTable[rpcID];
		( ( *this ).*( function ) )( _packet );
	}

	//================================================================================================================================
	// SynchClientFrame RPC - wrap data
	//================================================================================================================================
	PacketReplication RPCManager::RPCSynchClientFrame( const sf::Uint64 _frameIndex, const float _rtt )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;

		packet.packetData.clear();
		packet.packetData << RpcId( 'SYCF' );
		packet.packetData << _frameIndex;
		packet.packetData << _rtt;

		return packet;
	}

	//================================================================================================================================
	// SynchClientFrame RPC - unwrap data & synchronizes the frame index of the client depending on its rtt
	//================================================================================================================================
	void RPCManager::UnwrapSynchClientFrame( sf::Packet& _packet )
	{
		sf::Uint64 frameIndex;
		float RTT;

		_packet >> frameIndex;
		_packet >> RTT;

		Debug::Highlight() << frameIndex << " " << RTT << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RPCManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		RPCManager& rpc = static_cast<RPCManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "rpc list: " );
			ImGui::Indent();
			for ( std::pair<RpcId, RpcUnwrapFunc> pair : rpc.nameToRPCTable )
			{
				char* charArray = (char*)(&pair.first);
				std::stringstream ss;
				ss << charArray[3] << charArray[2] << charArray[1] << charArray[0];
				ImGui::Text( ss.str().c_str() );
			}
			ImGui::Unindent();
		}ImGui::Unindent(); ImGui::Unindent();
	}
}