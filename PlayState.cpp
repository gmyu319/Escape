#include "PlayState.h"
#include "TitleState.h"

using namespace Ogre;

PlayState PlayState::mPlayState;

void PlayState::enter(void)
{
    mRoot = Root::getSingletonPtr();
    mRoot->getAutoCreatedWindow()->resetStatistics();

    mSceneMgr = mRoot->getSceneManager("main");
    mCamera = mSceneMgr->getCamera("main");
    mCamera->setPosition(Ogre::Vector3::ZERO);

    _drawGridPlane();
    _setLights();
    _drawGroundPlane();

    mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
    mInformationOverlay->show();

    mCharacterRoot = mSceneMgr->getRootSceneNode()->createChildSceneNode("ProfessorRoot");
    mCharacterYaw = mCharacterRoot->createChildSceneNode("ProfessorYaw");

    mCameraYaw = mCharacterRoot->createChildSceneNode("CameraYaw", Vector3(0.0f, 120.0f, 0.0f));
    mCameraPitch = mCameraYaw->createChildSceneNode("CameraPitch");
    mCameraHolder = mCameraPitch->createChildSceneNode("CameraHolder", Vector3(0.0f, 80.0f, 500.0f));

    mCharacterEntity = mSceneMgr->createEntity("Professor", "DustinBody.mesh");

    Quaternion quat = Ogre::Vector3(Vector3::UNIT_Z).getRotationTo(Vector3::NEGATIVE_UNIT_Z);
    mCharacterYaw->setOrientation(quat);

    mCharacterYaw->attachObject(mCharacterEntity);
    mCharacterEntity->setCastShadows(true);

    mCameraHolder->attachObject(mCamera);
    mCamera->lookAt(mCameraYaw->getPosition());

    mAnimationStates.push_back(make_pair<string, Ogre::AnimationState*>("Idle", mCharacterEntity->getAnimationState("Idle")));
    mAnimationStates.push_back(make_pair<string, Ogre::AnimationState*>("Walk", mCharacterEntity->getAnimationState("Walk")));
    mAnimationStates.push_back(make_pair<string, Ogre::AnimationState*>("Run", mCharacterEntity->getAnimationState("Run")));

    mPlayerDir = Vector3(0.0f, 0.0f, 0.0f);
    mPlayerAnimationState = "Idle";
}

void PlayState::exit(void)
{
    mSceneMgr->clearScene();
    mInformationOverlay->hide();
}

void PlayState::pause(void)
{
}

void PlayState::resume(void)
{
}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{
    if (Vector3(0.0f, 0.0f, 0.0f) != mPlayerDir) {
        if (RUNNING_SPEED == mPlayerSpeed)
            mPlayerAnimationState = "Run";
        else
            mPlayerAnimationState = "Walk";

        mCharacterRoot->translate(mPlayerDir.normalisedCopy() * mPlayerSpeed * evt.timeSinceLastFrame);
    }
    else {
        mPlayerAnimationState = "Idle";
    }

    for (auto as : mAnimationStates)
    {
        if (mPlayerAnimationState == as.first) {
            as.second->addTime(evt.timeSinceLastFrame);
            as.second->setLoop(true);
            as.second->setEnabled(true);
        }
        else {
            as.second->setLoop(false);
            as.second->setEnabled(false);
        }
    }

    return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
    static Ogre::DisplayString currFps = L"���� FPS: ";
    static Ogre::DisplayString avgFps = L"��� FPS: ";
    static Ogre::DisplayString bestFps = L"�ְ� FPS: ";
    static Ogre::DisplayString worstFps = L"���� FPS: ";

    OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
    OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
    OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
    OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");

    const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();

    guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
    guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
    guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
    guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS));

    return true;
}


bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
    if (OIS::KC_W == e.key) mPlayerDir.z += 1.0f;
    if (OIS::KC_S == e.key) mPlayerDir.z += -1.0f;
    if (OIS::KC_A == e.key) mPlayerDir.x += 1.0f;
    if (OIS::KC_D == e.key) mPlayerDir.x += -1.0f;
    if (OIS::KC_LSHIFT == e.key) mPlayerSpeed = WALKING_SPEED;

    return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
    if (OIS::KC_W == e.key) mPlayerDir.z += -1.0f;
    if (OIS::KC_S == e.key) mPlayerDir.z += 1.0f;
    if (OIS::KC_A == e.key) mPlayerDir.x += -1.0f;
    if (OIS::KC_D == e.key) mPlayerDir.x += 1.0f;
    if (OIS::KC_LSHIFT == e.key) mPlayerSpeed = RUNNING_SPEED;

    switch (e.key)
    {
    case OIS::KC_ESCAPE:
        game->changeState(TitleState::getInstance());
        break;
    }

    return true;
}

bool PlayState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
    //mCameraYaw->yaw(Degree(-e.state.X.rel));
    //mCameraPitch->pitch(Degree(-e.state.Y.rel));

    //mCameraHolder->translate(Ogre::Vector3(0, 0, -e.state.Z.rel * 0.1f));
    return true;
}



void PlayState::_setLights(void)
{
    mSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));
    mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

    mLightD = mSceneMgr->createLight("LightD");
    mLightD->setType(Light::LT_DIRECTIONAL);
    mLightD->setDirection(Vector3(1.0f, -2.0f, -1.0f));
    mLightD->setVisible(true);
}

void PlayState::_drawGroundPlane(void)
{
    Plane plane(Vector3::UNIT_Y, 0);
    MeshManager::getSingleton().createPlane(
        "Ground",
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane,
        500, 500,
        1, 1,
        true, 1, 5, 5,
        Vector3::NEGATIVE_UNIT_Z
    );

    Entity* groundEntity = mSceneMgr->createEntity("GroundPlane", "Ground");
    mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);
    groundEntity->setMaterialName("KPU_LOGO");
    groundEntity->setCastShadows(false);
}

void PlayState::_drawGridPlane(void)
{
    // ��ǥ�� ǥ��
    Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
    mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
    mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

    Ogre::ManualObject* gridPlane = mSceneMgr->createManualObject("GridPlane");
    Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode");

    Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial", "General");
    gridPlaneMaterial->setReceiveShadows(false);
    gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true);
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1, 1, 1, 0);
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1, 1, 1);
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1, 1, 1);

    gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST);
    for (int i = 0; i < 21; i++)
    {
        gridPlane->position(-500.0f, 0.0f, 500.0f - i * 50);
        gridPlane->position(500.0f, 0.0f, 500.0f - i * 50);

        gridPlane->position(-500.f + i * 50, 0.f, 500.0f);
        gridPlane->position(-500.f + i * 50, 0.f, -500.f);
    }

    gridPlane->end();

    gridPlaneNode->attachObject(gridPlane);
}