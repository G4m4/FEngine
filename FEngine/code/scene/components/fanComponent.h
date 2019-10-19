 #pragma once

#include "core/fanISerializable.h"
#include "core/fanTypeInfo.h"
#include "ecs/fanECSComponents.h"

//Editor
#include "scene/fanComponentsRegister.h"
#include "editor/fanImguiIcons.h"

namespace fan
{
	class Gameobject;

	//================================================================================================================================
	//================================================================================================================================
	class Component : public ISerializable {
	public:
		friend class Gameobject;

		// Returns a reference on the gameobject the component is bound to
		inline Gameobject* GetGameobject() const { return m_gameobject; }
		bool IsBeingDeleted() const { return m_isBeingDeleted; }
		bool IsModified() const;
		void MarkModified(const bool _updateAABB = false);
		bool IsRemovable() const { return m_isRemovable; }
		void SetRemovable(const bool _isRemovable) { m_isRemovable = _isRemovable; }

		virtual bool IsCollider()	const { return false; }
		virtual bool IsActor()		const { return false; }

		virtual void OnGui();
		virtual ImGui::IconType GetIcon() const { return ImGui::IconType::NONE; };

		DECLARE_ABSTRACT_TYPE_INFO(Component, ISerializable );
	protected:
		// Friend class gameobject is the factory of components
		Component();
		virtual ~Component() {}
		virtual void OnAttach();
		virtual void OnDetach();

		bool Save( Json & _json ) const override;

	private:
		uint64_t m_lastModified;	// Frame index at which it was modified
		bool m_isBeingDeleted : 1;
		bool m_isRemovable : 1;
		Gameobject * m_gameobject = nullptr;
	};
}