#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/resources/fanResource.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/fanPrefabManager.hpp"

namespace fan
{
	struct SceneNode;

	//================================================================================================================================
	// represents a gameobjects tree
	// stores its data in a json
	// Allow scene node tree instantiation ( copy / paste, prefabs )
	//================================================================================================================================
	class Prefab : public Resource
	{
	public:
		static PrefabManager s_resourceManager;

		bool CreateFromJson( const Json& _json );
		bool CreateFromFile( const std::string& _path );
		void CreateFromSceneNode( const SceneNode& _node );
		SceneNode* Instanciate( SceneNode& _parent ) const;

		const Json& GetJson() const { return m_json; }
		std::string GetPath() const { return m_path; }

		bool IsEmpty() const { return !m_json.contains( "prefab" ); }
		void Clear() { m_json = Json(); }

	private:
		Json m_json;
		std::string m_path;		
	};
}