#include "fanIncludes.h"
#include "fanIncludes.h"

#include "editor/windows/fanInspectorWindow.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanMesh.h"
#include "editor/components/fanFPSCamera.h"
#include "util/fanUtil.h"
#include "util/fbx/fanFbxImporter.h"
#include "util/fanImguiUtil.h"
#include "util/fanSignal.h"
#include "vulkan/vkRenderer.h"
#include "fanEngine.h"

namespace editor {
	InspectorWindow::InspectorWindow() : 
		m_cachePathMesh(".") 
		,m_meshExtensionFilter({".fbx"}) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::Draw() {
		if (IsVisible() == true) {
			fan::Engine & engine = fan::Engine::GetEngine();
			//scene::Scene & scene = engine.GetScene();
			scene::Gameobject * const selection = engine.GetSelectedGameobject();

			bool visible = IsVisible();
			ImGui::Begin("Inspector", &visible );
			if ( selection != nullptr )
			{
				// Gameobject gui
				ImGui::Text("GameObject : %s", selection->GetName().c_str());
				int componentCount = 0;

				const std::vector<scene::Component*> & components = selection->GetComponents();
				for (int componentIndex = 0; componentIndex < components.size() ; componentIndex++) {
					scene::Component * component =  components[componentIndex];

					ImGui::Separator();

					// Delete button

					if ( component->IsRemovable()) {
						std::stringstream ss;
						ss << "X" << "##" << component->GetName() << componentCount++;	// make unique id
						if (ImGui::Button(ss.str().c_str())) {
							selection->DeleteComponent(component);
							component = nullptr;
						} ImGui::SameLine();
					}
					if(component != nullptr){
						DrawComponent(*component);
					}

						/*scene::Material* mat = dynamic_cast<scene::Material*>(component);
						if (mat)
							if (ImGui::Button("Set"))
								ImGui::OpenPopup("Set Material");*/
					

				}
				ImGui::Separator();

				if (ImGui::BeginPopup("Set Material"))
				{
	// 				for (auto data : m_pEditorApplication->GetRenderer()->GetTextureData())
	// 				{
	// 					// Material
	// 					if (ImGui::MenuItem(data.second.path.c_str()))
	// 					{
	// 						//Create new Component 
	// 						scene::Material* mat = m_pEditorApplication->GetSelectedGameobject()->GetComponent<scene::Material>();
	// 						mat->SetTextureKey(data.first);
	// 						ImGui::CloseCurrentPopup();
	// 					}
	// 				}
					ImGui::EndPopup();
				}

				ImGui::SameLine();

				//Add component button
				if (ImGui::Button("Add component"))
					ImGui::OpenPopup("New component");

				NewComponentPopup();

			} ImGui::End();
			SetVisible( visible );
		}		
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup() {

		if (ImGui::BeginPopup("New component"))
		{
			fan::Engine & engine = fan::Engine::GetEngine();
			scene::Gameobject * const selection = engine.GetSelectedGameobject();

			// Mesh
			if (ImGui::MenuItem("Mesh")){
				selection->AddComponent<scene::Mesh>();
				ImGui::CloseCurrentPopup();
			}

			// Camera
			if (ImGui::MenuItem("Camera")) {
				// Create new Component 
				selection->AddComponent<scene::Camera>();
				ImGui::CloseCurrentPopup();
			}

			// Material
			/*if (ImGui::MenuItem("Material"))
			{
				// Create new Component 
				scene::Material* mat = m_pEditorApplication->GetSelectedGameobject()->AddComponent<editor::Material>();
				mat->SetTextureKey(m_pEditorApplication->GetRenderer()->GetDefaultTexture());
				ImGui::CloseCurrentPopup();
			}*/

			// Material
			/*if (ImGui::MenuItem("RigidBody"))
			{
				// Create new Component 
				m_pEditorApplication->GetSelectedGameobject()->AddComponent<editor::Rigidbody>();
				ImGui::CloseCurrentPopup();
			}*/

			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawComponent(scene::Component & _component) {
		const std::type_info& typeInfo = typeid(_component);

		if( typeInfo == typeid(scene::Transform)){
			DrawTransform(static_cast<scene::Transform &>(_component));
		} else if (typeInfo == typeid(scene::Camera)) {
			DrawCamera(static_cast<scene::Camera &>(_component));
		} else if (typeInfo == typeid(scene::Mesh)) {
			DrawMesh(static_cast<scene::Mesh &>(_component));
		} else if (typeInfo == typeid(scene::FPSCamera)) {
			DrawFPSCamera(static_cast<scene::FPSCamera &>(_component));
		}
		else {
			ImGui::Text( (std::string("Component not supported: ") + std::string(_component.GetName())).c_str());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawCamera(scene::Camera & _camera) {
		ImGui::Text(_camera.GetName().c_str());


		// fov
		if (ImGui::Button("##fov")) {
			_camera.SetFov(110.f);
		}
		ImGui::SameLine();
		float fov = _camera.GetFov();
		if (ImGui::DragFloat("fov", &fov, 1.f, 1.f, 179.f)) {
			_camera.SetFov(fov);
		}	

		// fov
		if (ImGui::Button("##nearDistance")) {
			_camera.SetNearDistance(0.1f);
		}
		ImGui::SameLine();
		float near = _camera.GetNearDistance();
		if (ImGui::DragFloat("near distance", &near, 0.01f, 0.01f, 10.f)) {
			_camera.SetNearDistance(near);
		}		
		
		// fov
		if (ImGui::Button("##fardistance")) {
			_camera.SetFarDistance(1000.f);
		}
		ImGui::SameLine();
		float far = _camera.GetFarDistance();
		if ( ImGui::DragFloat("far distance", &far, 10.f, 0.05f, 10000.f)) {
			_camera.SetFarDistance(far);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawTransform(scene::Transform & _transform) {
		ImGui::Text(_transform.GetName().c_str());

		// Position
		if (ImGui::Button("##TransPos")) {
			_transform.SetPosition(glm::vec3(0, 0, 0));
		} ImGui::SameLine();
		float posBuffer[3] = { _transform.GetPosition().x, _transform.GetPosition().y, _transform.GetPosition().z };
		if (ImGui::DragFloat3("position", posBuffer, 0.1f)) {
			_transform.SetPosition(glm::vec3(posBuffer[0], posBuffer[1], posBuffer[2]));
		}

		// rotation
		if (ImGui::Button("##TransRot")) {
			_transform.SetRotationEuler(glm::vec3(0, 0, 0));
		} ImGui::SameLine(); 	
		const glm::vec3 rot = _transform.GetRotationEuler();
		float bufferAngles[3] = { rot.x,rot.y,rot.z};
		if (ImGui::DragFloat3("rotation", bufferAngles, 0.1f))	{
			_transform.SetRotationEuler(glm::vec3(bufferAngles[0], bufferAngles[1], bufferAngles[2]));
		}

		// Scale
		if (ImGui::Button("##TransScale")) {
			_transform.SetScale(glm::vec3(1, 1, 1));
		} ImGui::SameLine();
		glm::vec3 scale = _transform.GetScale();
		if (ImGui::DragFloat3("scale", &scale.x, 0.1f)) {
			_transform.SetScale(scale);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawMesh(scene::Mesh & _mesh) {
		ImGui::Text(_mesh.GetName().c_str());

		// Set path popup
		bool openSetPathPopup = false;
		if (ImGui::Button("##setPath")) {
			openSetPathPopup = true;
		}
		ImGui::SameLine();
		ImGui::Text("path: %s", _mesh.GetPath().c_str());
		// Set path  popup on double click
		if (openSetPathPopup || ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
			if (_mesh.GetPath().empty() == false) {
				m_cachePathMesh = std::experimental::filesystem::path(_mesh.GetPath()).parent_path();
			} else {
				m_cachePathMesh = "./";
			}
			ImGui::OpenPopup("set_path");
		}

		if (ImGui::BeginPopup("set_path"))
		{
			std::experimental::filesystem::directory_entry newEntry = util::Imgui::FilesSelector(m_cachePathMesh, m_meshExtensionFilter);
			if (std::experimental::filesystem::is_directory(newEntry)) {
				m_cachePathMesh = newEntry.path();
			}
			else if (std::experimental::filesystem::is_regular_file(newEntry))
			{
				const std::string path = newEntry.path().string();

				util::FBXImporter importer;
				if (importer.LoadScene(path) == true ) {
					if (importer.GetMesh(_mesh)) {
						fan::Engine::GetEngine().GetRenderer().AddMesh(&_mesh);
					}
				}
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::DrawFPSCamera(scene::FPSCamera & _fpsCamera) {
		ImGui::Text(_fpsCamera.GetName().c_str());



		// SetSensitivity
		if (ImGui::Button("##SetSensitivity")) {
			_fpsCamera.SetXYSensitivity( glm::vec2(0.005f, 0.005f) );
		} ImGui::SameLine();
		glm::vec2 xySensitivity = _fpsCamera.GetXYSensitivity();
		if (ImGui::DragFloat2("XY sensitivity", &xySensitivity.x, 1.f)) {
			_fpsCamera.SetXYSensitivity(xySensitivity);
		}

		// SetSpeed
		if (ImGui::Button("##SetSpeed")) {
			_fpsCamera.SetSpeed(10.f);
		} ImGui::SameLine();
		float speed = _fpsCamera.GetSpeed();
		if (ImGui::DragFloat("speed", &speed, 1.f)) {
			_fpsCamera.SetSpeed(speed);
		}

		// SetSpeedMultiplier
		if (ImGui::Button("##SetSpeedMultiplier")) {
			_fpsCamera.SetSpeedMultiplier(3.f);
		} ImGui::SameLine();
		float speedMultiplier = _fpsCamera.GetSpeedMultiplier();
		if (ImGui::DragFloat("speed multiplier", &speedMultiplier, 1.f)) {
			_fpsCamera.SetSpeedMultiplier(speedMultiplier);
		}

	}
}