#include "fanGlobalIncludes.h"

#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanModel.h"
#include "scene/actors/fanActor.h"
#include "scene/fanGameobjectPtr.h"
#include "core/fanSignal.h"
#include "core/time/fanScopedTimer.h"
#include "core/time/fanProfiler.h"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	Scene::Scene(const std::string _name, EcsManager * _ecsManager, PhysicsManager * _physicsManager ) :
		m_name(_name)
		, m_path("")
		, m_root(nullptr) 
		, m_ecsManager(_ecsManager)
		, m_physicsManager( _physicsManager )
	{

		Actor::onActorAttach.Connect(&Scene::OnActorAttach, this);
		Actor::onActorDetach.Connect(&Scene::OnActorDetach, this);
	}

	//================================================================================================================================
	//================================================================================================================================
	Scene::~Scene() {
		Clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject *	Scene::CreateGameobject(const std::string _name, Gameobject * _parent) {
		if (_parent == nullptr) {
			_parent = m_root;
		}
		Gameobject* gameobject = new Gameobject(_name, _parent, this, GetUniqueID() );
		assert( m_gameobjects.find( gameobject->GetUniqueID() ) == m_gameobjects.end() );
		m_gameobjects[gameobject->GetUniqueID()] = gameobject;

		return gameobject;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::DeleteGameobject(Gameobject* _gameobject ) {
		m_entitiesToDelete.push_back(_gameobject );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::R_BuildEntitiesList(Gameobject* _gameobject, std::vector<Gameobject*>& _entitiesList) const {
		_entitiesList.push_back(_gameobject);
		const std::vector<Gameobject*>& childs = _gameobject->GetChilds();
		for (int childIndex = 0; childIndex < childs.size(); childIndex++) {
			R_BuildEntitiesList(childs[childIndex], _entitiesList);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Component *	Scene::R_FindComponentOfType(Gameobject * _gameobject, const uint32_t _typeID) const {

		// Search in components
		const std::vector<Component*> & components = _gameobject->GetComponents();
		for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
			Component* component = components[componentIndex];
			if (component->GetType() == _typeID) {
				return component;
			}
		}

		// Recursive call to child entities
		const std::vector<Gameobject*> & childEntities = _gameobject->GetChilds();
		for (int childIndex = 0; childIndex < childEntities.size(); childIndex++) {
			Component* component = R_FindComponentOfType(childEntities[childIndex], _typeID);
			if (component != nullptr) {
				return component;
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector < Gameobject * >  Scene::BuildEntitiesList() const {
		std::vector<Gameobject*> entitiesList;
		R_BuildEntitiesList(m_root, entitiesList);
		return entitiesList;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::BeginFrame() {
		SCOPED_PROFILE( scene_begin )
		for (Actor * actor : m_startingActors) {
			actor->Start();
			m_activeActors.insert(actor);
		}
		m_startingActors.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Update(const float _delta) {
		SCOPED_PROFILE( scene_update )
		for (Actor * actor : m_activeActors) {
			if (actor->IsEnabled()) {
				try	{ 
					actor->Update( _delta ); 
				}
				catch (...)
				{
					Debug::Error() << "Update error on actor " << actor->s_name << " of gameobject " << actor->GetGameobject()->GetName() << Debug::Endl();
					actor->SetEnabled(false);
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::LateUpdate( const float _delta )
	{
		SCOPED_PROFILE( scene_L_update )
		for ( Actor * actor : m_activeActors )
		{
			if ( actor->IsEnabled() )
			{
				try
				{
					actor->LateUpdate( _delta );
				}
				catch (...)
				{
					Debug::Error() << "LateUpdate error on actor " << actor->s_name << " of gameobject " << actor->GetGameobject()->GetName() << Debug::Endl();
					actor->SetEnabled( false );
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::SetMainCamera( Camera * _camera ) {
		if ( _camera != m_mainCamera ) {
			m_mainCamera = _camera;
			onSetMainCamera.Emmit( _camera );				
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject * Scene::FindGameobject( const uint64_t _id )
	{
		if ( _id != 0 )
		{
			auto it = m_gameobjects.find( _id );
			if ( it != m_gameobjects.end() )
			{
				return it->second;
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::InsertID( const uint64_t _id, Gameobject * _gameobject ) { 
		assert( m_gameobjects.find( _id ) == m_gameobjects.end() );
		m_gameobjects[_id] = _gameobject ; 
		if ( _id >= m_nextUniqueID )
		{
			m_nextUniqueID = _id + 1;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Play() {
		onScenePlay.Emmit(); 
		m_isPaused = false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Pause() {
		onScenePause.Emmit(); 
		m_isPaused = true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Scene::R_DeleteGameobject(Gameobject* _gameobject, std::set<Gameobject*>& _deletedEntitiesSet) {
		if (_gameobject != nullptr && _deletedEntitiesSet.find(_gameobject) == _deletedEntitiesSet.end()) {

			const std::vector<Gameobject*> childs = _gameobject->GetChilds(); // copy
			for (int childIndex = 0; childIndex < childs.size(); childIndex++) {
				R_DeleteGameobject(childs[childIndex], _deletedEntitiesSet);
			}

			onDeleteGameobject.Emmit( _gameobject );
			_deletedEntitiesSet.insert(_gameobject);
			if (_gameobject->GetParent() != nullptr) {
				_gameobject->GetParent()->RemoveChild(_gameobject);
			}
			m_gameobjects.erase( _gameobject->GetUniqueID() );
			delete(_gameobject);
		}
	}

	//================================================================================================================================
	// Deletes every gameobject in the m_toDeleteLater vector
	//================================================================================================================================
	void Scene::EndFrame() {
		SCOPED_PROFILE( scene_endFrame )
		// Delete entities 
		std::set<Gameobject*> deletedEntitiesSet;
		for (int gameobjectToDeleteIndex = 0; gameobjectToDeleteIndex < m_entitiesToDelete.size(); gameobjectToDeleteIndex++) {
			Gameobject * gameobjectDelete = m_entitiesToDelete[gameobjectToDeleteIndex];
			R_DeleteGameobject(gameobjectDelete, deletedEntitiesSet);
		}
		m_entitiesToDelete.clear();
	}

	//================================================================================================================================
	// Deletes every gameobject in the m_toDeleteLater vector
	//================================================================================================================================
	void Scene::OnGui() {
		int nb = (int)m_activeActors.size();
		ImGui::DragInt("nb", &nb);
		for (Actor * actor : m_activeActors)
		{
			ImGui::Text(actor->GetName());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::OnActorAttach(Actor * _actor) {
		assert(m_activeActors.find(_actor) == m_activeActors.end());
		assert(m_startingActors.find(_actor) == m_startingActors.end());

		m_startingActors.insert(_actor);

	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::OnActorDetach(Actor * _actor) {
		const size_t size = m_activeActors.erase(_actor);
		if (size == 0) {
			Debug::Get() << Debug::Severity::warning << " Scene::OnActorDetach Actor not active : " << _actor->GetName() << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Clear() {
		m_path = "";
		std::set<Gameobject*> deletedEntitiesSet;
		R_DeleteGameobject(m_root, deletedEntitiesSet);
		m_startingActors.clear();
		m_activeActors.clear();
		m_entitiesToDelete.clear();
		m_gameobjects.clear();
		m_root = nullptr;

		onSceneClear.Emmit();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::New() {
		Clear();
		m_root = CreateGameobject("root", nullptr);
		onSceneLoad.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::Save() const {	

		Debug::Get() << Debug::Severity::log << "saving scene: " << m_name << Debug::Endl();
		std::ofstream outStream(m_path);
		if (outStream.is_open()) {
			Json json;	
			if ( Save( json ) ) {
				// Out to disk
				outStream << json;
			}
			outStream.close();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::Save( Json& _json ) const {
		// Parameters
		Json & jParameters = _json["parameters"]; {
			jParameters["name"] = m_name;
			jParameters["path"] = m_path;
		}

		// Gameobjects
		Json & jGameobjects = _json["gameobjects"]; {
			if ( !m_root->Save( jGameobjects ) ) {
				return false;
			}
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::Load( Json & _json ) {
		//ScopedTimer timer("load scene");

		// Parameters
		Json & jParameters = _json["parameters"]; {
			LoadString( jParameters , "name" , m_name );
			LoadString( jParameters, "path", m_path );
		}

		// Gameobjects
		GameobjectPtr::s_onCreateUnresolved.Connect( &Scene::OnGameobjectPtrCreate, this );
		Json & jGameobjects = _json["gameobjects"]; {
			m_root = CreateGameobject( "root" );
			m_root->Load( jGameobjects );
		}
		GameobjectPtr::s_onCreateUnresolved.Disconnect( &Scene::OnGameobjectPtrCreate, this );
		ResolveGameobjectPointers();
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::OnGameobjectPtrCreate( GameobjectPtr * _gameobjectPtr )
	{
		m_unresolvedGameobjectPointers.push_back( _gameobjectPtr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Scene::ResolveGameobjectPointers()
	{
		for (int ptrIndex = 0; ptrIndex < m_unresolvedGameobjectPointers.size(); ptrIndex++)
		{
 			GameobjectPtr* ptr = m_unresolvedGameobjectPointers[ptrIndex];
			if( ptr->GetID() != 0 ) {
				auto it = m_gameobjects.find( ptr->GetID() );
				if ( it != m_gameobjects.end() )
				{
					(*ptr) = GameobjectPtr( it->second );
				}
				else
				{
					Debug::Warning() << "gameobject pointer resolution failed for id " << ptr->GetID() << Debug::Endl();
				} 				
			}
		}
		m_unresolvedGameobjectPointers.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Scene::LoadFrom(const std::string _path) {
		Clear();
		std::ifstream inStream(_path);
		if (inStream.is_open() && inStream.good()) {
			// Load scene
			Debug::Get() << Debug::Severity::log << "loading scene: " << _path << Debug::Endl();

			Json sceneJson;
			inStream >> sceneJson;
			if (Load( sceneJson )) {
				m_path = _path;
				inStream.close();
				onSceneLoad.Emmit(this);
				return true;
			}
			else {
				Debug::Get() << Debug::Severity::error << "failed to load scene: " << _path << Debug::Endl();
				m_path = "";
				inStream.close();
				New();
				return false;
			}

		}
		else {
			Debug::Get() << Debug::Severity::error << "failed to open file " << _path << Debug::Endl();
			New();
			return false;
		}
	}
}