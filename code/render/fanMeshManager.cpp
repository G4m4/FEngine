#include "render/fanMeshManager.hpp"

#include "render/fanMesh.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void MeshManager::Init( Device& _device )
	{
		m_device = &_device;
		ResourcePtr< Mesh >::s_onResolve.Connect( &MeshManager::ResolvePtr, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshManager::Clear()
	{
		while ( !m_meshList.empty() ) 
		{ 
			delete m_meshList.begin()->second; 
			m_meshList.erase( m_meshList.begin() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshManager::ResolvePtr( ResourcePtr< Mesh >& _resourcePtr )
	{
		assert( ! _resourcePtr.IsValid() );

		MeshPtr& meshPtr = static_cast< MeshPtr& >( _resourcePtr );
		Mesh * mesh = LoadMesh( meshPtr.GetPath() );

		if ( mesh )
		{
			_resourcePtr.SetResource( * mesh );
		}
	}

	//================================================================================================================================
	// Load a mesh from a path, loads it and registers it
	//================================================================================================================================
	Mesh* MeshManager::LoadMesh( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

		const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();

		Mesh* mesh = FindMesh( cleanPath );
		if ( mesh != nullptr )
		{
			return mesh;
		}
		else
		{
			// Load
			mesh = new Mesh();			
			if ( mesh->LoadFromFile( cleanPath ) )
			{	
				m_meshList[cleanPath] = mesh;
				return mesh;
			}
			delete mesh;
			return nullptr;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh* MeshManager::FindMesh( const std::string& _path )
	{
		auto it = m_meshList.find( _path );
		return it == m_meshList.end() ? nullptr : it->second;
	}

}