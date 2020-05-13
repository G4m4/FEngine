#pragma  once

#include "ecs/fanComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// [Server] Sends packets to clients to replicates objects / run RPC
	//==============================================================================================================================================================
	struct HostReplication: public Component
	{
		DECLARE_COMPONENT( HostReplication )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );

		enum ReplicationFlags
		{
			None = 0,
			ResendUntilReplicated = 1 << 1	// if the packets drops, resend it
		};

		//================================================================
		// replication data for a singleton component
		//================================================================
		struct ReplicationData
		{
			ReplicationFlags flags = ReplicationFlags::None;	// replication parameters
			PacketReplication packet;							// saved replication data
			Signal<> onSuccess;
		};

		std::multimap< PacketTag, ReplicationData> pendingReplication; // sent on the network, waiting for a status
		std::vector<ReplicationData>				nextReplication;	// waiting  to be sent on the network
	
		void		Write( Packet& _packet );

		Signal<>&	Replicate( const PacketReplication& _packet, const ReplicationFlags _flags );
		void		OnReplicationSuccess( const PacketTag _packetTag );
		void		OnReplicationFail( const PacketTag _packetTag );

		static PacketReplication BuildSingletonPacket( const EcsWorld& _world, const uint32_t _staticID );
		static PacketReplication BuildEntityPacket( EcsWorld& _world, const EntityHandle _entityHandle, const std::vector<uint32_t>& _componentTypeInfo );
		static PacketReplication BuildRPCPacket( sf::Packet& _dataRPC );
	};
	static constexpr size_t sizeof_hostReplication = sizeof( HostReplication );
}