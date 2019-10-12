#pragma once

#include "core/fanSignal.h"
#include "core/math/shapes/fanAABB.h"
#include "core/fanISerializable.h"
#include "core/ecs/fanECSManager.h"	
#include "scene/fanScene.h"


namespace fan
{
	class Component;
	class Scene;

	//================================================================================================================================
	//================================================================================================================================
	class Gameobject : public ISerializable {
	public:
		enum Flag {
			NONE = 0x00,
			NO_DELETE = 0x01,
			NOT_SAVED = 0x02,
		};

		Gameobject(const std::string _name, Gameobject * _parent, Scene * _scene );
		~Gameobject();

		void OnGui();
		
		// Get/add/delete components
		template<typename ComponentType> ComponentType*				 AddComponent();	
		template< typename _componentType >	bool					 AddEcsComponent() const;
		template<typename ComponentType> ComponentType*				 GetComponent();	
		template< typename _componentType >	_componentType*			 GetEcsComponent() const;
		template<typename ComponentType> std::vector<ComponentType*> GetComponents();	

		bool							DeleteComponent(const Component * _component);		
		Component*						AddComponent( const uint32_t _componentID );
		const std::vector<Component*> & GetComponents() const { return m_components; }

		// Getters
		std::string		GetName() const { return m_name; }
		void			SetName(const std::string _newName) { m_name = _newName; }
		inline Scene *	GetScene() const { return m_scene; }
		const AABB &	GetAABB() const;
		void			ComputeAABB();

		// Gameobject scene tree parenting
		Gameobject* GetParent() const { return m_parent; }
		const std::vector<Gameobject*>& GetChilds() const { return m_childs; }
		bool IsAncestorOf(const Gameobject * _node) const;
		void RemoveChild(const Gameobject * _child);
		bool HasChild(const Gameobject * _child);
		void AddChild(Gameobject * _child);
		void SetParent(Gameobject * _parent);
		void InsertBelow(Gameobject * _brother);

		// ISerializable
		bool LoadGameobject(std::istream& _in);
		bool Save(std::ostream& _out, const int _indentLevel) const override;

		// Flags
		bool		HasFlag(const Flag _flag) const { return m_flags & _flag; }
		uint32_t	GetFlags() const { return m_flags; }
		void		SetFlags(const uint32_t _flags) { m_flags = _flags; }

	private:
		std::string				 m_name;
		std::vector<Gameobject*> m_childs;
		Gameobject *			 m_parent;
		uint32_t				 m_flags;
		std::vector<Component*>  m_components;
		Scene *	const			 m_scene = nullptr;
		ecsHandle				 m_ecsHandleEntity = ecsNullHandle;

		bool m_computeAABB = true;

		bool Load(std::istream& _in) override;

		void AddComponent(Component * _component);
	};


	//================================================================================================================================
	// Creates an instance of ComponentType, adds it to the gameobject and returns a pointer
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType* Gameobject::AddComponent()
	{
		// Checks if ComponentType derivates from Component
		static_assert((std::is_base_of<Component, ComponentType>::value));

		ComponentType* component = new ComponentType();
		if (component->IsUnique() && GetComponent< ComponentType >() != nullptr) {
			delete(static_cast<Component*>(component));
			return nullptr;
		}

		AddComponent(component);

		return component;
	}

	//================================================================================================================================
	// Returns a pointer on the first instance of ComponentType in the gameobject, nullptr if none exists
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType* Gameobject::GetComponent()
	{
		for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
			Component* component = m_components[componentIndex];

			if (component->IsType<ComponentType>()) {
				return static_cast<ComponentType*>(component);
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	// Gets components of a specific type
	//================================================================================================================================
	template<typename ComponentType>
	std::vector<ComponentType*> Gameobject::GetComponents()
	{
		std::vector<ComponentType*> componentTypeVector;
		for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
			Component* component = m_components[componentIndex];
			if (component->IsType<ComponentType>()) {
				componentTypeVector.push_back(static_cast<ComponentType*>(component));
			}
		}
		return componentTypeVector;
	}

	//================================================================================================================================
	// Returns a component associated with the ecs entityof the gameobject
	//================================================================================================================================
	template< typename _componentType >
	_componentType* Gameobject::GetEcsComponent() const {
		static_assert( IsComponent< _componentType>::value );

		_componentType* component = m_scene->GetEcsManager()->FindComponent<_componentType>( m_ecsHandleEntity );
		assert( component  != nullptr );
		return component;
	}

	//================================================================================================================================
	// Adds a component to the ecs manager using the ecs entity of the gameobject
	//================================================================================================================================
	template< typename _componentType >	bool  Gameobject::AddEcsComponent() const {
		static_assert( IsComponent< _componentType>::value );

		ecsEntity entity;
		if ( m_scene->GetEcsManager()->FindEntity( m_ecsHandleEntity, entity ) ) {
			m_scene->GetEcsManager()->AddComponent<_componentType>(entity);
			return true;
		}
		return false;
	}
}