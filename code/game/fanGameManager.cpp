#include "game/fanGameManager.hpp"
#include "game/components/fanSpaceShip.hpp"	
#include "core/input/fanKeyboard.hpp"
#include "core/time/fanTime.hpp"


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void GameManager::Start()
	{
// 		if ( *m_gameCamera == nullptr )
// 		{
// 			Debug::Warning( "Game manager has no camera attached" );
// 			SetEnabled( false );
// 			//m_gameobject->GetScene().SetMainCamera( *m_gameCamera );
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnAttach()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnDetach()
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::Update( const float /*_delta*/ )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void GameManager::OnGui()
	{
// 		Actor::OnGui();
// 
// 		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
// 		{
// 			ImGui::FanComponent( "game camera", m_gameCamera );
// 			ImGui::Text( "Press tab to switch cameras" );
// 		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameManager::Load( const Json& _json )
	{
// 		Actor::Load( _json );
// 		Serializable::LoadComponentPtr( _json, m_gameobject->GetScene(), "game_camera", m_gameCamera );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool GameManager::Save( Json& _json ) const
	{
// 		Actor::Save( _json );
// 		Serializable::SaveComponentPtr( _json, "game_camera", m_gameCamera );
		return true;
	}
}