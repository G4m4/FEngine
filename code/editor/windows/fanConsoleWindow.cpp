#include "editor/windows/fanConsoleWindow.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/time/fanProfiler.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
	//================================================================================================================================
	// ConsoleWindow::LogItem nested constructor
	//================================================================================================================================
	ConsoleWindow::LogItem::LogItem( const Debug::LogItem& _logItem )
	{
		severity = _logItem.severity;
		logType = _logItem.type;
		logMessage = Time::SecondsToString( _logItem.time ).c_str() + std::string( " " ) + _logItem.message;
		color = GetSeverityColor( _logItem.severity );
	}

	//================================================================================================================================
	//================================================================================================================================
	ConsoleWindow::ConsoleWindow() :
		EditorWindow( "console", ImGui::IconType::CONSOLE16 ),
		m_maxSizeLogBuffers( 256 ),
		m_firstLogIndex( 0 ),
		m_grabFocus( false )
	{
		m_inputBuffer[ 0 ] = '\0';
		m_logBuffer.reserve( m_maxSizeLogBuffers );
		Debug::Get().onNewLog.Connect( &ConsoleWindow::OnNewLog, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	ConsoleWindow::~ConsoleWindow()
	{
		Debug::Get().onNewLog.Clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ConsoleWindow::OnNewLog( Debug::LogItem _item )
	{
		m_scrollDown = true;

		if ( (int)m_logBuffer.size() < m_maxSizeLogBuffers )
		{
			m_logBuffer.push_back( LogItem( _item ) );
		}
		else
		{
			m_logBuffer[ m_firstLogIndex ] = LogItem( _item );
			m_firstLogIndex = ( m_firstLogIndex + 1 ) % m_maxSizeLogBuffers;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ConsoleWindow::OnGui( EcsWorld& /*_world*/ )
	{
		SCOPED_PROFILE(console)

		// List the logs
		const float height = ImGui::GetWindowHeight();
		if ( height > 60 )
		{
			ImGui::BeginChild( "scrolling", ImVec2( 0, height - 65 ), true, ImGuiWindowFlags_AlwaysVerticalScrollbar );
			for ( int logIndex = m_firstLogIndex; logIndex < m_firstLogIndex + (int)m_logBuffer.size(); logIndex++ )
			{
				const LogItem& item = m_logBuffer[ logIndex % m_maxSizeLogBuffers ];
				ImGui::TextColored( item.color, item.logMessage.c_str() );	// Time			
			}
			if ( m_scrollDown )
			{
				ImGui::SetScrollHereY( 1.0f );
				m_scrollDown = false;
			}
			ImGui::EndChild();
		}

		// Text input

		if ( m_grabFocus )
		{
			ImGui::SetKeyboardFocusHere();
			m_grabFocus = false;
		}

		// Icon
		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();

		// Input
		bool pressed = false;
		ImGui::PushItemWidth( ImGui::GetWindowWidth() - 90 );
		if ( ImGui::InputText( "##input_console", m_inputBuffer, IM_ARRAYSIZE( m_inputBuffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			pressed = true;
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Button
		if ( ImGui::Button( ">>" ) )
		{
			pressed = true;
		}
		if ( pressed == true )
		{
			const std::string message = m_inputBuffer;
			if ( message.size() > 0 )
			{
				if ( message != "clear" )
				{
					Debug::Get() << "Unknown command: " << message << Debug::Endl();
				}
				else
				{
					Debug::Get().Clear();
				}
				m_inputBuffer[ 0 ] = '\0';
				m_scrollDown = true;
				m_grabFocus = true;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	ImVec4 ConsoleWindow::GetSeverityColor( const Debug::Severity& _severity )
	{
		switch ( _severity )
		{
		case Debug::Severity::log:
		{
			return { 1,1,1,1 };		// White
		} break;
		case Debug::Severity::highlight:
		{
			return { 0,1,0,1 };		// Green
		} break;
		case Debug::Severity::warning:
		{
			return { 1,1,0,1 };		// Yellow
		} break;
		case Debug::Severity::error:
		{
			return { 1,0,0,1 };		// Red
		} break;
		default:
			assert( false );
			break;
		}
		return { 0,0,0,1 };		// Black
	}
}