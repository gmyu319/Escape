#include "PlayState.h"
#include "TitleState.h"

using namespace Ogre;

PlayState PlayState::mPlayState;
bool g_bLeftButtonDown = false;
bool g_bBulletMark[MAX_BULLET];

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

    mCameraYaw = mCharacterRoot->createChildSceneNode("CameraYaw", Vector3(0.0f, 240.0f, 0.0f));
    mCameraPitch = mCameraYaw->createChildSceneNode("CameraPitch");
    mCameraHolder = mCameraPitch->createChildSceneNode("CameraHolder", Vector3(0.0f, 0.0f, -480.0f));

    mCameraYaw->setInheritOrientation(false);

    mCharacterEntity = mSceneMgr->createEntity("Professor", "DustinBody.mesh");

    string fish = "Fish";
    for (int i = 0; i < MAX_BULLET; ++i) {
        mBulletEntity[i] = mSceneMgr->createEntity(fish + to_string(i), "fish.mesh");
        mBulletNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode(fish + "Node" + to_string(i));
        mBulletNode[i]->attachObject(mBulletEntity[i]);
        mBulletNode[i]->setScale(Vector3(20.0f, 20.0f, 20.0f));
        mBulletNode[i]->yaw(Degree(-90.0f));
    }

    mCharacterYaw->attachObject(mCharacterEntity);
    mCharacterEntity->setCastShadows(true);

    mCameraHolder->attachObject(mCamera);
    mCamera->lookAt(mCameraYaw->getPosition());

    mAnimationStates.push_back(make_pair<string, Ogre::AnimationState*>("Idle", mCharacterEntity->getAnimationState("Idle")));
    mAnimationStates.push_back(make_pair<string, Ogre::AnimationState*>("Walk", mCharacterEntity->getAnimationState("Walk")));
    mAnimationStates.push_back(make_pair<string, Ogre::AnimationState*>("Run", mCharacterEntity->getAnimationState("Run")));
    mAnimationStates.push_back(make_pair<string, Ogre::AnimationState*>("Jumping", mCharacterEntity->getAnimationState("Jumping")));

    mPlayerDir = Vector3(0.0f, 0.0f, 0.0f);
    mPlayerJump = Vector3(0.0f, 0.0f, 0.0f);
    mPlayerAnimationState = "Idle";

    mPlayerBullet.Init();
}

void PlayState::exit(void)
{
    mSceneMgr->clearScene();
    mInformationOverlay->hide();

    mPlayerAnimationState = "";
    mAnimationStates.clear();
}

void PlayState::pause(void)
{
}

