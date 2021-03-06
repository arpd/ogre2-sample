
#ifndef _Demo_GraphicsSystem_H_
#define _Demo_GraphicsSystem_H_

#include "BaseSystem.hpp"
#include "GameEntityManager.hpp"
#include "OgrePrerequisites.h"
#include "OgreColourValue.h"
#include "Overlay/OgreOverlayPrerequisites.h"

#include "Threading/OgreUniformScalableTask.h"

#include <SDL.h>

namespace Demo
{
    class SdlInputHandler;

    class GraphicsSystem : public BaseSystem, public Ogre::UniformScalableTask
    {
    protected:
        BaseSystem          *mLogicSystem;

        SDL_Window          *mSdlWindow;
        SdlInputHandler     *mInputHandler;

        Ogre::Root                  *mRoot;
        Ogre::RenderWindow          *mRenderWindow;
        Ogre::SceneManager          *mSceneManager;
        Ogre::Camera                *mCamera;
        Ogre::CompositorWorkspace   *mWorkspace;
        Ogre::String                mResourcePath;

        Ogre::v1::OverlaySystem     *mOverlaySystem;

        /// Tracks the amount of elapsed time since we last
        /// heard from the LogicSystem finishing a frame
        float               mAccumTimeSinceLastLogicFrame;
        Ogre::uint32        mCurrentTransformIdx;
        GameEntityVec       mGameEntities[Ogre::NUM_SCENE_MEMORY_MANAGER_TYPES];
        GameEntityVec const *mThreadGameEntityToUpdate;
        float               mThreadWeight;

        bool                mQuit;

        Ogre::ColourValue   mBackgroundColour;

        void handleWindowEvent( const SDL_Event& evt );

        /// @see MessageQueueSystem::processIncomingMessage
        virtual void processIncomingMessage( Mq::MessageId messageId, const void *data );

        static void addResourceLocation( const Ogre::String &archName, const Ogre::String &typeName,
                                         const Ogre::String &secName );
        virtual void setupResources(void);
        virtual void registerHlms(void);
        /// Optional override method where you can perform resource group loading
        /// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
        virtual void loadResources(void);
        virtual void chooseSceneManager(void);
        virtual void createCamera(void);
        /// Virtual so that advanced samples such as Sample_Compositor can override this
        /// method to change the default behavior if setupCompositor() is overridden, be
        /// aware @mBackgroundColour will be ignored
        virtual Ogre::CompositorWorkspace* setupCompositor(void);

        /// Optional override method where you can create resource listeners (e.g. for loading screens)
        virtual void createResourceListener(void) {}

        void gameEntityAdded( const GameEntityManager::CreatedGameEntity *createdGameEntity );
        void gameEntityRemoved( GameEntity *toRemove );
    public:
        GraphicsSystem( GameState *gameState,
                        Ogre::ColourValue backgroundColour = Ogre::ColourValue( 0.2f, 0.4f, 0.6f ) );
        virtual ~GraphicsSystem();

        void _notifyLogicSystem( BaseSystem *logicSystem )      { mLogicSystem = logicSystem; }

        void initialize( const Ogre::String &windowTitle );
        void deinitialize(void);

        void update( float timeSinceLast );

        /** Updates the SceneNodes of all the game entities in the container,
            interpolating them according to weight, reading the transforms from
            mCurrentTransformIdx and mCurrentTransformIdx-1.
        @param gameEntities
            The container with entities to update.
        @param weight
            The interpolation weight, ideally in range [0; 1]
        */
        void updateGameEntities( const GameEntityVec &gameEntities, float weight );

        /// Overload Ogre::UniformScalableTask. @see updateGameEntities
        virtual void execute( size_t threadId, size_t numThreads );

        /// Returns the GameEntities that are ready to be rendered. May include entities
        /// that are scheduled to be removed (i.e. they are no longer updated by logic)
        const GameEntityVec& getGameEntities( Ogre::SceneMemoryMgrTypes type ) const
                                                                { return mGameEntities[type]; }

        SdlInputHandler* getInputHandler(void)                  { return mInputHandler; }

        bool getQuit(void) const                                { return mQuit; }
        void quit(void)                                         { mQuit = true; }

        float getAccumTimeSinceLastLogicFrame(void) const       { return mAccumTimeSinceLastLogicFrame; }

        Ogre::Root* getRoot(void) const                         { return mRoot; }
        Ogre::RenderWindow* getRenderWindow(void) const         { return mRenderWindow; }
        Ogre::SceneManager* getSceneManager(void) const         { return mSceneManager; }
        Ogre::Camera* getCamera(void) const                     { return mCamera; }
        Ogre::CompositorWorkspace* getCompositorWorkspace(void) const { return mWorkspace; }
        Ogre::v1::OverlaySystem* getOverlaySystem(void) const   { return mOverlaySystem; }
    };
}

#endif
