#include "fanGlobalIncludes.h"
#include "scene/components/fanRigidbody.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( Rigidbody );
	REGISTER_TYPE_INFO( Rigidbody )

	//================================================================================================================================
	//================================================================================================================================	
	Rigidbody::Rigidbody() {

	}

	//================================================================================================================================
	//================================================================================================================================	
	Rigidbody::~Rigidbody() {

	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Load( Json & /*_json*/ ) {
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Save( Json & _json ) const {
		Component::Save( _json );
		return true;
	}
}