void PlayState::resume(void)
{
}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{
    static Vector3 gravity = Vector3(0.0f, -9.8f, 0.0f);
    static float sumDeltaTime = 0.0;
    mCharacterRoot->setOrientation(mCameraYaw->getOrientation());

    if (Vector3(0.0f, 0.0f, 0.0f) != mPlayerDir) { // move
        if (RUNNING_SPEED == mPlayerSpeed)
            mPlayerAnimationState = "Run";
        else
            mPlayerAnimationState = "Walk";

        mCharacterRoot->translate(mPlayerDir.normalisedCopy() * mPlayerSpeed * evt.timeSinceLastFrame, Node::TransformSpace::TS_LOCAL);

        Vector3 dir = mPlayerDir.normalisedCopy();
        dir.y = 0.0f;
        Quaternion quat = Vector3(Vector3::UNIT_Z).getRotationTo(dir);
        mCharacterYaw->setOrientation(quat);
    }
    else { // idle
        mPlayerAnimationState = "Idle";
    }

    // jump
    mPlayerJump += gravity * evt.timeSinceLastFrame * 4.0f;
    mCharacterRoot->translate(mPlayerJump, Node::TransformSpace::TS_WORLD);
    if (mCharacterRoot->getPosition().y < 0.0f)
        mCharacterRoot->setPosition(mCharacterRoot->getPosition().x, 0.0f, mCharacterRoot->getPosition().z);

    Ogre::Matrix3 mtx;
    mCameraYaw->_getFullTransform().extract3x3Matrix(mtx);
    Vector3 vec = mtx.GetColumn(2);

    // fire
    if(g_bLeftButtonDown) {
        sumDeltaTime += evt.timeSinceLastFrame;
        if (sumDeltaTime >= mFireSpeed) {
            mPlayerBullet.Add(mCharacterRoot->getPosition().x, mCharacterRoot->getPosition().y + 140.0f, 
                mCharacterRoot->getPosition().z, vec.x, vec.y, vec.z);
            sumDeltaTime = 0.0f;
        }

        Vector3 vec2;
        mCharacterRoot->_getFullTransform().extract3x3Matrix(mtx);
        vec2 = mtx.GetColumn(2);
        Quaternion quat = Vector3(vec2).getRotationTo(vec);
        mCharacterYaw->setOrientation(quat);
    }

    // bullet update
    mPlayerBullet.Update(evt.timeSinceLastFrame);
    CNode *start = mPlayerBullet.getHead()->m_next;
    CNode *end = mPlayerBullet.getTail();

    for (int i = 0; i < MAX_BULLET; ++i) g_bBulletMark[i] = false;
    while (start != end) {
        mBulletNode[start->getKey()]->setPosition(Vector3(start->mPos.x, start->mPos.y, start->mPos.z));
        g_bBulletMark[start->getKey()] = true;
        start = start->m_next;
    }
    for (int i = 0; i < MAX_BULLET; ++i) {
        if (g_bBulletMark[i]) continue;
        mBulletNode[i]->setPosition(Vector3(-9999.0f, -9999.0f, -9999.0f));
    }

    // animation update
    for (auto as : mAnimationStates) {
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
    static Ogre::DisplayString currFps = L"현재 FPS: ";
    static Ogre::DisplayString avgFps = L"평균 FPS: ";
    static Ogre::DisplayString bestFps = L"최고 FPS: ";
    static Ogre::DisplayString worstFps = L"최저 FPS: ";

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
    if (OIS::KC_W == e.key) mPlayerDir.z += -1.0f;
    if (OIS::KC_S == e.key) mPlayerDir.z += 1.0f;
    if (OIS::KC_A == e.key) mPlayerDir.x += -1.0f;
    if (OIS::KC_D == e.key) mPlayerDir.x += 1.0f;
    if (OIS::KC_LSHIFT == e.key) mPlayerSpeed = WALKING_SPEED;

    return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
    if (OIS::KC_W == e.key) mPlayerDir.z += 1.0f;
    if (OIS::KC_S == e.key) mPlayerDir.z += -1.0f;
    if (OIS::KC_A == e.key) mPlayerDir.x += 1.0f;
    if (OIS::KC_D == e.key) mPlayerDir.x += -1.0f;
    if (OIS::KC_LSHIFT == e.key) mPlayerSpeed = RUNNING_SPEED;
    if (OIS::KC_SPACE == e.key) if(mCharacterRoot->getPosition().y == 0.0f) mPlayerJump = Vector3(0.0f, 19.6f, 0.0f);
    if (OIS::KC_R == e.key) mPlayerBullet.Init();

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
    if (e.state.buttonDown(OIS::MB_Right))
        mCamera->setFOVy(Degree(20.0f));
    if (e.state.buttonDown(OIS::MB_Left))
        g_bLeftButtonDown = true;

    return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    if (!e.state.buttonDown(OIS::MB_Right))
        mCamera->setFOVy(Degree(45.0f));
    if (!e.state.buttonDown(OIS::MB_Left))
        g_bLeftButtonDown = false;

    return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
    SetCursorPos(960.0f, 540.0f);

    mCameraYaw->yaw(Degree(-e.state.X.rel * 0.1f));
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
    // 좌표축 표시
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
    for (int i = 0; i < 21; ++i)
    {
        gridPlane->position(-5000.0f, 0.0f, 5000.0f - i * 500.0f);
        gridPlane->position(5000.0f, 0.0f, 5000.0f - i * 500.0f);

        gridPlane->position(-5000.0f + i * 500.0f, 0.0f, 5000.0f);
        gridPlane->position(-5000.0f + i * 500.0f, 0.0f, -5000.0f);
    }

    gridPlane->end();

    gridPlaneNode->attachObject(gridPlane);
}