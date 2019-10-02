#include "fanGlobalIncludes.h"

#include "editor/windows/fanRenderWindow.h"
#include "editor/fanModals.h"
#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/fanMesh.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/pipelines/fanForwardPipeline.h"
#include "core/fanTime.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	RenderWindow::RenderWindow( Renderer * _renderer ) :
		EditorWindow("Renderer")
		, m_renderer( _renderer )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	void RenderWindow::OnGui() {
		RessourceManager * ressourceManager = m_renderer->GetRessourceManager();
		// Display mesh list
		if ( ImGui::CollapsingHeader( "Loaded meshes : " ) ) {
			for ( auto meshData : m_renderer->GetRessourceManager()->GetMeshData() ) {
				ImGui::Text( meshData.second->GetPath().c_str() );
			}
		}
		// display textures list
		const std::vector< Texture * > & textures = ressourceManager->GetTextures();
		if ( ImGui::CollapsingHeader( "Loaded textures : " ) ) {
			for ( int textureIndex = 0; textureIndex < textures.size(); textureIndex++ ) {
				const Texture * texture = textures[textureIndex];
				std::stringstream ss;
				ss << texture->GetSize().x << " " << texture->GetSize().x << "\t" << texture->GetPath();
				ImGui::Text( ss.str().c_str() );
			}
		}

		if ( ImGui::CollapsingHeader( "Rendered Models : " ) ) {
			const std::array< Mesh *, GlobalValues::s_maximumNumModels > & meshArray = m_renderer->GetMeshArray();
			for ( uint32_t meshIndex = 0; meshIndex < m_renderer->GetNumMesh(); meshIndex++ ) {
				const Mesh * mesh = meshArray[meshIndex];
				if ( mesh != nullptr ) {
					ImGui::Text( mesh->GetPath().c_str() );
				} else {
					ImGui::Text( "Empty slot" );
				}
			}
		}

		LightsUniforms & lights = m_renderer->GetForwardPipeline()->m_lightUniforms;
		if ( ImGui::CollapsingHeader( "Directional lights : " ) ) {
			for ( size_t lightIndex = 0; lightIndex < lights.dirLightsNum; lightIndex++ ) {
				DirectionalLightUniform light  = lights.dirLights[lightIndex];
				ImGui::PushItemWidth(150); ImGui::DragFloat3("dir ", &light.direction[0] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.diffuse[0], gui::colorEditFlags );
			}
		}
		if ( ImGui::CollapsingHeader( "Point lights : " ) ) {
			for ( size_t lightIndex = 0; lightIndex < lights.pointLightNum; lightIndex++ ) {
				PointLightUniform& light = lights.pointlights[lightIndex];
				ImGui::PushItemWidth( 150 ); ImGui::DragFloat3( "pos ##pos", &light.position[0] );
				ImGui::SameLine();
				ImGui::ColorEdit3( "diffuse", &light.diffuse[0], gui::colorEditFlags );
			}
		}
	}
}