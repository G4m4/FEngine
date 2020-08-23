#include <scene/singletons/fanRenderResources.hpp>
#include <scene/singletons/fanSceneResources.hpp>
#include <editor/singletons/fanEditorPlayState.hpp>
#include "editor/fanEditorHolder.hpp"

#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "network/singletons/fanTime.hpp"
#include "render/fanRenderer.hpp"
#include "render/fanFont.hpp"
#include "editor/windows/fanPreferencesWindow.hpp"	
#include "editor/windows/fanSingletonsWindow.hpp"
#include "editor/windows/fanInspectorWindow.hpp"	
#include "editor/windows/fanProfilerWindow.hpp"	
#include "editor/windows/fanConsoleWindow.hpp"
#include "editor/windows/fanRenderWindow.hpp"	
#include "editor/windows/fanSceneWindow.hpp"	
#include "editor/windows/fanGameViewWindow.hpp"
#include "editor/windows/fanEcsWindow.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorCopyPaste.hpp"
#include "editor/singletons/fanEditorGizmos.hpp"
#include "editor/fanMainMenuBar.hpp"
#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "scene/singletons/fanRenderWorld.hpp"
#include "scene/fanFullscreen.hpp"
#include "scene/components/fanPointLight.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/systems/fanUpdateTransforms.hpp"
#include "scene/systems/fanDrawDebug.hpp"
#include "scene/systems/fanUpdateRenderWorld.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/fanSceneTags.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    EditorHolder::EditorHolder( const LaunchSettings _settings, std::vector<IGame*> _games ) :
            mGames( _games ), mApplicationShouldExit( false ), mLaunchSettings( _settings )
    {
        for( IGame* gameBase : mGames )
        {
            fanAssert( gameBase != nullptr );
            gameBase->Init();
        }

        // Creates keyboard events
        Input::Get().Manager().CreateKeyboardEvent( "delete", Keyboard::DELETE );
        Input::Get().Manager().CreateKeyboardEvent( "open_scene", Keyboard::O, Keyboard::LEFT_CONTROL );
        Input::Get().Manager().CreateKeyboardEvent( "save_scene", Keyboard::S, Keyboard::LEFT_CONTROL );
        Input::Get().Manager().CreateKeyboardEvent( "reload_scene", Keyboard::R, Keyboard::LEFT_CONTROL );
        Input::Get().Manager().CreateKeyboardEvent( "freeze_capture", Keyboard::END );
        Input::Get().Manager().CreateKeyboardEvent( "copy", Keyboard::C, Keyboard::LEFT_CONTROL );
        Input::Get().Manager().CreateKeyboardEvent( "paste", Keyboard::V, Keyboard::LEFT_CONTROL );
        Input::Get().Manager().CreateKeyboardEvent( "toogle_camera", Keyboard::TAB );
        Input::Get().Manager().CreateKeyboardEvent( "toogle_world", Keyboard::F1 );
        Input::Get().Manager().CreateKeyboardEvent( "show_ui", Keyboard::F3 );
        Input::Get().Manager().CreateKeyboardEvent( "play_pause", Keyboard::F5 );
        Input::Get().Manager().CreateKeyboardEvent( "reload_shaders", Keyboard::F11 );
        Input::Get().Manager().CreateKeyboardEvent( "reload_icons", Keyboard::F12 );
        Input::Get().Manager().CreateKeyboardEvent( "toogle_follow_transform_lock", Keyboard::L );
        Input::Get().Manager().CreateKeyboardEvent( "test", Keyboard::T );
        //editor axis
        Input::Get().Manager().CreateKeyboardAxis( "editor_forward", Keyboard::W, Keyboard::S );
        Input::Get().Manager().CreateKeyboardAxis( "editor_left", Keyboard::A, Keyboard::D );
        Input::Get().Manager().CreateKeyboardAxis( "editor_up", Keyboard::E, Keyboard::Q );
        Input::Get().Manager().CreateKeyboardAxis( "editor_boost", Keyboard::LEFT_SHIFT, Keyboard::NONE );
        GameClient::CreateGameAxes();

        // creates window
        glm::ivec2 windowPosition = mLaunchSettings.window_position;
        glm::ivec2 windowSize = mLaunchSettings.window_size;
        if( windowPosition == glm::ivec2(-1,-1) ){ SerializedValues::LoadWindowPosition( windowPosition ); }
        if( windowSize == glm::ivec2(-1,-1) ){ SerializedValues::LoadWindowSize( windowSize ); }
        mWindow.Create( _settings.windowName.c_str(), windowPosition, windowSize );
        mFullScreen.SavePreviousPositionAndSize( mWindow );

        Mouse::SetCallbacks( mWindow.mWindow );

        // creates renderer
        mRenderer = new Renderer( mWindow, Renderer::ViewType::Editor );
        RenderResources::SetupResources( mRenderer->mMeshManager,
                                         mRenderer->mMesh2DManager,
                                         mRenderer->mTextureManager,
                                         mRenderer->mDefaultFont );
        SceneResources::SetupResources( mPrefabManager );


        // Initialize editor components
        mMainMenuBar    = new MainMenuBar();
        mGameViewWindow = new GameViewWindow( _settings.launchMode );
        SceneWindow& sceneWindow = *new SceneWindow();
        mMainMenuBar->SetWindows( {
            mGameViewWindow,
            &sceneWindow,
            new RenderWindow( *mRenderer ),
            new InspectorWindow(),
            new ConsoleWindow(),
            new EcsWindow(),
            new ProfilerWindow(),
            new PreferencesWindow( *mRenderer, mFullScreen ),
            new SingletonsWindow(),
            new UnitTestsWindow()
        } );

        // Instance messages
        mMainMenuBar->onReloadShaders.Connect( &Renderer::ReloadShaders, mRenderer );
        mMainMenuBar->onReloadIcons.Connect( &Renderer::ReloadIcons, mRenderer );
        mMainMenuBar->onExit.Connect( &EditorHolder::Exit, this );

        // Events linking
        InputManager& manager = Input::Get().Manager();
        manager.FindEvent( "reload_shaders" )->Connect( &Renderer::ReloadShaders, mRenderer );
        manager.FindEvent( "play_pause" )->Connect( &EditorHolder::OnCurrentGameSwitchPlayStop, this );
        manager.FindEvent( "copy" )->Connect( &EditorHolder::OnCurrentGameCopy, this );
        manager.FindEvent( "paste" )->Connect( &EditorHolder::OnCurrentGamePaste, this );
        manager.FindEvent( "show_ui" )->Connect( &EditorHolder::OnToogleShowUI, this );
        manager.FindEvent( "toogle_camera" )->Connect( &EditorHolder::OnCurrentGameToogleCamera, this );
        manager.FindEvent( "open_scene" )->Connect( &EditorHolder::OnCurrentGameOpen, this );
        manager.FindEvent( "save_scene" )->Connect( &EditorHolder::OnCurrentGameSave, this );
        manager.FindEvent( "reload_scene" )->Connect( &EditorHolder::OnCurrentGameReload, this );
        manager.FindEvent( "toogle_world" )->Connect( &EditorHolder::OnCycleCurrentGame, this );
        manager.FindEvent( "reload_icons" )->Connect( &Renderer::ReloadIcons, mRenderer );

        mGameViewWindow->onSizeChanged.Connect( &Renderer::ResizeGame, mRenderer );
        mGameViewWindow->onPlay.Connect( &EditorHolder::OnCurrentGameStart, this );
        mGameViewWindow->onPause.Connect( &EditorHolder::OnCurrentGamePause, this );
        mGameViewWindow->onResume.Connect( &EditorHolder::OnCurrentGameResume, this );
        mGameViewWindow->onStop.Connect( &EditorHolder::OnCurrentGameStop, this );
        mGameViewWindow->onStep.Connect( &EditorHolder::OnCurrentGameStep, this );
        mGameViewWindow->onSelectGame.Connect( &EditorHolder::OnCurrentGameSelect, this );

        // Loop over all worlds to initialize them
        for( int gameIndex = 0; gameIndex < (int)mGames.size(); gameIndex++ )
        {
            IGame   & game  = *mGames[gameIndex];
            EcsWorld& world = game.mWorld;

            world.AddSingletonType<EditorPlayState>();
            world.AddSingletonType<EditorCamera>();
            world.AddSingletonType<EditorGrid>();
            world.AddSingletonType<EditorSelection>();
            world.AddSingletonType<EditorCopyPaste>();
            world.AddSingletonType<EditorGizmos>();
            world.AddTagType<tag_editorOnly>();

            RenderWorld& renderWorld = world.GetSingleton<RenderWorld>();
            renderWorld.isHeadless = ( &game != &GetCurrentGame() );

            RenderResources& renderResources = world.GetSingleton<RenderResources>();
            renderResources.SetPointers( &mRenderer->mMeshManager,
                                         &mRenderer->mMesh2DManager,
                                         &mRenderer->mTextureManager,
                                         &mRenderer->mDefaultFont);

            SceneResources& sceneResources = world.GetSingleton<SceneResources>();
            sceneResources.SetPointers( &mPrefabManager );

            Scene          & scene     = world.GetSingleton<Scene>();
            EditorSelection& selection = world.GetSingleton<EditorSelection>();

            selection.ConnectCallbacks( scene );
            sceneWindow.onSelectSceneNode.Connect( &EditorSelection::SetSelectedSceneNode, &selection );

            scene.onLoad.Connect( &SceneWindow::OnExpandHierarchy, &sceneWindow );
            scene.onLoad.Connect( &EditorHolder::OnSceneLoad, this );

            // load scene
            scene.New();
            if( !_settings.loadScene.empty() )
            {
                scene.LoadFrom( _settings.loadScene );

                // auto play the scene
                if( _settings.autoPlay )
                {
                    GameStart( game );
                }
            }
        }
    }

    //========================================================================================================
    //========================================================================================================
    EditorHolder::~EditorHolder()
    {
        // Deletes ui
        delete mMainMenuBar;

        // Save window position/size if it was not modified by a launch command
        if( mLaunchSettings.window_size == glm::ivec2( -1, -1 ) )
        {
            SerializedValues::SaveWindowSize( mWindow.GetSize() );
        }
        if( mLaunchSettings.window_position == glm::ivec2( -1, -1 ) )
        {
            SerializedValues::SaveWindowPosition( mWindow.GetPosition() );
        }
        SerializedValues::Get().SaveValuesToDisk();

        mPrefabManager.Clear();

        delete mRenderer;
        mWindow.Destroy();
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::Exit()
    {
        mApplicationShouldExit = true;
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::Run()
    {
        // initializes timers
        mLastRenderTime = Time::ElapsedSinceStartup();
        for( IGame* game : mGames )
        {
            Time& time = game->mWorld.GetSingleton<Time>();
            time.lastLogicTime = Time::ElapsedSinceStartup();
        }
        Profiler::Get().Begin();

        // main loop
        while( mApplicationShouldExit == false && mWindow.IsOpen() == true )
        {
            Step();
        }

        // Exit sequence
        Debug::Log( "Exit application" );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::Step()
    {
        const double currentTime       = Time::ElapsedSinceStartup();
        const bool   renderIsThisFrame = currentTime > mLastRenderTime + Time::s_renderDelta;
        const Time& currentWorldTime = GetCurrentGame().mWorld.GetSingleton<Time>();
        const bool logicIsThisFrame = currentTime >
                                      currentWorldTime.lastLogicTime + currentWorldTime.logicDelta;

        // Update all worlds
        for( int gameIndex = 0; gameIndex < (int)mGames.size(); gameIndex++ )
        {
            IGame   & game  = *mGames[gameIndex];
            EcsWorld& world = game.mWorld;
            const bool isCurrentWorld = ( &game == &GetCurrentGame() );

            // runs logic, renders ui
            Time& time = world.GetSingleton<Time>();
            while( currentTime > time.lastLogicTime + time.logicDelta )
            {
                world.GetSingleton<RenderDebug>().Clear();

                if( isCurrentWorld )
                {
                    // Update input
                    ImGui::GetIO().DeltaTime = time.logicDelta;
                    const btVector2 gamePos  = mGameViewWindow->GetPosition();
                    const btVector2 gameSize = mGameViewWindow->GetSize();
                    //todo mGameViewWindow->IsHovered()
                    Mouse::NextFrame( mWindow.mWindow, ToGLM( gamePos ), ToGLM( gameSize ) );
                    Input::Get().NewFrame();
                    Input::Get().Manager().PullEvents();
                    world.GetSingleton<Mouse>().UpdateData( mWindow.mWindow );
                }

                // checking the loop timing is not late
                const double loopDelayMilliseconds = 1000. *
                                                     ( currentTime -
                                                       ( time.lastLogicTime + time.logicDelta ) );
                if( loopDelayMilliseconds > 30 )
                {
                    //Debug::Warning() << "logic is late of " << loopDelayMilliseconds << "ms" << Debug::Endl();
                    // if we are really really late, resets the timer
                    if( loopDelayMilliseconds > 100 )
                    {
                        time.lastLogicTime = currentTime - time.logicDelta;
                        //Debug::Warning() << "reset logic timer " << Debug::Endl();
                    }
                }

                // increase the logic time of a timeScaleDelta with n timeScaleIncrements
                if( std::abs( time.timeScaleDelta ) >= time.timeScaleIncrement )
                {
                    const float increment = time.timeScaleDelta > 0.f
                            ? time.timeScaleIncrement
                            : -time.timeScaleIncrement;
                    time.lastLogicTime -= increment;
                    time.timeScaleDelta -= increment;
                }

                time.lastLogicTime += time.logicDelta;

                const EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
                game.Step( playState.mState == EditorPlayState::PLAYING ? time.logicDelta : 0.f );

                // ui & debug
                if( mShowUi )
                {
                    world.Run<SMoveFollowTransforms>();
                }

                if( isCurrentWorld )
                {
                    assert( logicIsThisFrame );
                    EditorCamera& editorCamera = world.GetSingleton<EditorCamera>();
                    Scene       & scene        = world.GetSingleton<Scene>();
                    // only update the editor camera when we are using it
                    if( scene.mainCameraHandle == editorCamera.cameraHandle )
                    {
                        EditorCamera::Update( world, time.logicDelta );
                    }
                    world.GetSingleton<EditorSelection>().Update( mGameViewWindow->IsHovered() );

                    if( renderIsThisFrame )
                    {
                        SCOPED_PROFILE( debug_draw );
                        // Debug Draw
                        if( mMainMenuBar->ShowWireframe() ){ world.Run<S_DrawDebugWireframe>(); }
                        if( mMainMenuBar->ShowNormals() ){ world.Run<S_DrawDebugNormals>(); }
                        if( mMainMenuBar->ShowAABB() ){ world.Run<S_DrawDebugBounds>(); }
                        if( mMainMenuBar->ShowHull() ){ world.Run<S_DrawDebugHull>(); }
                        if( mMainMenuBar->ShowLights() )
                        {
                            world.Run<S_DrawDebugPointLights>();
                            world.Run<S_DrawDebugDirectionalLights>();
                        }
                        EditorGrid::Draw( GetCurrentGame().mWorld );

                        // ImGui render
                        {
                            SCOPED_PROFILE( ImGui_render );
                            ImGui::NewFrame();
                            mMainMenuBar->Draw( game.mWorld );
                            if( ImGui::Begin( "test" ) )
                             {
                                 if( ImGui::Button("load char")){
                                     mRenderer->WaitIdle();
                                     mRenderer->mTextureManager.Remove("font48");
                                     mRenderer->mTextureManager.DestroyRemovedTextures( mRenderer->mDevice );
                                     mRenderer->mTextureManager.Add(
                                             mRenderer->mDefaultFont.GenerateAtlas(), "font48" );
                                 }
                                 ImGui::End();
                             }
                            ImGui::Render();
                        }
                    }
                }
                {
                    SCOPED_PROFILE( apply_transitions );
                    world.ApplyTransitions();
                }
            }
        }

        mOnLPPSynch.Emmit();

        // Render world
        if( renderIsThisFrame && logicIsThisFrame )
        {
            mLastRenderTime = currentTime;

            Time::RegisterFrameDrawn();    // used for stats

            UpdateRenderWorld( *mRenderer, GetCurrentGame(), ToGLM( mGameViewWindow->GetSize() ) );

            mRenderer->DrawFrame();
            Profiler::Get().End();
            Profiler::Get().Begin();
        }

        // sleep for the rest of the frame
        if( mLaunchSettings.mainLoopSleep )
        {
            // @todo repair this to work with multiple worlds running
// 				const double minSleepTime = 1;
// 				const double endFrameTime = Time::ElapsedSinceStartup();
// 				const double timeBeforeNextLogic = lastLogicTime + time.logicDelta - endFrameTime;
// 				const double timeBeforeNextRender = lastRenderTime + Time::s_renderDelta - endFrameTime;
// 				const double sleepTimeMiliseconds = 1000. * std::min( timeBeforeNextLogic, timeBeforeNextRender );
// 				if( sleepTimeMiliseconds > minSleepTime )
// 				{
// 					std::this_thread::sleep_for( std::chrono::milliseconds( int( sleepTimeMiliseconds / 2 ) ) );
// 				}
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::GameStart( IGame& _game )
    {
        // saves the scene before playing
        Scene& scene = _game.mWorld.GetSingleton<Scene>();
        if( scene.path.empty() )
        {
            Debug::Warning() << "please save the scene before playing" << Debug::Endl();
            return;
        }

        EditorPlayState& playState = _game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            scene.Save();
            Debug::Highlight() << _game.mName << ": start" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
            _game.Start();
        }
    }

    //========================================================================================================
    //========================================================================================================
    struct SceneRestoreState
    {
        SceneRestoreState( Scene& _scene ) : mScene( _scene ) {}
        void Save()
        {
            EcsWorld& world = *mScene.world;

            // Saves the camera position for restoring it later
            const EcsEntity oldCameraID = world.GetEntity( mScene.mainCameraHandle );
            mPrevCameraTransform = world.GetComponent<Transform>( oldCameraID ).transform;
            // save old selection
            SceneNode* prevSelectionNode = world.GetSingleton<EditorSelection>().GetSelectedSceneNode();
            mPrevSelectionHandle = prevSelectionNode != nullptr ? prevSelectionNode->handle : 0;
        }

        void Restore() const
        {
            EcsWorld& world = *mScene.world;

            // restore camera transform
            const EcsEntity newCameraID = world.GetEntity( mScene.mainCameraHandle );
            world.GetComponent<Transform>( newCameraID ).transform = mPrevCameraTransform;

            // restore selection
            if( mPrevSelectionHandle != 0 && mScene.nodes.find( mPrevSelectionHandle ) != mScene.nodes.end() )
            {
                fan::SceneNode& node = world.GetComponent<fan::SceneNode>
                        ( world.GetEntity( mPrevSelectionHandle ) );

                world.GetSingleton<EditorSelection>().SetSelectedSceneNode( &node );
            }
        }

        Scene& mScene;
        EcsHandle   mPrevSelectionHandle = 0;
        btTransform mPrevCameraTransform;
    };

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::GameStop( IGame& _game )
    {
        EcsWorld& world = _game.mWorld;

        EditorPlayState& playState = _game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING || playState.mState == EditorPlayState::PAUSED )
        {
            UseEditorCamera( world );

            Scene& scene = world.GetSingleton<Scene>();
            SceneRestoreState restoreState( scene );
            restoreState.Save();

            Debug::Highlight() << _game.mName << ": stopped" << Debug::Endl();
            playState.mState = EditorPlayState::STOPPED;
            _game.Stop();
            scene.LoadFrom( scene.path ); // reload the scene

            restoreState.Restore();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::GamePause( IGame& _game )
    {
        EditorPlayState& playState = _game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PLAYING )
        {
            Debug::Highlight() << _game.mName << ": paused" << Debug::Endl();
            playState.mState = EditorPlayState::PAUSED;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::GameResume( IGame& _game )
    {
        EditorPlayState& playState = _game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::PAUSED )
        {
            Debug::Highlight() << _game.mName << ": resumed" << Debug::Endl();
            playState.mState = EditorPlayState::PLAYING;
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnSceneLoad( Scene& _scene )
    {
        EcsWorld& world = *_scene.world;
        EditorCamera::CreateEditorCamera( world );

        if( &world == &GetCurrentGame().mWorld )
        {
            world.GetSingleton<EditorSelection>().Deselect();
        }
    }

    //========================================================================================================
    // Updates the render world singleton component
    //========================================================================================================
    void EditorHolder::UpdateRenderWorld( Renderer& _renderer, IGame& _game, const glm::vec2 _size )
    {
        EcsWorld& world = _game.mWorld;

        SCOPED_PROFILE( update_RW );
        RenderWorld      & renderWorld = world.GetSingleton<RenderWorld>();
        const RenderDebug& renderDebug = world.GetSingleton<RenderDebug>();
        renderWorld.targetSize = _size;

        // update render data
        {
            SCOPED_PROFILE( update_render_data );
            world.Run<SUpdateRenderWorldModels>();
            world.Run<SUpdateRenderWorldUI>();
            world.Run<S_UpdateRenderWorldPointLights>();
            world.Run<S_UpdateRenderWorldDirectionalLights>();
        }

        // particles mesh
        RenderDataModel particlesDrawData;
        particlesDrawData.mesh         = renderWorld.particlesMesh;
        particlesDrawData.modelMatrix  = glm::mat4( 1.f );
        particlesDrawData.normalMatrix = glm::mat4( 1.f );
        particlesDrawData.color        = glm::vec4( 1.f, 1.f, 1.f, 1.f );
        particlesDrawData.shininess    = 1;
        particlesDrawData.textureIndex = 1;
        renderWorld.drawData.push_back( particlesDrawData );

        {
            SCOPED_PROFILE( set_render_data );
            _renderer.SetDrawData( renderWorld.drawData );
            _renderer.SetUIDrawData( renderWorld.uiDrawData );
            _renderer.SetPointLights( renderWorld.pointLights );
            _renderer.SetDirectionalLights( renderWorld.directionalLights );
            _renderer.SetDebugDrawData( renderDebug.debugLines,
                                        renderDebug.debugLinesNoDepthTest,
                                        renderDebug.debugTriangles );
        }

        // Camera
        {
            SCOPED_PROFILE( update_camera );
            Scene& scene = world.GetSingleton<Scene>();
            EcsEntity cameraID = world.GetEntity( scene.mainCameraHandle );
            Camera& camera = world.GetComponent<Camera>( cameraID );
            camera.aspectRatio = _size[0] / _size[1];
            Transform& cameraTransform = world.GetComponent<Transform>( cameraID );
            _renderer.SetMainCamera(
                    camera.GetProjection(),
                    camera.GetView( cameraTransform ),
                    ToGLM( cameraTransform.GetPosition() )
            );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnCycleCurrentGame()
    {
        OnCurrentGameSelect( ( mCurrentGame + 1 ) % ( mGames.size() ) );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnCurrentGameStart()
    {
        IGame& game = GetCurrentGame();
        GameStart( game );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnCurrentGameSwitchPlayStop()
    {
        IGame                & game      = GetCurrentGame();
        const EditorPlayState& playState = game.mWorld.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            GameStart( game );
        }
        else
        {
            GameStop( game );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::UseEditorCamera( EcsWorld& _world )
    {
        Scene       & scene        = _world.GetSingleton<Scene>();
        EditorCamera& editorCamera = _world.GetSingleton<EditorCamera>();
        scene.SetMainCamera( editorCamera.cameraHandle );
    }

    //========================================================================================================
    //========================================================================================================
    void EditorHolder::OnCurrentGameOpen() { mMainMenuBar->Open( GetCurrentGame().mWorld ); }
    void EditorHolder::OnCurrentGameReload() { mMainMenuBar->Reload( GetCurrentGame().mWorld ); }
    void EditorHolder::OnCurrentGameSave() { mMainMenuBar->Save( GetCurrentGame().mWorld ); }
    void EditorHolder::OnCurrentGameCopy()
    {
        GetCurrentGame().mWorld.GetSingleton<EditorCopyPaste>().OnCopy();
    }
    void EditorHolder::OnCurrentGamePaste()
    {
        GetCurrentGame().mWorld.GetSingleton<EditorCopyPaste>().OnPaste();
    }

    //========================================================================================================
    // sets which ecs world will be displayed in the editor
    //========================================================================================================
    void EditorHolder::OnCurrentGameSelect( const int _index )
    {
        assert( _index < (int)mGames.size() );

        mCurrentGame = _index;

        // Set all to headless except the current
        for( int gameIndex = 0; gameIndex < (int)mGames.size(); gameIndex++ )
        {
            IGame      & game        = *mGames[gameIndex];
            RenderWorld& renderWorld = game.mWorld.GetSingleton<RenderWorld>();
            renderWorld.isHeadless = ( gameIndex != mCurrentGame );
        }

        mGameViewWindow->SetCurrentGameSelected( mCurrentGame );
    }

    //========================================================================================================
    // toogle the camera between editor and game
    //========================================================================================================
    void EditorHolder::OnCurrentGameToogleCamera()
    {
        IGame   & currentGame = GetCurrentGame();
        EcsWorld& world       = currentGame.mWorld;

        const EditorPlayState& playState = world.GetSingleton<EditorPlayState>();
        if( playState.mState == EditorPlayState::STOPPED )
        {
            Debug::Warning() << "You cannot toogle camera outside of play mode" << Debug::Endl();
            return;
        }

        Scene       & scene        = world.GetSingleton<Scene>();
        EditorCamera& editorCamera = world.GetSingleton<EditorCamera>();

        if( scene.mainCameraHandle == editorCamera.cameraHandle )
        {
            currentGame.mOnSwitchToGameCamera.Emmit();
        }
        else
        {
            UseEditorCamera( world );
        }
    }

    //========================================================================================================
    // callback of the game view step button
    //========================================================================================================
    void EditorHolder::OnCurrentGameStep()
    {
        IGame& game = GetCurrentGame();
        Time & time = game.mWorld.GetSingleton<Time>();
        game.Step( time.logicDelta );
    }
}