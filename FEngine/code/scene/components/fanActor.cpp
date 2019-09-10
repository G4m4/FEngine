#include "fanGlobalIncludes.h"

#include "scene/components/fanActor.h"
#include "scene/components/fanComponent.h"
#include "scene/fanEntity.h"
#include "core/fanSignal.h"
#include "fanEngine.h"

namespace fan
{
	namespace scene
	{	
		fan::Signal< Actor * > Actor::onActorAttach;
		fan::Signal< Actor * > Actor::onActorDetach;

		//================================================================================================================================
		//================================================================================================================================
		void Actor::OnAttach() {
			onActorAttach.Emmit(this);
		}

		//================================================================================================================================
		//================================================================================================================================
		void Actor::OnDetach() {
			onActorDetach.Emmit(this);
		}
	}
}