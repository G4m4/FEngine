#include "core/ecs/fanEcsSystem.hpp"
#include "engine/components/ui/fanUIButton.hpp"
#include "engine/components/ui/fanUITransform.hpp"
#include "engine/components/ui/fanUIRenderer.hpp"
#include "engine/singletons/fanMouse.hpp"
#include "engine/fanSceneTags.hpp"
#include "core/fanDebug.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct SHoverButtons : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UIButton>() |
                    _world.GetSignature<UITransform>() |
                    _world.GetSignature<TagUIEnabled>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            Mouse& mouse = _world.GetSingleton<Mouse>();

            auto buttonIt = _view.begin<UIButton>();
            auto transformUIIt = _view.begin<UITransform>();
            for( ; buttonIt != _view.end<UIButton>(); ++buttonIt, ++transformUIIt )
            {
                UIButton   & button    = *buttonIt;
                UITransform& transform = *transformUIIt;

                button.mIsHovered = mouse.mLocalPosition.x >= transform.mPosition.x &&
                                    mouse.mLocalPosition.y >= transform.mPosition.y &&
                                    mouse.mLocalPosition.x < transform.mPosition.x + transform.mSize.x &&
                                    mouse.mLocalPosition.y < transform.mPosition.y + transform.mSize.y;
                if( button.mIsHovered )
                {
                    ImGui::GetIO().WantCaptureMouse = true;
                }


                if( button.mIsHovered && mouse.mPressed[Mouse::buttonLeft] )
                {
                    button.mIsPressed = true;
                    button.mPressed.Emmit();
                }
                else if( !mouse.mDown[Mouse::buttonLeft] || !button.mIsHovered ){ button.mIsPressed = false; }
            }
        }
    };

    //========================================================================================================
    //========================================================================================================
    struct SHighlightButtons : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UIButton>() | _world.GetSignature<UIRenderer>();
        }

        static void Run( EcsWorld& /*_world*/, const EcsView& _view )
        {
            auto buttonIt = _view.begin<UIButton>();
            auto uiRendererIt = _view.begin<UIRenderer>();
            for( ; buttonIt != _view.end<UIButton>(); ++buttonIt, ++uiRendererIt )
            {
                UIButton  & button     = *buttonIt;
                UIRenderer& uiRenderer = *uiRendererIt;
                if( button.mIsPressed )
                {
                    uiRenderer.mColor = button.mColorHovered;
                    uiRenderer.mTexture = button.mImagePressed;
                }
                else if( button.mIsHovered )
                {
                    uiRenderer.mColor = button.mColorHovered;
                    uiRenderer.mTexture = button.mImageNormal;
                }
                else
                {
                    uiRenderer.mColor = button.mColorNormal;
                    uiRenderer.mTexture = button.mImageNormal;
                }
            }
        }
    };
}