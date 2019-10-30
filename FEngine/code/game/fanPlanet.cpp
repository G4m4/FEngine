#include "fanGlobalIncludes.h"
#include "game/fanPlanet.h"

#include "core/time/fanTime.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT(Planet)
	REGISTER_TYPE_INFO(Planet)

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Start() {
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnAttach() {
		Actor::OnAttach();

		ecsPlanet ** tmpPlanet = &const_cast<ecsPlanet*>( m_planet );
		*tmpPlanet = m_gameobject->AddEcsComponent<ecsPlanet>();
		m_planet->Init();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnDetach() {
		Actor::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsPlanet>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Update(const float /*_delta*/) {
		// Updates the parent entity
		ecsHandle handle = m_gameobject->GetParent()->GetEcsHandle();
		m_gameobject->GetScene()->GetEcsManager()->FindEntity( handle , m_planet->parentEntity );
		m_gameobject->AddFlag(Gameobject::Flag::OUTDATED_TRANSFORM);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::SetSpeed( const float _speed )		{ m_planet->speed = _speed; };
	void Planet::SetRadius( const float _radius )	{ m_planet->radius = _radius; };
	void Planet::SetPhase( const float _phase )		{ m_planet->phase = _phase; };

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnGui() {
		ImGui::DragFloat("radius", &m_planet->radius, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("speed",  &m_planet->speed,   0.1f, 0.f, 10.f);
		ImGui::DragFloat("phase",  &m_planet->phase, PI/3, 0.f, 2 * PI);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Load( Json & _json) {
		Actor::Load(_json);

		LoadFloat( _json, "radius", m_planet->radius );
		LoadFloat( _json, "speed",  m_planet->speed );
		LoadFloat( _json, "phase",  m_planet->phase );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Save( Json & _json ) const {

		SaveFloat( _json, "radius", m_planet->radius );
		SaveFloat( _json, "speed",  m_planet->speed );
		SaveFloat( _json, "phase",  m_planet->phase );
		Actor::Save( _json );
		
		return true;
	}
}