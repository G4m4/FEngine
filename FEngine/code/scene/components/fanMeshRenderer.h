#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	class Mesh;
	class AABB;
	struct ecsMesh;

	//================================================================================================================================
	//================================================================================================================================
	class MeshRenderer : public Component
	{
	public:
		static Signal< MeshRenderer * >				onRegisterMeshRenderer;
		static Signal< MeshRenderer * >				onUnRegisterMeshRenderer;

		void			SetPath( std::string _path );
		void			SetMesh(Mesh * _mesh);
		Mesh *			GetMesh();
		const Mesh *	GetMesh() const;

		int		GetRenderID() const;
		void	SetRenderID(const int _renderID);

		void OnGui() override;
		virtual ImGui::IconType GetIcon() const { return ImGui::IconType::MESH; };

		DECLARE_EDITOR_COMPONENT(MeshRenderer)
		DECLARE_TYPE_INFO(MeshRenderer, Component );

	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnAttach() override;
		void OnDetach() override;

	private:
		// Editor
		std::fs::path m_pathBuffer;
		ecsMesh *		const m_mesh = nullptr;
	};
}