#pragma once

#include "ecs/fanSlot.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "core/fanAssert.hpp"
#include "editor/fanGroupsColors.hpp"

namespace ImGui
{
    //========================================================================================================
    //========================================================================================================
    struct FanPopupSetSingletonSlot
    {
        static constexpr char * sName = "Select singleton and slot";
        static void Open();
        static void Draw( fan::EcsWorld& _world , fan::SlotPtr& _slotPtr );
    };

    //========================================================================================================
    //========================================================================================================
    struct FanPopupSetComponentSlot
    {
        static constexpr const char * sName = "Select component and slot";

        static void Open();
        static void Draw( fan::EcsWorld& _world, fan::SlotPtr& _slotPtr );
    };
}