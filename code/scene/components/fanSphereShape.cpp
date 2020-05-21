#include "scene/components/fanSphereShape.hpp"

#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::SPHERE_SHAPE16;
		_info.onGui = &SphereShape::OnGui;
		_info.init = &SphereShape::Init;
		_info.destroy = &SphereShape::Destroy;
		_info.load  = &SphereShape::Load;
		_info.save  = &SphereShape::Save;
		_info.editorPath = "/";
		_info.name = "sphere shape";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::Init( EcsWorld& _world, EcsComponent& _component )
	{
		SphereShape& sphereShape = static_cast<SphereShape&>( _component );
		sphereShape.sphereShape = new btSphereShape( 1.f );
		sphereShape.sphereShape->setUserPointer( nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::Destroy( EcsWorld& _world, EcsComponent& _component )
	{
		SphereShape& sphereShape = static_cast<SphereShape&>( _component );
		assert( sphereShape.sphereShape != nullptr );
		delete sphereShape.sphereShape;
		sphereShape.sphereShape = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		SphereShape& sphereShape = static_cast<SphereShape&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			float radius = sphereShape.GetRadius();
			if( ImGui::DragFloat( "radius##sphshapradius", &radius, 0.1f, 0.f ) )
			{
				sphereShape.SetRadius( radius );
			}
			//RendererDebug::Get().DebugSphere( m_gameobject->GetTransform().GetBtTransform(), radius, 2, Color::Green );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::Save( const EcsComponent& _component, Json& _json )
	{
		const SphereShape& sphereShape = static_cast<const SphereShape&>( _component );
		Serializable::SaveFloat( _json, "radius", sphereShape.GetRadius() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::Load( EcsComponent& _component, const Json& _json )
	{
		SphereShape& sphereShape = static_cast<SphereShape&>( _component );
		float radius;
		Serializable::LoadFloat( _json, "radius", radius );

		sphereShape.SetRadius( radius );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  SphereShape::SetRadius( const float _radius )
	{
		sphereShape.setUnscaledRadius( _radius >= 0 ? _radius : 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	float SphereShape::GetRadius() const
	{
		return sphereShape.getRadius();
	}
}