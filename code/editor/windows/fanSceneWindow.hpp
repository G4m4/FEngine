#pragma once

#include <filesystem>
#include "core/fanSignal.hpp"
#include "editor/windows/fanEditorWindow.hpp"

namespace fan
{
	struct Scene;
	struct SceneNode;

	//================================================================================================================================
	// shows the scene nodes tree
	//================================================================================================================================
	class SceneWindow : public EditorWindow
	{
	public:
		Signal< SceneNode* > onSelectSceneNode;

		SceneWindow( Scene& _scene );
		~SceneWindow();

		// Callbacks
		void OnSceneNodeSelected( SceneNode* node ) { m_sceneNodeSelected = node; }
		void OnExpandHierarchy( Scene& /*_scene*/ ) { m_expandSceneHierarchy = true; }

	protected:
		void OnGui() override;

	private:
		Scene* m_scene;
		SceneNode* m_sceneNodeSelected;


		std::filesystem::path m_pathBuffer;
		char m_textBuffer[ 32 ];
		SceneNode* m_lastSceneNodeRightClicked = nullptr;
		bool m_expandSceneHierarchy = false;

		void NewGameobjectModal();
		void RenameGameobjectModal();
		void ExportPrefabModal();
		void ImportPrefabModal();
		void PopupRightClick();

		void R_DrawSceneTree( SceneNode& _node, SceneNode*& _nodeRightClicked );
	};
}
