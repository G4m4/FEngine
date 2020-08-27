#include "network/components/fanHostReplication.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "network/singletons/fanLinkingContext.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void HostReplication::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::IconType::Network16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &HostReplication::OnGui;
		_info.mName  = "host replication";
	}

	//========================================================================================================
	//========================================================================================================
	void HostReplication::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostReplication& hostReplication = static_cast<HostReplication&>( _component );
		hostReplication.mPendingReplication.clear();
		hostReplication.mNextReplication.clear();
	}

	//========================================================================================================
	// Replicates data on all connected hosts
	// returns a success signal that the caller can connect to to get notified of the acknowledgments 
	// ( ResendUntilReplicated flag must be on )
	//========================================================================================================
	Signal<>& HostReplication::Replicate( const PacketReplication& _packet, const ReplicationFlags _flags )
	{
		mNextReplication.emplace_back();
		ReplicationData& replicationData = mNextReplication[mNextReplication.size() - 1];
		replicationData.mFlags  = _flags;
		replicationData.mPacket = _packet;
		return replicationData.mOnSuccess;
	}

	//========================================================================================================
	// Builds & returns a replication packet to replicate an ecs singleton
	//========================================================================================================
	PacketReplication HostReplication::BuildSingletonPacket( const EcsWorld& _world, const uint32_t _staticID)
	{
		// generates replicated data for a singleton component
		const EcsSingletonInfo& info = _world.GetSingletonInfo( _staticID );
		const EcsSingleton& component = _world.GetSingleton( _staticID );
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::SingletonComponent;
		packet.packetData.clear();
		packet.packetData << sf::Uint32( info.mType);
		info.netSave( component, packet.packetData );

		return packet;
	}

	//========================================================================================================
	// Builds & returns a replication packet to replicate a list of components on an entity
	//========================================================================================================
    PacketReplication HostReplication::BuildEntityPacket( EcsWorld& _world,
                                                          const EcsHandle _handle,
                                                          const std::vector<uint32_t>& _componentTypeInfo )
	{
		const LinkingContext& linkingContext = _world.GetSingleton< LinkingContext> ();
		const EcsEntity entity = _world.GetEntity( _handle );

		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::Entity;
		packet.packetData.clear();

		// Serializes net id
		 const auto it = linkingContext.mEcsHandleToNetID.find( _handle );
		 if( it != linkingContext.mEcsHandleToNetID.end() )
		 {
			 const NetID netID = it->second;
			 packet.packetData << netID;
			 packet.packetData << sf::Uint8( _componentTypeInfo.size() );
			 if( it != linkingContext.mEcsHandleToNetID.end() )
			 {
				 for( const uint32_t typeInfo : _componentTypeInfo )
				 {
					 const EcsComponentInfo& info = _world.GetComponentInfo( typeInfo );
					 EcsComponent& component = _world.GetComponent( entity, typeInfo );
					 packet.packetData << sf::Uint32( info.mType );
					 info.netSave( component, packet.packetData );
				 }
			 }
		 }
		 return packet;
	}

	//========================================================================================================
	// Builds & returns a replication packet for running RPC 
	//========================================================================================================
	PacketReplication HostReplication::BuildRPCPacket( sf::Packet& _dataRPC )
	{
		PacketReplication packet;
		packet.replicationType = PacketReplication::ReplicationType::RPC;
		packet.packetData.clear();
		packet.packetData = _dataRPC;

		return packet;
	}

	//========================================================================================================
	// Sends all new replication packed
	//========================================================================================================
	void HostReplication::Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet )
	{
		for( ReplicationData& data : mNextReplication )
		{
			data.mPacket.Write( _packet );
			if( data.mFlags & ReplicationFlags::ResendUntilReplicated )
			{
				mPendingReplication.insert( { _packet.tag , data } );
				const EcsHandle& handle = _world.GetHandle( _entity );
				_packet.onSuccess.Connect( &HostReplication::OnReplicationSuccess, _world, handle );
				_packet.onFail.Connect( &HostReplication::OnReplicationFail, _world, handle );
			}
		}
		mNextReplication.clear();
	}

	//========================================================================================================
	// Replication packet has arrived, remove from pending list
	//========================================================================================================
	void HostReplication::OnReplicationSuccess( const PacketTag _packetTag )
	{
		using MMapIt = std::multimap< PacketTag, ReplicationData>::iterator;
		std::pair<MMapIt, MMapIt> result = mPendingReplication.equal_range( _packetTag );
		for( MMapIt it = result.first; it != result.second; it++ )
		{
			ReplicationData& data = it->second;
			data.mOnSuccess.Emmit();
		}
		mPendingReplication.erase( _packetTag );
		//Debug::Highlight() << "success: " << _packetTag << Debug::Endl();
	}

	//========================================================================================================
	// Replication packet has dropped, resends it if necessary
	//========================================================================================================
	void HostReplication::OnReplicationFail( const PacketTag _packetTag )
	{
		using MMapIt = std::multimap< PacketTag, ReplicationData>::iterator;
		std::pair<MMapIt, MMapIt> result = mPendingReplication.equal_range( _packetTag );
		for( MMapIt it = result.first; it != result.second; it++ )
		{
			ReplicationData& data = it->second;
			mNextReplication.push_back( data );
		}
		mPendingReplication.erase( _packetTag );
		//Debug::Warning() << "fail: " << _packetTag << Debug::Endl();
	}

	//========================================================================================================
	//========================================================================================================
	void HostReplication::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		HostReplication& hostReplication = static_cast<HostReplication&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "next replication:    %d", hostReplication.mNextReplication.size() );
			ImGui::Text( "pending replication: %d", hostReplication.mPendingReplication.size() );
		} 
		ImGui::PopItemWidth();
	}
}