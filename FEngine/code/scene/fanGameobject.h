#pragma once

#include "util/fanSignal.h"
#include "util/shapes/fanAABB.h"
#include "util/fanISerializable.h"

namespace scene
{
	class Component;

	class Gameobject : public fan::ISerializable {
	public:
		enum Flag {
			NONE = 0x00,
			NO_DELETE = 0x01,
			NOT_SAVED = 0x02,
		};

		util::Signal<Component*> onComponentCreated;
		util::Signal<Component*> onComponentDeleted;
		util::Signal<Component*> onComponentModified;

		Gameobject( const std::string _name);
		~Gameobject();

		// Creates an instance of ComponentType, adds it to the GameObject and returns a pointer
		template<typename ComponentType>
		ComponentType* AddComponent();

		// Returns a pointer on the first instance of ComponentType in the GameObject, nullptr if none exists
		template<typename ComponentType>
		ComponentType* GetComponent();

		template<typename ComponentType>
		std::vector<ComponentType*> GetComponents();

		// Remove the component from the GameObject and deletes it
		bool DeleteComponent(const Component * _component);

		// Returns the component vector
		const std::vector<Component*> & GetComponents() const { return m_components; }

		//Getters
		std::string GetName() { return m_name; }
		const shape::AABB & GetAABB() const { return m_aabb; }
		void ComputeAABB();

		// ISerializable
		void Load(std::istream& _in) override;
		void Save(std::ostream& _out) override;

		bool HasFlag(const Flag _flag) { return m_flags & _flag; }
		uint32_t GetFlags() const {	return m_flags;	}
		void SetFlags( const uint32_t _flags) { m_flags = _flags; }

	private:
		std::string m_name;
		uint32_t m_flags;

		shape::AABB m_aabb;

		std::vector<Component*> m_components;

		void AddComponent( scene::Component * _component );
		void OnComponentModified( scene::Component * _component );
		void OnComponentDeleted( scene::Component * _component);

	};


	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType* Gameobject::AddComponent()
	{
		// Checks if ComponentType derivates from Component
		static_assert((std::is_base_of<Component, ComponentType>::value));

		const ComponentType * sample = Component::GetSample<ComponentType>(); // this is probably very bad

		// Checks if ComponentType is unique and doesn't isn't already added to the GameObject	
		if (sample->IsUnique() && GetComponent<ComponentType>() != nullptr) {
			return nullptr;
		}
		ComponentType* componentType = new ComponentType(this);
		AddComponent(componentType);

		return componentType;
	}

	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	ComponentType* Gameobject::GetComponent()
	{
		ComponentType * componentType;
		for (Component* component : m_components)
		{
			componentType = dynamic_cast<ComponentType*>(component);
			if (componentType != nullptr)
				return componentType;
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	template<typename ComponentType>
	std::vector<ComponentType*> Gameobject::GetComponents()
	{
		std::vector<ComponentType*> componentTypeVector;
		for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
			scene::Component* component = m_components[componentIndex];
			if (component->IsType<ComponentType>()) {
				componentTypeVector.push_back(static_cast<ComponentType*>(component));
			}
		}
		return componentTypeVector;
	}
}