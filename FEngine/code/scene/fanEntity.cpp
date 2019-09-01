#include "fanGlobalIncludes.h"

#include "scene/fanEntity.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanTransform.h"
#include "renderer/fanMesh.h"
#include "core/fanSignal.h"

namespace fan
{
	namespace scene
	{
		//================================================================================================================================
		//================================================================================================================================
		Entity::Entity(const std::string _name) :
			m_name(_name)
			, m_flags(Flag::NONE)
		{
			onComponentModified.Connect(&Entity::OnComponentModified, this);
			onComponentDeleted.Connect(&Entity::OnComponentDeleted, this);
		}

		//================================================================================================================================
		//================================================================================================================================
		Entity::~Entity() {
			for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
				m_components[componentIndex]->Delete();
			}
			for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
				delete m_components[componentIndex];
			}
			m_components.clear();
		}

		//================================================================================================================================
		//================================================================================================================================
		bool Entity::DeleteComponent(const Component * component)
		{
			// Find the component
			for (int componentIndex = 0; componentIndex < m_components.size(); ++componentIndex)
			{
				if (m_components[componentIndex] == component)
				{
					// Deletes it
					m_components[componentIndex]->m_isBeingDeleted = true;
					onComponentDeleted.Emmit(m_components[componentIndex]);
					m_components[componentIndex]->Delete();
					m_components.erase(m_components.begin() + componentIndex);
					return true;
				}
			}
			return false;
		}

		//================================================================================================================================
		//================================================================================================================================
		Component* Entity::AddComponent(const uint32_t _componentID) { 
			scene::Component * component = TypeInfo::Instantiate<Component>(_componentID);
			AddComponent(component);
			return component;
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::AddComponent(scene::Component * _component) {
			_component->m_entity = this;
			_component->Initialize();
			m_components.push_back(_component);
			onComponentCreated.Emmit(_component);
			onComponentModified.Emmit(_component);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::ComputeAABB() {

			const scene::Model * model = GetComponent< scene::Model >();
			if (model != nullptr && model->IsBeingDeleted() == false && model->GetMesh() != nullptr && model->GetMesh()->GetIndices().size() > 0) {
				m_aabb = model->ComputeAABB();
			}
			else {
				const btVector3 origin = GetComponent< scene::Transform >()->GetPosition();
				const float size = 0.05f;
				m_aabb = shape::AABB(origin - size * btVector3::One(), origin + size * btVector3::One());
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::OnComponentModified(scene::Component * _component) {
			if (_component->IsType<scene::Transform>() || _component->IsType<scene::Model>()) {
				ComputeAABB();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::OnComponentDeleted(scene::Component * _component) {
			if (_component->IsType<scene::Model>()) {
				ComputeAABB();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::Load(std::istream& _in) {
			std::string buffer;
			_in >> buffer;
			while (buffer != "end") {

				// Get component id
				uint32_t componentID;
				_in >> componentID;

				fan::Debug::Get() << fan::Debug::Severity::log << "\tComponent: " << buffer << std::endl;

				// Instanciate component

				scene::Component * component = AddComponent(componentID);
				component->Load(_in);
				_in >> buffer; // skip component name
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void Entity::Save(std::ostream& _out) {
			_out << "entity " << m_name << std::endl;
			for (int componentIndex = 0; componentIndex < m_components.size(); componentIndex++) {
				_out << "\t" << m_components[componentIndex]->GetName() << " ";
				_out << m_components[componentIndex]->GetType() << std::endl;
				m_components[componentIndex]->Save(_out);
			}
			_out << "end" << std::endl;
		}
	}
}