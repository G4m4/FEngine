#include "fanIncludes.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanModel.h"
#include "vulkan/core/vkTexture.h"
#include "vulkan/fanTexturesManager.h"
#include "vulkan/vkRenderer.h"

namespace scene {
	REGISTER_TYPE_INFO(Material)
		
	util::Signal< Material * > Material::onMaterialCreated;
	util::Signal< Material * > Material::onMaterialDeleted;

	//================================================================================================================================
	//================================================================================================================================
	void Material::Initialize() {
		onMaterialCreated.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::Delete() {
		onMaterialDeleted.Emmit(this);
		fan::Debug::Get() << "delete: " << this << std::endl;
		//delete this;
	}
		
	//================================================================================================================================
	//================================================================================================================================
	void Material::Load(std::istream& _in) {
		std::string path;
		_in >> path;
		if (path != std::string("void")) {
			// TODO find a cleaner way to set the texture
			vk::TexturesManager * texturesManager = vk::Renderer::GetRenderer().GetTexturesManager();
			vk::Texture * texture = texturesManager->FindTexture(path);
			if (texture == nullptr) {
				texture = texturesManager->LoadTexture(path);
			}
			SetTexture(texture);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::Save(std::ostream& _out) {

		_out << "\t\t" << (m_texture != nullptr ? m_texture->GetPath() : "void" ) << std::endl;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Material::SetTexture(vk::Texture * _texture) {
		m_texture = _texture;
		SetModified( true );
	}
}