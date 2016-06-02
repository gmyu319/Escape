#pragma once

#include "GameState.h"
#include "BulletList.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

#define WALKING_SPEED 800.0f
#define RUNNING_SPEED 1600.0f
#define ZOMBIE_SPEED 400.0f
#define MAX_BULLET 30
#define NUM_OF_NPC 100

class PlayState : public GameState
{
public:
    void enter(void);
    void exit(void);

    void pause(void);
    void resume(void);

    bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
    bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

    bool mouseMoved(GameManager* game, const OIS::MouseEvent &e);
    bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);

    bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
    bool keyReleased(GameManager* game, const OIS::KeyEvent &e);

    static PlayState* getInstance() { return &mPlayState; }

private:
    void _setLights(void);
    void _drawGroundPlane(void);
    void _drawGridPlane(void);

    static PlayState mPlayState;

    Ogre::Root *mRoot;
    Ogre::RenderWindow* mWindow;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Camera* mCamera;

    Ogre::Light *mLightP, *mLightD, *mLightS;

    Ogre::SceneNode* mCharacterRoot;
    Ogre::SceneNode* mCharacterYaw;
    Ogre::SceneNode* mCameraHolder;
    Ogre::SceneNode* mCameraYaw;
    Ogre::SceneNode* mCameraPitch;

    Ogre::Entity* mCharacterEntity;

    Ogre::SceneNode* mBulletNode[MAX_BULLET];
    Ogre::Entity* mBulletEntity[MAX_BULLET];

    Ogre::SceneNode* mZombieNode[NUM_OF_NPC];
    Ogre::Entity* mZombieEntity[NUM_OF_NPC];
    Ogre::Vector3 mZombieTargetPoint[NUM_OF_NPC];

    Ogre::AnimationState *mZombieAnimationState[NUM_OF_NPC];

    std::vector<pair<string, Ogre::AnimationState*>> mAnimationStates;

    Ogre::Overlay* mInformationOverlay;

    Ogre::Vector3 mPlayerDir;
    float mPlayerSpeed = WALKING_SPEED;
    string mPlayerAnimationState;
    Ogre::Vector3 mPlayerJump;
    CBulletList mPlayerBullet;
    float mFireSpeed = 0.2f;
};