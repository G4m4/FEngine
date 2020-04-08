#pragma once

#include "editor/fanEditorPrecompiled.hpp"

namespace fan
{
	struct EditorGrid;
	class EcsWorld;
	class EditorWindow;
	class EditorSelection;

	//================================================================================================================================
	// top main menu bar of the editor
	//================================================================================================================================
	class MainMenuBar
	{
	private:
		//================================================================
		//================================================================
		struct FPSCounter
		{
			int count = 0;
			float sum = 0.f;
			float fps = 0.f;
		};

	public:
		Signal< std::string > onSaveScene;
		Signal< std::string > onNewScene;
		Signal< std::string > onOpenScene;
		Signal<> onReloadShaders;
		Signal<> onReloadIcons;
		Signal<> onExit;

		MainMenuBar( EcsWorld& _world, EditorSelection& _editorSelection );
		~MainMenuBar();

		void SetGrid( EditorGrid* _editorGrid ) { m_editorGrid = _editorGrid; }

		void SetWindows( std::vector< EditorWindow* > _editorWindows );

		void Draw();

		bool ShowHull() const { return m_showHull; }
		bool ShowAABB() const { return m_showAABB; }
		bool ShowWireframe() const { return m_showWireframe; }
		bool ShowNormals() const { return m_showNormals; }
		bool ShowLights() const { return m_showLights; }

	private:
		EcsWorld* m_world;
		EditorGrid* m_editorGrid;
		EditorSelection& m_editorSelection;

		std::vector< EditorWindow* > m_editorWindows;

		bool m_showImguiDemoWindow;
		bool m_showHull;
		bool m_showAABB;
		bool m_showWireframe;
		bool m_showNormals;
		bool m_showLights;

		bool m_openNewScenePopupLater = false;
		bool m_openLoadScenePopupLater = false;
		bool m_openSaveScenePopupLater = false;

		FPSCounter m_fpsCounter;

		// Temporary buffers
		std::filesystem::path m_pathBuffer;
		std::set < std::string > m_sceneExtensionFilter;

		void DrawModals();
		void New();
		void Open();
		void Reload();
		void Save();
		void SaveAs();
	};
}