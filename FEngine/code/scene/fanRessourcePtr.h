#pragma once

namespace fan
{
	class Gameobject;
	class Texture;

	//================================================================================================================================
	// A gameobject pointer is used to reference a gameobject using a pointer and a id that can be serialized
	//================================================================================================================================
	template< typename _RessourceType, typename _IDType >
	class RessourcePtr
	{		
	public:
		static Signal< RessourcePtr * > s_onCreateUnresolved;
		static Signal< RessourcePtr * > s_onSetFromSelection;

		RessourcePtr( _RessourceType * _ressourceType, _IDType _ressourceID );		
		RessourcePtr();

		_IDType GetID( ) const { return m_ressourceId; }
		void	 InitUnresolved( const _IDType m_ressourceId );
		_RessourceType* operator->() const	{ return m_ressourcePtr; }
		_RessourceType* operator*() const{ return m_ressourcePtr; }

	private:
		_RessourceType *	m_ressourcePtr = nullptr;
		_IDType				m_ressourceId = 0;
	};

	template< typename _RessourceType, typename _IDType >
	Signal< RessourcePtr<_RessourceType, _IDType> * > RessourcePtr<_RessourceType, _IDType>::s_onCreateUnresolved;

	template< typename _RessourceType, typename _IDType >
	Signal< RessourcePtr<_RessourceType, _IDType> * >  RessourcePtr<_RessourceType, _IDType>::s_onSetFromSelection;


	//================================================================================================================================
	//================================================================================================================================
	template< typename _RessourceType, typename _IDType >
	RessourcePtr<_RessourceType, _IDType>::RessourcePtr( _RessourceType * _ressourceType, _IDType _ressourceID ) :
		m_ressourceId( _ressourceID ),
		m_ressourcePtr( _ressourceType )
	{}

	//================================================================================================================================
	// This constructor requires the scene to manually set its gameobject pointer
	// Useful at loading time
	//================================================================================================================================
	template< typename _RessourceType, typename _IDType >
	void RessourcePtr<_RessourceType, _IDType>::InitUnresolved( const _IDType _id )
	{
		m_ressourceId = _id;
		m_ressourcePtr = nullptr;
		s_onCreateUnresolved.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	template< typename _RessourceType, typename _IDType >
	RessourcePtr<_RessourceType, _IDType>::RessourcePtr() :
		m_ressourceId(),
		m_ressourcePtr( nullptr )
	{}

	using GameobjectPtr = RessourcePtr<Gameobject, uint64_t>;
	using TexturePtr = RessourcePtr<Texture, std::string >;
}




namespace ImGui
{
	void InputGameobject( const char * _label, fan::GameobjectPtr * _ptr );
	void InputTexture( const char * _label, fan::TexturePtr * _ptr );
}
