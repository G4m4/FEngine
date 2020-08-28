#include "scene/singletons/fanSceneResources.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/fanPrefab.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void SceneResources::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mIcon  = ImGui::IconType::Scene16;
        _info.mGroup = EngineGroups::SceneRender;
        _info.mName  = "scene resources";
        _info.onGui = &SceneResources::OnGui;
    }

    //========================================================================================================
    //========================================================================================================
    void SceneResources::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        SceneResources& sceneResources = static_cast<SceneResources&>( _singleton );
        (void)sceneResources;
    }

    //========================================================================================================
    //========================================================================================================
    void SceneResources::SetPointers( PrefabManager* _prefabManager )
    {
        mPrefabManager   = _prefabManager;
    }

    //========================================================================================================
    //========================================================================================================
    void SceneResources::SetupResources( PrefabManager& _prefabManager )
    {
        ResourcePtr< Prefab >::sOnResolve.Connect ( &PrefabManager::ResolvePtr, &_prefabManager );
    }

    //========================================================================================================
    //========================================================================================================
    void SceneResources::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        if( ImGui::CollapsingHeader( "prefabs" ) )
        {
            SceneResources& sceneResources = static_cast<SceneResources&>( _singleton );
            const std::map<std::string, Prefab*>& prefabs = sceneResources.mPrefabManager->GetPrefabs();
            for( auto pair : prefabs )
            {
                ImGui::Text( pair.second->mPath.c_str() );
            }
        }
    }
}