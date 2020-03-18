#include "editor/windows/fanSceneWindow.hpp"

#include "editor/fanDragnDrop.hpp"
#include "editor/fanModals.hpp"
#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanPrefabManager.hpp"
#include "scene/fanScene.hpp"
#include "core/input/fanKeyboard.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanMouse.hpp"
#include "scene/fanPrefab.hpp"

#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanDirectionalLight.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "scene/components/fanParticleEmitter.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::SceneWindow() :
		EditorWindow( "scene", ImGui::IconType::SCENE16 )
	{
		m_textBuffer[0] = '\0';
	}

	//================================================================================================================================
	//================================================================================================================================
	SceneWindow::~SceneWindow() {}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::OnGui()
	{
		SCOPED_PROFILE( scene );

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( m_scene->GetName().c_str() );
 		ImGui::SameLine();
 		ImGui::Text( m_scene->IsServer() ? "- server" : "- client" );
 		ImGui::Separator();

		// Draws all scene nodes
		SceneNode* nodeRightClicked = nullptr;
		R_DrawSceneTree( m_scene->GetRootNode(), nodeRightClicked );

 		m_expandSceneHierarchy = false;
 
		if( nodeRightClicked != nullptr )
		{
			ImGui::OpenPopup( "scene_window_node_rclicked" );
			m_lastSceneNodeRightClicked = nodeRightClicked;
 		}
 
		PopupRightClick();
// 
// 		// load prefab popup
// 		if( loadPrefabPopup )
// 		{
// 			m_pathBuffer = "content/prefab";
// 			ImGui::OpenPopup( "Load prefab" );
// 		}
// 		if( ImGui::FanLoadFileModal( "Load prefab", RenderGlobal::s_prefabExtensions, m_pathBuffer ) )
// 		{
// 			Prefab* prefab = Prefab::s_resourceManager.LoadPrefab( m_pathBuffer.string() );
// 			if( prefab != nullptr )
// 			{
// 				m_scene->CreateGameobject( *prefab, m_lastGameobjectRightClicked );
// 			}
// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::PopupRightClick()
	{
		// Popup set gameobject when right click
		bool newNodePopup = false;
		bool renameNodePopup = false;
		bool exportToPrefabPopup = false;
		bool loadPrefabPopup = false;

		if( ImGui::BeginPopup( "scene_window_node_rclicked" ) )
		{
			// New scene node 
			bool itemClicked = false;
			if( ImGui::BeginMenu( "New node" ) )
			{
				assert( m_lastSceneNodeRightClicked != nullptr );
				Scene& scene = *m_lastSceneNodeRightClicked->scene;
				EcsWorld& world = scene.GetWorld();				
				btVector3& origin = btVector3::Zero();
				const EntityID parentID = world.GetEntityID( m_lastSceneNodeRightClicked->entityHandle );
				if( world.HasComponent<Transform>( parentID ) )
				{
					origin = world.GetComponent<Transform>( parentID ).GetPosition();
				}

				// Popup empty gameobject
				if( ImGui::IsItemClicked() )
				{
					itemClicked = true;
				}

				// Entities templates
				ImGui::Icon( ImGui::CUBE_SHAPE16, { 16,16 } ); ImGui::SameLine();
				
				// model 
				if( ImGui::MenuItem( "Model" ) )
				{
					SceneNode& node = scene.InstanciateSceneNode( "model", m_lastSceneNodeRightClicked );
					const EntityID entityID = world.GetEntityID( node.entityHandle );
					
					Transform& transform = world.AddComponent<Transform>( entityID );
					transform.SetPosition( origin );
					
					MeshRenderer& meshRenderer = world.AddComponent<MeshRenderer>( entityID );	
					meshRenderer.mesh = Mesh::s_resourceManager.GetMesh( RenderGlobal::s_meshSphere );
					
					Material& material = world.AddComponent<Material>( entityID );
					material.texture = Texture::s_resourceManager.GetTexture( RenderGlobal::s_textureWhite );
					onSelectSceneNode.Emmit( &node );
				}

				// model with a rigidbody & shape
				ImGui::Icon( ImGui::RIGIDBODY16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Physics model" ) )
				{
					SceneNode& node = scene.InstanciateSceneNode( "physics_model", m_lastSceneNodeRightClicked );
					const EntityID entityID = world.GetEntityID( node.entityHandle );
					
					Transform& transform = world.AddComponent<Transform>( entityID );
					transform.SetPosition( origin );
					
					MeshRenderer& meshRenderer = world.AddComponent<MeshRenderer>( entityID );
					meshRenderer.mesh = Mesh::s_resourceManager.GetMesh( RenderGlobal::s_meshCube );
					Material& material = world.AddComponent<Material>( entityID );
					material.texture = Texture::s_resourceManager.GetTexture( RenderGlobal::s_textureWhite );
					onSelectSceneNode.Emmit( &node );
					
					Rigidbody& rigidbody = world.AddComponent<Rigidbody>( entityID );
					MotionState& motionState = world.AddComponent<MotionState>( entityID );
					BoxShape& shape = world.AddComponent<BoxShape>( entityID );
					rigidbody.SetKinematic();
					rigidbody.SetMotionState( &motionState.motionState );
					rigidbody.SetCollisionShape( &shape.boxShape );
					world.GetSingletonComponent<PhysicsWorld>().dynamicsWorld->addRigidBody( &rigidbody.rigidbody );
				}

				// point light
				ImGui::Icon( ImGui::POINT_LIGHT16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Point light" ) )

				{
					SceneNode& node = scene.InstanciateSceneNode( "point_light", m_lastSceneNodeRightClicked );
					const EntityID entityID = world.GetEntityID( node.entityHandle );
					
					Transform& transform = world.AddComponent<Transform>( entityID );
					transform.SetPosition( origin );
					
					PointLight& light = world.AddComponent<PointLight>( entityID );
					onSelectSceneNode.Emmit( &node );
				}

				// directional light
				ImGui::Icon( ImGui::DIR_LIGHT16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "Dir light" ) )
				{
					SceneNode& node = scene.InstanciateSceneNode( "directional_light", m_lastSceneNodeRightClicked );
					const EntityID entityID = world.GetEntityID( node.entityHandle );
					
					Transform& transform = world.AddComponent<Transform>( entityID );
					transform.SetPosition( origin  );
					
					DirectionalLight& light = world.AddComponent<DirectionalLight>( entityID );
					transform.SetRotationEuler( btVector3(30.f,10.f,0.f) );
					onSelectSceneNode.Emmit( &node );
				}

				// particle system
				ImGui::Icon( ImGui::PARTICLES16, { 16,16 } ); ImGui::SameLine();
				if( ImGui::MenuItem( "particle system" ) )
				{
					SceneNode& node = scene.InstanciateSceneNode( "particle_system", m_lastSceneNodeRightClicked );
					const EntityID entityID = world.GetEntityID( node.entityHandle );
					
					Transform& transform = world.AddComponent<Transform>( entityID );
					transform.SetPosition( origin );

					ParticleEmitter& emitter = world.AddComponent<ParticleEmitter>( entityID );
					transform.SetPosition( origin );
				}

				ImGui::EndMenu();
			}

			if( ImGui::IsItemClicked() )
			{
				newNodePopup = true;
			}

			if( ImGui::MenuItem( "Import prefab" ) )
			{
				loadPrefabPopup = true;
			}

			ImGui::Separator();

			// rename
			if( ImGui::Selectable( "Rename" ) )
			{
				renameNodePopup = true;
			}

			// export to prefab
			if( ImGui::Selectable( "Export to prefab" ) )
			{
				exportToPrefabPopup = true;
			}

			// delete
			ImGui::Separator();
			if( ImGui::Selectable( "Delete" ) && m_lastSceneNodeRightClicked != nullptr )
			{
				m_scene->DeleteSceneNode( *m_lastSceneNodeRightClicked );
			}
			ImGui::EndPopup();
		}

		// new entity modal
		if( newNodePopup )
		{
			ImGui::OpenPopup( "new_scenenode" );
		} NewGameobjectModal();

		// rename modal
		if( renameNodePopup )
		{
			ImGui::OpenPopup( "rename_scenenode" );
		} RenameGameobjectModal();

		// export to prefab modal
		if( exportToPrefabPopup )
		{
			m_pathBuffer = "content/prefab";
			ImGui::OpenPopup( "export_to_prefab" );
		} ExportToPrefabModal();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::R_DrawSceneTree( SceneNode& _node, SceneNode*& _nodeRightClicked )
	{

		std::stringstream ss;
		ss << "##" << &_node; // Unique id

		if( ImGui::IsWindowAppearing() || m_expandSceneHierarchy == true )
		{
			ImGui::SetNextItemOpen( true );
		}
		bool isOpen = ImGui::TreeNode( ss.str().c_str() );

		// Gameobject dragndrop target empty selectable -> place dragged below
		SceneNode* nodeDrop1 = ImGui::FanBeginDragDropTargetSceneNode();
		if( nodeDrop1 && nodeDrop1 != &_node )
		{
			nodeDrop1->InsertBelow( _node );
		}


		ImGui::SameLine();
		bool selected = ( &_node == m_sceneNodeSelected );

		// Draw gameobject empty selectable to display a hierarchy
		std::stringstream ss2;
		ss2 << _node.name << "##" << &_node; // Unique id
		if( ImGui::Selectable( ss2.str().c_str(), &selected ) )
		{
			onSelectSceneNode.Emmit( &_node );
		}
		if( ImGui::IsItemClicked( 1 ) )
		{
			_nodeRightClicked = &_node;
		}

		// SceneNode dragndrop source = selectable -^
		ImGui::FanBeginDragDropSourceSceneNode( _node );

		// SceneNode dragndrop target gameobject name -> place as child
		SceneNode* nodeDrop = ImGui::FanBeginDragDropTargetSceneNode();
		if( nodeDrop )
		{
			nodeDrop->SetParent( &_node );
		}

		if( isOpen )
		{
			for( int childIndex = 0; childIndex < _node.childs.size(); childIndex++ )
			{
				SceneNode* child = _node.childs[childIndex];
				R_DrawSceneTree( *child, _nodeRightClicked );
			}

			ImGui::TreePop();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::NewGameobjectModal()
	{
		ImGui::SetNextWindowSize( ImVec2( 200, 200 ) );
		if( ImGui::BeginPopupModal( "new_scenenode" ) )
		{
			if( ImGui::IsWindowAppearing() )
			{
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
			if( ImGui::InputText( "Name ", m_textBuffer, IM_ARRAYSIZE( m_textBuffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				enterPressed = true;
			}
			if( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
			{
				m_lastSceneNodeRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if( ImGui::Button( "Ok" ) || enterPressed )
			{
				if( std::string( m_textBuffer ) != "" )
				{
					//Create new scene node 
					SceneNode& newNode = m_scene->InstanciateSceneNode( m_textBuffer, m_lastSceneNodeRightClicked );
					onSelectSceneNode.Emmit( &newNode );
					m_lastSceneNodeRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::RenameGameobjectModal()
	{
		ImGui::SetNextWindowSize( ImVec2( 200, 200 ) );
		if( ImGui::BeginPopupModal( "rename_scenenode" ) )
		{
			if( ImGui::IsWindowAppearing() )
			{
				strcpy_s( m_textBuffer, 32, m_lastSceneNodeRightClicked->name.c_str() );
				ImGui::SetKeyboardFocusHere();
			}
			bool enterPressed = false;
			if( ImGui::InputText( "New Name ", m_textBuffer, IM_ARRAYSIZE( m_textBuffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
			{
				enterPressed = true;
			}
			if( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
			{
				m_lastSceneNodeRightClicked = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if( ImGui::Button( "Ok" ) || ImGui::IsKeyPressed( GLFW_KEY_ENTER, false ) || enterPressed )
			{
				if( std::string( m_textBuffer ) != "" )
				{
					m_lastSceneNodeRightClicked->name = m_textBuffer;
					m_lastSceneNodeRightClicked = nullptr;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneWindow::ExportToPrefabModal()
	{
		if( m_lastSceneNodeRightClicked == nullptr )
		{
			return;
		}

		if( ImGui::FanSaveFileModal( "export_to_prefab", RenderGlobal::s_prefabExtensions, m_pathBuffer ) )
		{
			Debug::Log() << "Exporting prefab to " << m_pathBuffer.string() << Debug::Endl();

			std::ofstream outStream( m_pathBuffer.string() );
			if( outStream.is_open() )
			{
				// Try to update the existing prefab if it exists
// 				Prefab* prefab = Prefab::s_resourceManager.FindPrefab( m_pathBuffer.string() );
// 				if( prefab != nullptr )
// 				{
// 					prefab->CreateFromGameobject( *m_lastSceneNodeRightClicked );
// 				}
// 
// 				Prefab newprefab;
// 				newprefab.CreateFromGameobject( *m_lastSceneNodeRightClicked );
// 				outStream << newprefab.GetJson();

				outStream.close();
			}
			else
			{
				Debug::Warning() << "Prefab export failed : " << m_pathBuffer.string() << Debug::Endl();
			}
		}
	}
}