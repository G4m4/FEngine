#pragma once

#include "scene/components/fanActor.h"
#include "editor/windows/fanInspectorWindow.h"

namespace fan {
	namespace game {
		class Planet : public scene::Actor {
		public:
			bool IsUnique() const override { return true; }

			void Start() override;
			void Update(const float _delta) override;
			bool Load(std::istream& _in) override;
			bool Save(std::ostream& _out, const int _indentLevel) const override;

			DECLARE_TYPE_INFO(Planet);
		protected:
		private:
			static bool ms_registered;

			float m_speed  = 1.f;
			float m_radius = 1.f;
		};
	}
}