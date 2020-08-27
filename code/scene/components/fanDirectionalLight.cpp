#include "scene/components/fanDirectionalLight.hpp"
#include "core/fanSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::DIR_LIGHT16;
		_info.group = EngineGroups::SceneRender;
		_info.onGui = &DirectionalLight::OnGui;
		_info.load = &DirectionalLight::Load;
		_info.save = &DirectionalLight::Save;
		_info.editorPath = "lights/";
		_info.name = "directional light";
	}

	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		DirectionalLight& light = static_cast<DirectionalLight&>( _component );
		light.mAmbiant  = Color( 0.1f, 0.1f, 0.1f );
		light.mDiffuse  = Color::White;
		light.mSpecular = Color::White;
	}

	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::Save( const EcsComponent& _component, Json& _json )
	{
		const DirectionalLight& light = static_cast<const DirectionalLight&>( _component );
		Serializable::SaveColor( _json, "ambiant", light.mAmbiant );
		Serializable::SaveColor( _json, "diffuse", light.mDiffuse );
		Serializable::SaveColor( _json, "specular", light.mSpecular );
	}
	
	//========================================================================================================
	//========================================================================================================
	void DirectionalLight::Load( EcsComponent& _component, const Json& _json )
	{
		DirectionalLight& light = static_cast<DirectionalLight&>( _component );
		Serializable::LoadColor( _json, "ambiant", light.mAmbiant );
		Serializable::LoadColor( _json, "diffuse", light.mDiffuse );
		Serializable::LoadColor( _json, "specular", light.mSpecular );
	}

    //========================================================================================================
    //========================================================================================================
    void DirectionalLight::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        DirectionalLight& light = static_cast<DirectionalLight&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            // Filter color
            if( ImGui::Button( "##ambiant" ) ) { light.mAmbiant = Color::Black; } ImGui::SameLine();
            ImGui::ColorEdit3( "ambiant", light.mAmbiant.Data(), ImGui::fanColorEditFlags );

            if( ImGui::Button( "##diffuse" ) ) { light.mDiffuse = Color::Black; } ImGui::SameLine();
            ImGui::ColorEdit3( "diffuse", light.mDiffuse.Data(), ImGui::fanColorEditFlags );

            if( ImGui::Button( "##specular" ) ) { light.mSpecular = Color::Black; } ImGui::SameLine();
            ImGui::ColorEdit3( "specular", light.mSpecular.Data(), ImGui::fanColorEditFlags );


// 			const Transform& transform = m_gameobject->GetTransform();
// 			const btVector3 pos = transform.GetPosition();
// 			const btVector3 dir = transform.Forward();
// 			const btVector3 up = transform.Up();
// 			const btVector3 left = transform.Left();
// 			const float length = 2.f;
// 			const float radius = 0.5f;
// 			const Color color = Color::Yellow;
// 			btVector3 offsets[5] = { btVector3::Zero(), radius * up ,-radius * up, radius * left ,-radius * left };
// 			for( int offsetIndex = 0; offsetIndex < 5; offsetIndex++ )
// 			{
// 				const btVector3 offset = offsets[offsetIndex];
// 				_world.GetSingleton<RenderDebug>().DebugLine( pos + offset, pos + offset + length * dir, color );
// 			}
// 			_world.GetSingleton<RenderDebug>().DebugSphere( transform.GetBtTransform(), radius, 0, color );
        } ImGui::PopItemWidth();
    }
}