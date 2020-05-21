#include "network/components/fanReliabilityLayer.hpp"

#include "network/singletonComponents/fanHostManager.hpp"
#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	const float ReliabilityLayer::timeoutDuration = 2.f;

	//================================================================================================================================
	//================================================================================================================================
	void ReliabilityLayer::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NETWORK16;
		_info.onGui = &ReliabilityLayer::OnGui;
		_info.init = &ReliabilityLayer::Init;
		_info.name = "reliability layer";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ReliabilityLayer::Init( EcsWorld& _world, EcsComponent& _component )
	{
		ReliabilityLayer& reliabilityLayer = static_cast<ReliabilityLayer&>( _component );
		reliabilityLayer.nextPacketTag = 0;
		reliabilityLayer.expectedPacketTag = 0;
		reliabilityLayer.pendingAck;
		reliabilityLayer.inFlightPackets;
	}

	//================================================================================================================================
	// Ensure packets are never processed out of order.Old packets arriving after newer packets are dropped.
	// Return true if the packet is valid and ready to be processed, false otherwise
	//================================================================================================================================
	bool ReliabilityLayer::ValidatePacket( Packet& _packet )
	{
		if( _packet.onlyContainsAck ) return true;

		if( _packet.tag == expectedPacketTag )	// packet is perfect \o/
		{
			expectedPacketTag++;
			pendingAck.push_back( _packet.tag );
			return true;
		}
		else if( _packet.tag < expectedPacketTag ) // silently drop old packet.
		{
			return false;
		}
		else //we missed some packets
		{
			assert( _packet.tag > expectedPacketTag );
			expectedPacketTag = _packet.tag + 1;
			pendingAck.push_back( _packet.tag );
			return true;
		}
	}

	//================================================================================================================================
	// Registers the packet as an inFlightPacket.
	// allows timeout and delivery notification to be issued
	//================================================================================================================================
	void ReliabilityLayer::RegisterPacket( Packet& _packet )
	{
		if( _packet.onlyContainsAck )
		{
			nextPacketTag--;
			return;
		}

		InFlightPacket inFlightPacket;
		inFlightPacket.tag = _packet.tag;
		inFlightPacket.onFailure = _packet.onFail;
		inFlightPacket.onSuccess = _packet.onSuccess;
		inFlightPacket.timeDispatch = Time::Get().ElapsedSinceStartup();
		inFlightPackets.push( inFlightPacket );
	}

	//================================================================================================================================
	// Process incoming acknowledgments and notify connected modules about which packets were received or dropped
	//================================================================================================================================
	void ReliabilityLayer::ProcessPacket( const PacketAck& _packetAck )
	{
		int ackIndex = 0;
		while( ackIndex < _packetAck.tags.size() && !inFlightPackets.empty() )
		{
			const PacketTag ackPacketTag = _packetAck.tags[ackIndex];
			/*const*/ InFlightPacket& inFlightPacket = inFlightPackets.front();
			if( inFlightPacket.tag == ackPacketTag ) // packet was received ! \o/
			{
				inFlightPacket.onSuccess.Emmit( inFlightPacket.tag );
				inFlightPackets.pop();
				ackIndex++;
			}
			else if( inFlightPacket.tag < ackPacketTag ) // packet was dropped or too old
			{
				inFlightPacket.onFailure.Emmit( inFlightPacket.tag );
				inFlightPackets.pop();
			}
			else // inFlightPacket was already removed (maybe from timeout)	
			{
				assert( inFlightPacket.tag > ackPacketTag );
				ackIndex++;
			}
		}
	}

	//================================================================================================================================
	// Send out an acknowledgment for each validated packet 
	//================================================================================================================================
	void ReliabilityLayer::Write( Packet& _packet )
	{
		if( !pendingAck.empty() )
		{
			PacketAck packetAck;
			packetAck.tags = pendingAck;
			packetAck.Write( _packet );
			pendingAck.clear();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ReliabilityLayer::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		ReliabilityLayer& deliveryNotification = static_cast<ReliabilityLayer&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "next packet tag:       %d", deliveryNotification.nextPacketTag );
			ImGui::Text( "expected packet tag:   %d", deliveryNotification.expectedPacketTag );
			ImGui::Text( "num pending ack:       %d", deliveryNotification.pendingAck.size() );
			ImGui::Text( "num in flight packets: %d", deliveryNotification.inFlightPackets.size() );
		}
		ImGui::PopItemWidth();
	}
}