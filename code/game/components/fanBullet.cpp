#include "game/components/fanBullet.hpp"

namespace fan
{

	//========================================================================================================
	//========================================================================================================
	void Bullet::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Joystick16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &Bullet::OnGui;
		_info.load        = &Bullet::Load;
		_info.save        = &Bullet::Save;
		_info.mEditorPath = "game/";
		_info.mName       = "bullet";
	}

	//========================================================================================================
	//========================================================================================================
	void Bullet::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );

		// bullets
		bullet.mExplosionPrefab.Set( nullptr );
	}

	//========================================================================================================
	//========================================================================================================
	void Bullet::Save( const EcsComponent& _component, Json& _json )
	{
		const Bullet& bullet = static_cast<const Bullet&>( _component );
		Serializable::SavePrefabPtr( _json, "explosion_prefab", bullet.mExplosionPrefab );
	}

	//========================================================================================================
	//========================================================================================================
	void Bullet::Load( EcsComponent& _component, const Json& _json )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );
		Serializable::LoadPrefabPtr( _json, "explosion_prefab", bullet.mExplosionPrefab );
	}

    //========================================================================================================
    //========================================================================================================
    void Bullet::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        Bullet& bullet = static_cast<Bullet&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::FanPrefab( "explosion prefab", bullet.mExplosionPrefab );
        } ImGui::PopItemWidth();
    }
}