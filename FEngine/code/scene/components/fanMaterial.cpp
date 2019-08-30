#include "fanIncludes.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanModel.h"
#include "vulkan/core/vkTexture.h"

namespace scene {
	REGISTER_TYPE_INFO(Material)

	util::Signal< scene::Model * > Material::onMaterialUpdated;

	//================================================================================================================================
	//================================================================================================================================
	void Material::Initialize() {
	
	}
		
	//================================================================================================================================
	//================================================================================================================================
	void Material::Load(std::istream& /*_in*/) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::Save(std::ostream& /*_out*/) {
	}	

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetTexture(vk::Texture * _texture) {
		m_texture = _texture;
		scene::Model * model = GetGameobject()->GetComponent<scene::Model>();
		if (model != nullptr) {
			onMaterialUpdated.Emmit(model);
		}
	}
}