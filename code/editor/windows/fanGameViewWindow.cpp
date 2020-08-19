#include "editor/windows/fanGameViewWindow.hpp"

#include "network/singletons/fanTime.hpp"
#include "scene/singletons/fanScene.hpp"
#include "game/singletons/fanGame.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	GameViewWindow::GameViewWindow( const LaunchSettings::Mode _launchMode )
		: EditorWindow( "game view", ImGui::IconType::JOYSTICK16 )
		, m_isHovered( false )
	{
		AddFlag( ImGuiWindowFlags_MenuBar );

		// compute game world str for
		switch( _launchMode )
		{
            case LaunchSettings::Mode::EditorClient:
                memcpy( m_gameWorldsStr, "client\0\0", 8 );
                break;
            case LaunchSettings::Mode::EditorServer:
                memcpy( m_gameWorldsStr, "server\0\0", 8 );
                break;
            case LaunchSettings::Mode::EditorClientServer:
                memcpy( m_gameWorldsStr, "client\0server\0\0,", 16 );
                break;
            default:
                fanAssert( false );
                break;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void GameViewWindow::OnGui( EcsWorld& _world )
	{
		// update window size
		const ImVec2 imGuiSize = ImGui::GetContentRegionAvail();
		btVector2 size = btVector2( imGuiSize.x, imGuiSize.y );
		if ( m_size != size )
		{
			m_size = size;
			onSizeChanged.Emmit( { (uint32_t)size[0], (uint32_t)size[1] } );
		}

		// draw menu bar
		if ( ImGui::BeginMenuBar() )
		{
			
			ImGui::Text( "%d x %d", ( int ) size.x(), ( int ) size.y() );

			const ImVec4 disabledColor = ImVec4( 0.3f, 0.3f, 0.3f, 0.3f );

            const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
			if ( playState.mState == EditorPlayState::STOPPED )
			{
				// Play
                if( ImGui::ButtonIcon( ImGui::PLAY16,
                                       { 16, 16 },
                                       -1,
                                       ImVec4( 0, 0, 0, 0 ),
                                       ImVec4( 1.f, 1.f, 1.f, 1.f ) ) )
                {
					onPlay.Emmit();
				}
			}
			else
			{
				// Stop
				if ( ImGui::ButtonIcon( ImGui::STOP16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
				{
					onStop.Emmit();
				}
			}

			const ImVec4 pauseTint
				= playState.mState == EditorPlayState::PLAYING ? ImVec4( 1.f, 1.f, 1.f, 1.f )
				: playState.mState == EditorPlayState::PAUSED ? ImVec4( 0.9f, 0.9f, 0.9f, 1.f )
				: disabledColor;

			// Pause
			if ( ImGui::ButtonIcon( ImGui::PAUSE16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0.f ), pauseTint ) )
			{
				if ( playState.mState == EditorPlayState::PLAYING ) { onPause.Emmit(); }
				else if ( playState.mState == EditorPlayState::PAUSED ) { onResume.Emmit(); }
			}

            // Step
            const ImVec4 stepTint = playState.mState == EditorPlayState::PAUSED
                    ? ImVec4( 1.f, 1.f, 1.f, 1.f )
                    : disabledColor;
            if( ImGui::ButtonIcon( ImGui::STEP16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0 ), stepTint ) &&
                playState.mState == EditorPlayState::PAUSED )
			{
				onStep.Emmit();
			}

			// combo current world			
			ImGui::Spacing();
			ImGui::SetNextItemWidth( 200.f );
			if( ImGui::Combo( "##current game", &m_currentGameSelected, m_gameWorldsStr ) )
			{
				onSelectGame.Emmit( m_currentGameSelected );
			}

			ImGui::EndMenuBar();
		}

		// update window position
		const ImVec2 cursorPos = ImGui::GetCursorPos();
		const ImVec2 windowPos = ImGui::GetWindowPos();

		m_position = btVector2( cursorPos.x + windowPos.x, cursorPos.y + windowPos.y );

		// Draw game
		ImGui::Image( ( void* ) 12, imGuiSize );

		m_isHovered = ImGui::IsItemHovered();
	}
}