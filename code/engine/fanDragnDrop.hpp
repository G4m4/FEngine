#pragma once

#include <cstdint>
#include "imgui/imgui.h"
#include "core/ecs/fanEcsTypes.hpp"

namespace fan
{
	class  Prefab;
	struct EcsComponentInfo;
	class EcsWorld;
}

namespace ImGui
{
	//========================================================================================================
	// helper struct for component drag & drop
	//========================================================================================================
	struct ComponentPayload
	{
	    static const std::string sPrefix;
		fan::EcsHandle           mHandle        = 0;
		uint32_t                 mComponentType = 0;

		bool IsValid() const { return mHandle != 0; }
		static bool IsComponentPayload( const ImGuiPayload * _payload );
	};

    //========================================================================================================
    // helper struct for singleton drag & drop
    //========================================================================================================
    struct SingletonPayload
    {
        static const std::string sPrefix;
        uint32_t                 mType = 0;

        static bool IsSingletonPayload( const ImGuiPayload * _payload );
    };

    void
    FanBeginDragDropSourcePrefab( fan::Prefab* _prefab, ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    fan::Prefab* FanBeginDragDropTargetPrefab();

    void FanBeginDragDropSourceSingleton( fan::EcsWorld& _world,
                                          uint32_t _type,
                                          ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );

    void FanBeginDragDropSourceComponent( fan::EcsWorld& _world,
                                          fan::EcsHandle& _handle,
                                          uint32_t _type,
                                          ImGuiDragDropFlags _flags = ImGuiDragDropFlags_None );
    ComponentPayload FanBeginDragDropTargetComponent( fan::EcsWorld& _world, uint32_t _type );


 	//========================================================================================================
 	//========================================================================================================
 	template< typename _componentType >
	ComponentPayload FanBeginDragDropTargetComponent( fan::EcsWorld& _world )
 	{
 		static_assert((std::is_base_of<fan::EcsComponent, _componentType>::value));
 		return FanBeginDragDropTargetComponent( _world, _componentType::Info::sType );
 	}
}