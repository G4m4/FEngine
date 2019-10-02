#pragma once

namespace fan
{
	struct EditorGrid;
	class Scene;
	class EditorWindow;

	//================================================================================================================================
	//================================================================================================================================
	class MainMenuBar {
	private:
		//================================================================
		//================================================================
		struct FPSCounter {
			int count = 0;
			float sum = 0.f;
			float fps = 0.f;
		};

	public:
		Signal< std::string > onSaveScene;
		Signal< std::string > onNewScene;
		Signal< std::string > onOpenScene;
		Signal<> onReloadShaders;
		Signal<> onExit;


		MainMenuBar( Scene & _scene, EditorGrid & _editorGrid );
		~MainMenuBar();
		void SetWindows( EditorWindow * _renderWindow,
						 EditorWindow * _sceneWindow, 
						 EditorWindow * _inspector,
						 EditorWindow * _preferences, 
						 EditorWindow * _console);

		void Initialize();
		void Draw();

		bool ShowHull() const { return m_showHull; }
		bool ShowAABB() const { return m_showAABB; }
		bool ShowWireframe() const { return m_showWireframe; }
		bool ShowNormals() const { return m_showNormals; }
	
	private:
		Scene & m_scene;
		EditorGrid & m_editorGrid;

		EditorWindow * m_renderWindow = nullptr;
		EditorWindow * m_sceneWindow = nullptr;
		EditorWindow * m_inspector = nullptr;
		EditorWindow * m_preferences = nullptr;
		EditorWindow * m_console = nullptr;

		bool m_showImguiDemoWindow;
		bool m_showHull;
		bool m_showAABB;
		bool m_showWireframe;
		bool m_showNormals;

		bool m_openNewScenePopupLater = false;
		bool m_openLoadScenePopupLater = false;
		bool m_openSaveScenePopupLater = false;

		FPSCounter m_fpsCounter;

		// Temporary buffers
		std::fs::path m_pathBuffer;
		int m_extensionIndexBuffer;
		std::set < std::string > m_sceneExtensionFilter;

		void ProcessKeyboardShortcuts();
		void DrawModals();
		void New();
		void Open();
		void Save();
		void SaveAs();
	};
}