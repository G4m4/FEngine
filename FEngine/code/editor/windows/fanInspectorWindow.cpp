#include "fanGlobalIncludes.h"

#include "editor/windows/fanInspectorWindow.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanMaterial.h"
#include "editor/components/fanFPSCamera.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	InspectorWindow::InspectorWindow() :
		EditorWindow("inspector")
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::OnGui() {

		if ( m_gameobjectSelected != nullptr)
		{
			// gameobject gui
			m_gameobjectSelected->OnGui();
			int componentCount = 0;			
			const std::vector<Component*> & components = m_gameobjectSelected->GetComponents();
			for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
				Component * component = components[componentIndex];

				ImGui::Separator();
				// Actor "enable" checkbox
				if (component->IsActor()) {	// TODO : use type info when type info deals with inheritance
					ImGui::PushID((int*)component);
					Actor * actor = static_cast<Actor*>(component);
					bool enabled = actor->IsEnabled();
					if (ImGui::Checkbox("", &enabled)) {
						actor->SetEnabled(enabled);
					}
					ImGui::SameLine();
					ImGui::PopID();
				}

				// Delete button
				ImGui::Text( component->GetName());
				if (component->IsRemovable()) {
					std::stringstream ss;
					ss << "X" << "##" << component->GetName() << componentCount++;	// make unique id
					ImGui::SameLine(ImGui::GetWindowWidth() - 40);
					if (ImGui::Button(ss.str().c_str())) {
						m_gameobjectSelected->RemoveComponent(component);
						component = nullptr;
					}
				}
				
				// Draw component
				if (component != nullptr) {
					component->OnGui();
				}
			}			
			ImGui::Separator();
			//Add component button
			if (ImGui::Button("Add component"))
				ImGui::OpenPopup("New component");

			NewComponentPopup();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void InspectorWindow::NewComponentPopup() {

		if (ImGui::BeginPopup("New component"))
		{
			std::vector<Component *>& components = ComponentsRegister::GetComponents();
			for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
				Component * component = components[componentIndex];
				if (ImGui::MenuItem(component->GetName())) {
					// Create new Component 
					m_gameobjectSelected->AddComponent(component->GetType());
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}
}