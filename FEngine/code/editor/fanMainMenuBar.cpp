#include "fanGlobalIncludes.h"

#include "fanGlobals.h"
#include "fanEngine.h"

#include "editor/fanMainMenuBar.h"
#include "editor/windows/fanEditorWindow.h"
#include "editor/fanEditorGrid.h"
#include "core/files/fanSerializedValues.h"
#include "core/input/fanInput.h"
#include "core/input/fanInputManager.h"
#include "core/time/fanProfiler.h"
#include "core/time/fanTime.h"
#include "editor/fanModals.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanCamera.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::MainMenuBar( Scene & _scene, EditorGrid & _editorGrid ) :
		m_scene( _scene )
		,m_editorGrid( _editorGrid )
		,m_showImguiDemoWindow(true)
		, m_showAABB(false)
		, m_showHull(false)
		, m_showWireframe(false)
		, m_showNormals(false)
		, m_sceneExtensionFilter(GlobalValues::s_sceneExtensions) {

		SerializedValues::Get().GetBool( "show_imguidemo", m_showImguiDemoWindow );
		SerializedValues::Get().GetBool( "editor_grid_show", m_editorGrid.isVisible );
		SerializedValues::Get().GetFloat( "editor_grid_spacing", m_editorGrid.spacing );
		SerializedValues::Get().GetInt( "editor_grid_linesCount", m_editorGrid.linesCount );
		SerializedValues::Get().GetColor( "editor_grid_color", m_editorGrid.color );
		SerializedValues::Get().GetVec3( "editor_grid_offset", m_editorGrid.offset );

		Input::Get().Manager().FindEvent( "open_scene")->Connect( &MainMenuBar::Open, this );
		Input::Get().Manager().FindEvent( "save_scene" )->Connect( &MainMenuBar::Save, this );
		Input::Get().Manager().FindEvent( "reload_scene" )->Connect( &MainMenuBar::Reload, this );
		Input::Get().Manager().FindEvent( "reload_icons" )->Connect( &Signal<>::Emmit, &this->onReloadIcons );
	}

	//================================================================================================================================
	//================================================================================================================================
	MainMenuBar::~MainMenuBar() {
		SerializedValues::Get().SetBool( "show_imguidemo",			m_showImguiDemoWindow );
		SerializedValues::Get().SetBool( "editor_grid_show",		m_editorGrid.isVisible );
		SerializedValues::Get().SetFloat( "editor_grid_spacing",	m_editorGrid.spacing );
		SerializedValues::Get().SetInt( "editor_grid_linesCount",	m_editorGrid.linesCount );
		SerializedValues::Get().SetColor( "editor_grid_color",		m_editorGrid.color );
		SerializedValues::Get().SetVec3( "editor_grid_offset",		m_editorGrid.offset );

		for (int windowIndex = 0; windowIndex < m_editorWindows.size() ; windowIndex++) {
			delete m_editorWindows[windowIndex];
		}


	}

	//================================================================================================================================
	// All editor windows are drawn & deleted with the main menubar
	//================================================================================================================================
	void MainMenuBar::SetWindows( std::vector< EditorWindow * > _editorWindows )	{
		m_editorWindows = _editorWindows;
	}

	//================================================================================================================================
	// Draws the main menu bar and the editor windows
	//================================================================================================================================
	void MainMenuBar::Draw() {
		SCOPED_PROFILE( main_bar )


		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos( viewport->Pos );
		ImGui::SetNextWindowSize( viewport->Size );

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		//fillscreen
		{
			ImGui::SetNextWindowPos( viewport->Pos );
			ImGui::SetNextWindowSize( viewport->Size );
			ImGui::SetNextWindowViewport( viewport->ID );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
			ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		bool p_open = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	   

		
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		ImGuiID dockspace_id = ImGui::GetID( "MyDockSpace" );
		ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), dockspace_flags );
		ImGui::End();

		// Draw editor windows
		for ( int windowIndex = 0; windowIndex < m_editorWindows.size(); windowIndex++ )
		{
			m_editorWindows[windowIndex]->Draw();
		}

		// Draw imgui demo
		if ( m_showImguiDemoWindow )
		{
			ImGui::ShowDemoWindow( &m_showImguiDemoWindow );
		}
		
		// Draw main menu  bar
		if ( ImGui::BeginMainMenuBar())
		{
			// File
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) {
					New();
				}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					Open();
				}
				if ( ImGui::MenuItem( "Reload", "Ctrl+R" ) ) {
					Reload();
				}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {
					Save();
				}
				if (ImGui::MenuItem("Save as")) {
					SaveAs();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Reload shaders", "F5")) {
					onReloadShaders.Emmit();
				}

				if ( ImGui::MenuItem( "Reload icons", "F6") )
				{
					onReloadIcons.Emmit();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) {
					onExit.Emmit();
				}

				ImGui::EndMenu();
			}

			// View
			if (ImGui::BeginMenu("View"))
			{
				ImGui::Icon( ImGui::IMGUI16, { 16,16 } ); ImGui::SameLine();
				ImGui::MenuItem( "Imgui demo", nullptr, &m_showImguiDemoWindow );

				for (size_t windowIndex = 0; windowIndex < m_editorWindows.size() ; windowIndex++) {
					EditorWindow * window = m_editorWindows[ windowIndex ];
					ImGui::Icon( window->GetIconType(), {16,16} ); ImGui::SameLine();					
					bool showWindow = window->IsVisible();
					if ( ImGui::MenuItem( window->GetName().c_str(), nullptr, &showWindow ) ) {
						window->SetVisible( showWindow );
					}
				}
				ImGui::EndMenu();
			}

			// Editor
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::MenuItem("show hull",		 nullptr, &m_showHull)) {}
				if (ImGui::MenuItem("show AABB",		 nullptr, &m_showAABB)) {}
				if (ImGui::MenuItem("show Wireframe", nullptr, &m_showWireframe)) {}
				if (ImGui::MenuItem("show Normals",	 nullptr, &m_showNormals)) {}
				ImGui::EndMenu();
			}

			// Grid
			if ( ImGui::BeginMenu( "Grid" ) )  {
				ImGui::PushItemWidth( 150.f ); 
				ImGui::MenuItem( "visible", nullptr, &m_editorGrid.isVisible );
				ImGui::DragFloat( "spacing", &m_editorGrid.spacing, 0.25f, 0.f, 100.f );
				ImGui::DragInt( "lines count", &m_editorGrid.linesCount, 1.f, 0, 1000 );
				ImGui::ColorEdit3( "color", &m_editorGrid.color[0], ImGui::fanColorEditFlags );
				ImGui::DragFloat3( "offset", &m_editorGrid.offset[0]);
				ImGui::PopItemWidth();

				ImGui::EndMenu();
			}

			// Framerate
			ImGui::SameLine(ImGui::GetWindowWidth() - 60);
			if ( ImGui::BeginMenu( std::to_string(Time::Get().GetRealFramerate()).c_str(), false ) ) {ImGui::EndMenu();}
			ImGui::FanToolTip(" Framerate. (Right click to set)");

			if ( ImGui::IsItemClicked( 1 ) ) {
				ImGui::OpenPopup( "main_menu_bar_set_fps" );
			}

			// Framerate set popup
			if ( ImGui::BeginPopup( "main_menu_bar_set_fps" ) ) {
				ImGui::PushItemWidth( 80.f );
				float maxFps = 1.f / Time::Get().GetRenderDelta();
				if ( ImGui::DragFloat( "fps", &maxFps, 1.f, 1.f, 3000.f, "%.f" ) ) {
					Time::Get().SetRenderDelta( maxFps < 1.f ? 1.f : 1.f / maxFps );
				}
				float maxLogicFrequency = 1.f / Time::Get().GetLogicDelta();
				if ( ImGui::DragFloat( "logic frequency", &maxLogicFrequency, 1.f, 1.f, 3000.f, "%.f" ) ) {
					Time::Get().SetLogicDelta( maxLogicFrequency < 1.f ? 1.f : 1.f / maxLogicFrequency );
				}
				float maxPhysicsFrequency = 1.f / Time::Get().GetPhysicsDelta();
				if ( ImGui::DragFloat( "physics frequency", &maxPhysicsFrequency, 1.f, 1.f, 3000.f, "%.f" ) ) {
					Time::Get().SetPhysicsDelta( maxPhysicsFrequency < 1.f ? 1.f : 1.f / maxPhysicsFrequency );
				}
				ImGui::PopItemWidth();
				ImGui::EndPopup();
			}

		} ImGui::EndMainMenuBar();

		
		// Open scene popup
		if (m_openNewScenePopupLater == true) {
			m_openNewScenePopupLater = false;
			ImGui::OpenPopup("New scene");
		}

		// Open scene popup
		if (m_openLoadScenePopupLater == true) {
			m_openLoadScenePopupLater = false;
			ImGui::OpenPopup("Open scene");
		}

		// Save scene popup
		if (m_openSaveScenePopupLater == true) {
			m_openSaveScenePopupLater = false;
			ImGui::OpenPopup("Save scene");
		}

		DrawModals();

	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::DrawModals() {
		// New scene
		if (ImGui::FanSaveFileModal("New scene", GlobalValues::s_sceneExtensions, m_pathBuffer)) {
			m_scene.New();
			m_scene.SetPath(m_pathBuffer.string());
		}

		// Open scene
		if (ImGui::FanLoadFileModal("Open scene", m_sceneExtensionFilter, m_pathBuffer)) {
			m_scene.LoadFrom(m_pathBuffer.string());
		}

		// Save scene
		if (ImGui::FanSaveFileModal("Save scene", GlobalValues::s_sceneExtensions, m_pathBuffer )) {
			m_scene.SetPath(m_pathBuffer.string());
			m_scene.Save();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::New() {
		m_pathBuffer = "./content/scenes/";
		m_openNewScenePopupLater = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Open() {
		m_pathBuffer = "./content/scenes/";
		m_openLoadScenePopupLater = true;
	}

	//================================================================================================================================
	// reload the scene
	//================================================================================================================================
	void MainMenuBar::Reload() {

		// Save camera data
		Json cameraData;
		m_scene.GetMainCamera()->GetGameobject()->Save(cameraData);

		// save old selection
		Gameobject* prevSelection = Globals::Get().engine->GetSelectedGameobject();
		const uint64_t id = prevSelection != nullptr ? prevSelection->GetUniqueID() : 0; 

		m_scene.LoadFrom( m_scene.GetPath() );

		// restore camera
		m_scene.GetMainCamera()->GetGameobject()->CopyDataFrom( cameraData );

		// restore selection
		if( id != 0 ) {
			Gameobject* selection = m_scene.FindGameobject( id );
			Globals::Get().engine->SetSelectedGameobject( selection );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::Save() {		
		if ( m_scene.HasPath() ) {
			m_scene.Save();
		}
		else {
			SaveAs();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MainMenuBar::SaveAs() {
		m_pathBuffer = "./content/scenes/";
		m_openSaveScenePopupLater = true;
	}
}