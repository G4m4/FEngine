#pragma once

#include <string>
#include "core/fanHash.hpp"
#include "editor/fanImguiIcons.hpp"
#include "ecs/fanEcsEntity.hpp"
#include "ecs/fanEcsTypes.hpp"
#include "fanJson.hpp"

namespace sf
{
	class Packet;
}

namespace fan
{
	class EcsWorld;

	//================================
	//================================
	#define ECS_COMPONENT( _ComponentType)															\
	public:																							\
	template <class T> struct EcsComponentInfoImpl	{													\
		static constexpr uint32_t	 s_size		{ sizeof( T )			  };						\
		static constexpr uint32_t	 s_alignment{ alignof( T )			  };						\
		static constexpr const char* s_name		{ #_ComponentType		  };						\
		static constexpr uint32_t	 s_type		{ SSID( #_ComponentType ) };						\
	};																								\
	using Info = EcsComponentInfoImpl< _ComponentType >;												\

	struct EcsComponent {};

	//================================
	//================================
	struct EcsComponentInfo
	{
		std::string name;
		ImGui::IconType icon = ImGui::IconType::NONE;	// editor icon
		uint32_t	type;
		int			index;
		uint32_t	size;
		uint32_t	alignment;
		const char* editorPath = "";					// editor path ( for the addComponent ) popup of the inspector

		void ( *init )( EcsWorld&, EcsComponent& ) = nullptr;						// (mandatory) called at the creation of the component
		void ( *destroy )( EcsWorld&, EcsComponent& ) = nullptr;					// called at the destruction of the component
		void ( *onGui )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;			// called by the editor for gui display
		void ( *save )( const EcsComponent&, Json& ) = nullptr;						// called when the scene is saved
		void ( *load )( EcsComponent&, const Json& ) = nullptr;						// called when the scene is loaded ( after the init )
		void ( *netSave ) ( const EcsComponent&, sf::Packet& _packet ) = nullptr;
		void ( *netLoad ) ( EcsComponent&, sf::Packet& _packet ) = nullptr;
	};
}