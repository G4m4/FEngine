#include "fanGlobalIncludes.h"
#include "editor/windows/fanEcsWindow.h"

#include "ecs/fanECSManager.h"
#include "core/time/fanProfiler.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	EcsWindow::EcsWindow( EcsManager * _ecsManager ) :
		EditorWindow("ecs", ImGui::IconType::ECS )
		, m_ecsManager( _ecsManager )	
	{}

	//================================================================
	// helper function to create a formatted string like "Storage: 1024 (16Ko)"
	//================================================================
	std::string TagCountSize( const char * _tag, const size_t _count, const size_t _size ) {
		std::stringstream ssStorage;
		ssStorage << _tag << ": " << _count << " (" << _count * _size / 1000 << "Ko)";
		return ssStorage.str();
	}

	//================================================================================================================================
	// Helper to draw all the components data
	//================================================================================================================================
	template <typename _componentsList > class DisplayHelper;
	template < template <typename...> typename _componentsList, typename... _components >
	class DisplayHelper<_componentsList<_components...> > {
	public:

		template<typename ... _list > struct DisplayImpl;

		// Specialization 
		template<> struct DisplayImpl<> {
			static void Display( const ecsComponentsTuple< ecsComponents >& /*_tuple*/ ) {}
		};

		// General case
		template<typename _component, typename ... _list>
		struct DisplayImpl< _component, _list...> : DisplayImpl<_list...> {
			static_assert( IsComponent<_component>::value );
			static void Display( const ecsComponentsTuple< ecsComponents >& _tuple ) {

				const ComponentData<_component>& data = _tuple.Get< _component >();
				const std::vector< Chunck<_component> * >& chuncks =  data.GetChuncks();

				size_t numElements = 0;
				for ( int chunckIndex = 0; chunckIndex < chuncks.size(); chunckIndex++ )
				{
					numElements += chuncks[chunckIndex]->Count();
				}

				ImGui::Text( _component::s_name );
				if ( ImGui::IsItemHovered() )
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted( "to recycle" );
					for ( int chunckIndex = 0; chunckIndex < chuncks.size(); chunckIndex++ )
					{
						ImGui::Text( std::to_string( chuncks[chunckIndex]->RecycleCount() ).c_str() );
					}
					ImGui::EndTooltip();
				}

				ImGui::NextColumn();
				ImGui::Text( std::to_string( numElements ).c_str() ); // num elements

				ImGui::NextColumn();
				ImGui::Text( std::to_string( chuncks.size() ).c_str() ); // num chuncks

				ImGui::NextColumn();
				ImGui::Text( std::to_string( chuncks.size() * sizeof( Chunck<_component> ) / 1000 ).c_str() ); // size of a chunck in Ko

				ImGui::NextColumn();

				DisplayImpl<_list...>::Display( _tuple );
			}
		};

		static void Display( const ecsComponentsTuple< ecsComponents >& _tuple ) {
			ImGui::Columns(4);
// 			ImGui::SetColumnWidth( 0, 100.f);
// 			ImGui::SetColumnWidth( 1, 100.f );
// 			ImGui::SetColumnWidth( 2, 100.f );
// 			ImGui::SetColumnWidth( 3, 100.f );

			ImGui::NextColumn();
			ImGui::Text("components");
			ImGui::NextColumn();
			ImGui::Text( "chuncks" );
			ImGui::NextColumn();
			ImGui::Text( "size (Ko)" );
			ImGui::NextColumn();
			ImGui::Separator();

			DisplayImpl<_components...>::Display( _tuple );
			ImGui::Columns( 1 );
		}
	};

	//================================================================================================================================
	//================================================================================================================================
	void EcsWindow::OnGui() {
		SCOPED_PROFILE( ecs )

		const std::vector<ecsComponentsKey>& entitiesData = m_ecsManager->GetEntitiesData();
		const ecsComponentsTuple< ecsComponents >& components = m_ecsManager->GetComponents();
		const std::unordered_map< ecsHandle, ecsEntity > & handlesToEntity = m_ecsManager->GetHandles();


		// Entities info
		ImGui::Icon( GetIconType(), { 19,19 } ); ImGui::SameLine();
		ImGui::Text( TagCountSize( "Entities  ", entitiesData.size(), sizeof( ecsComponentsKey ) ).c_str() );

		ImGui::Separator();

		// Components Info
		DisplayHelper<ecsComponents>::Display( components );

		// Test
		if ( ImGui::CollapsingHeader( "Testing" ) ) {
			// Components & tags selection
			static bool s_transform, s_particle, s_movement, s_scale;
			ImGui::Checkbox( "transform", &s_transform );
			ImGui::SameLine (); ImGui::Checkbox( "movement", &s_movement );
			ImGui::SameLine (); ImGui::Checkbox( "particle", &s_particle );
			ImGui::SameLine (); ImGui::Checkbox( "scale", &s_scale );
			static bool s_fakeTag;
			ImGui::Checkbox( "fakeTag", &s_fakeTag );

			// Entity creation
			ImGui::PushItemWidth( 100 );
			static int nb = 1;
			if ( ImGui::Button( "Create Entities" ) ) {
				for ( int i = 0; i < nb; i++ ) {
					ecsEntity entity =  m_ecsManager->CreateEntity();
					if ( s_transform )   m_ecsManager->AddComponent<ecsTranform>( entity );
					if ( s_movement ) 	 m_ecsManager->AddComponent<ecsMovement>( entity );
					if ( s_particle )	 m_ecsManager->AddComponent<ecsParticle>( entity );
					if ( s_scale )		 m_ecsManager->AddComponent<ecsScaling>( entity );
					if ( s_fakeTag )	 m_ecsManager->AddTag<ecsFakeTag>( entity );
				}
			} ImGui::SameLine (); 
			ImGui::DragInt( "nb", &nb, 1, 1, 100000 );

			// Create handle
			static int entitySelect = 0;
			if ( ImGui::Button( "Create handle  " ) ) {
				m_ecsManager->CreateHandle( entitySelect );
			} ImGui::SameLine();
			ImGui::DragInt( "handleIndex", &entitySelect );
			ImGui::PopItemWidth();
		}

		// Entities list
		if ( ImGui::CollapsingHeader( "Entities" ) ) {
			for ( int entityIndex = 0; entityIndex < entitiesData.size(); entityIndex++ ) {
				const ecsComponentsKey & data = entitiesData[entityIndex];
				std::stringstream ss;
				ss << data.bitset.to_string() << " " << entityIndex;
				ImGui::Text( ss.str().c_str() );
			}
		}

		// Entities handles
		if ( ImGui::CollapsingHeader( "Handles" ) ) {
			for ( auto& handlePair : handlesToEntity ) {
				ImGui::Text( ( std::to_string( handlePair.first ) + " " + std::to_string( handlePair.second ) ).c_str() );
			}
		}		
	}
}