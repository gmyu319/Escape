//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#define CLIENT_DESCRIPTION "Quaternion"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <iostream>
#include "BulletList.h"
#include <string>

#include <Ogre.h>
#include <OIS/OIS.h>

using namespace std;
using namespace Ogre;

bool g_bRun = false;
CBulletList bulletList;
const int Max_Bullet = 30;
bool g_bMarked[Max_Bullet];

class ESCListener : public FrameListener {
    OIS::Keyboard *mKeyboard;

public:
    ESCListener(OIS::Keyboard *keyboard) : mKeyboard(keyboard) {}
    bool frameStarted(const FrameEvent &evt)
    {
        mKeyboard->capture();
        return !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
    }
};

class MainListener : public FrameListener {
    OIS::Keyboard *mKeyboard;
    Root* mRoot;
    Camera* mCamera;
    SceneNode *mProfessorNode;
    SceneNode *mBulletNode[Max_Bullet];
    Vector3 mCameraPos;
    float m_fSpeed = 0.2f;

public:
    MainListener(Root* root, OIS::Keyboard *keyboard) : mKeyboard(keyboard), mRoot(root)
    {
        mCamera = mRoot->getSceneManager("main")->getCamera("main");
        mProfessorNode = mRoot->getSceneManager("main")->getSceneNode("Professor");

        string strBullet = "Bullet";
        for (int i = 0; i < Max_Bullet; ++i)
        {
            mBulletNode[i] = mRoot->getSceneManager("main")->getSceneNode(strBullet + to_string(i));
        }

        mCameraPos = mCamera->getPosition();
    }

    bool frameStarted(const FrameEvent &evt)
    {
        if (g_bRun)
            m_fSpeed = 0.4f;
        else
            m_fSpeed = 0.2f;

        if (mKeyboard->isKeyDown(OIS::KC_W))
        {
            mCameraPos.z += -m_fSpeed;
            mProfessorNode->translate(Vector3(0.0f, 0.0f, -m_fSpeed));
        }
        if (mKeyboard->isKeyDown(OIS::KC_S))
        {
            mCameraPos.z += m_fSpeed;
            mProfessorNode->translate(Vector3(0.0f, 0.0f, m_fSpeed));
        }
        if (mKeyboard->isKeyDown(OIS::KC_A))
        {
            mCameraPos.x += -m_fSpeed;
            mProfessorNode->translate(Vector3(-m_fSpeed, 0.0f, 0.0f));
        }
        if (mKeyboard->isKeyDown(OIS::KC_D))
        {
            mCameraPos.x += m_fSpeed;
            mProfessorNode->translate(Vector3(m_fSpeed, 0.0f, 0.0f));
        }

        mCamera->setPosition(mCameraPos);

        bulletList.Update(evt.timeSinceLastFrame);
        CNode *start = bulletList.getHead()->m_next;
        CNode *end = bulletList.getTail();
        int tempKey;
        for (int i = 0; i < Max_Bullet; ++i)
            g_bMarked[i] = false;

        while (start != end)
        {
            tempKey = start->getKey();
            mBulletNode[tempKey]->setPosition(Vector3(start->mPos.x, start->mPos.y, start->mPos.z));
            g_bMarked[tempKey] = true;
            start = start->m_next;
        }

        for (int i = 0; i < Max_Bullet; ++i)
        {
            if(false == g_bMarked[i]) mBulletNode[i]->setPosition(Vector3(9999.0f, 9999.0f, 9999.0f));
        }

        return true;
    }

};

class InputController : public FrameListener,
    public OIS::KeyListener,
    public OIS::MouseListener
{

public:
    InputController(Root* root, OIS::Keyboard *keyboard, OIS::Mouse *mouse) : mRoot(root), mKeyboard(keyboard), mMouse(mouse)
    {
        mProfessorNode = mRoot->getSceneManager("main")->getSceneNode("Professor");
        mCamera = mRoot->getSceneManager("main")->getCamera("main");

        mContinue = true;

        keyboard->setEventCallback(this);
        mouse->setEventCallback(this);
    }


    bool frameStarted(const FrameEvent &evt)
    {
        mKeyboard->capture();
        mMouse->capture();
        return mContinue;
    }

    bool keyPressed(const OIS::KeyEvent &evt)
    {
        switch (evt.key)
        {
        //case OIS::KC_W:
        //{
        //    mProfessorNode->setOrientation(Ogre::Quaternion::IDENTITY);
        //    Quaternion z(Degree(90), Vector3::UNIT_Z);
        //    mProfessorNode->rotate(z);
        //}
        //break;
        //case OIS::KC_2:
        //{
        //    mProfessorNode->setOrientation(Ogre::Quaternion::IDENTITY);
        //    Quaternion z(Degree(90), Vector3::UNIT_Z);
        //    Quaternion x(Degree(90), Vector3::UNIT_X);
        //    Quaternion p = x * z;
        //    mProfessorNode->rotate(p);
        //}
        //break;
        //case OIS::KC_3:
        //{
        //    mProfessorNode->setOrientation(Ogre::Quaternion::IDENTITY);
        //    Quaternion z(Degree(90), Vector3::UNIT_Z);
        //    Quaternion x(Degree(90), Vector3::UNIT_X);
        //    Quaternion p = z * x;
        //    mProfessorNode->rotate(p);
        //}
        //break;
        //case OIS::KC_4:
        //{
        //    mProfessorNode->setOrientation(Ogre::Quaternion::IDENTITY);
        //    Quaternion q(Degree(180), Vector3(1, 1, 0));
        //    mProfessorNode->rotate(q);
        //}
        //break;
        //case OIS::KC_R:
        //    mProfessorNode->setOrientation(Ogre::Quaternion::IDENTITY);
        //    break;
        //case OIS::KC_ESCAPE: mContinue = false; break;
        case OIS::KC_R:
            bulletList.Init();
            break;
        case OIS::KC_LSHIFT :
            g_bRun ^= true;
            break;
        }
        return true;
    }

    bool keyReleased(const OIS::KeyEvent &evt)
    {
        return true;
    }

    bool mouseMoved(const OIS::MouseEvent &evt)
    {

        if (evt.state.buttonDown(OIS::MB_Right))
        {
            mProfessorNode->yaw(Degree(-evt.state.X.rel * 0.05f));
            mProfessorNode->pitch(Degree(-evt.state.Y.rel * 0.05f));
            mCamera->yaw(Degree(-evt.state.X.rel * 0.05f));
            mCamera->pitch(Degree(-evt.state.Y.rel * 0.05f));
        }

        mCamera->moveRelative(Ogre::Vector3(0, 0, -evt.state.Z.rel * 0.1f));

        return true;
    }

    bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
    {
        if (evt.state.buttonDown(OIS::MB_Left))
        {
            bulletList.Add(mProfessorNode->getPosition().x, mProfessorNode->getPosition().y + 120.0f, mProfessorNode->getPosition().z,
                0.0f, 0.0f, -1.0f);
        }

        return true;
    }

    bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
    {
        return true;
    }


private:
    bool mContinue;
    Ogre::Root* mRoot;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse* mMouse;
    Camera* mCamera;
    SceneNode* mProfessorNode;
};


class LectureApp {

    Root* mRoot;
    RenderWindow* mWindow;
    SceneManager* mSceneMgr;
    Camera* mCamera;
    Viewport* mViewport;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse* mMouse;

    OIS::InputManager *mInputManager;

    MainListener* mMainListener;
    ESCListener* mESCListener;

public:

    LectureApp() {}

    ~LectureApp() {}

    void go(void)
    {
        // OGRE의 메인 루트 오브젝트 생성
#if !defined(_DEBUG)
        mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
        mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif


        // 초기 시작의 컨피규레이션 설정 - ogre.cfg 이용
        if (!mRoot->restoreConfig()) {
            if (!mRoot->showConfigDialog()) return;
        }

        mWindow = mRoot->initialise(true, CLIENT_DESCRIPTION " : Copyleft by Dae-Hyun Lee 2010");

        mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "main");
        mCamera = mSceneMgr->createCamera("main");


        mCamera->setPosition(0.0f, 200.0f, 500.0f);
        mCamera->lookAt(0.0f, 50.0f, 0.0f);

        mViewport = mWindow->addViewport(mCamera);
        mViewport->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.5f));
        mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));

        ResourceGroupManager::getSingleton().addResourceLocation("resource.zip", "Zip");
        ResourceGroupManager::getSingleton().addResourceLocation("fish.zip", "Zip");
        ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

        mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

        // 좌표축 표시
        Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
        mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
        mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

        _drawGridPlane();


        Entity* entity1 = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
        SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Professor", Vector3(0.0f, 0.0f, 0.0f));
        node1->rotate(Vector3(0.0f, 1.0f, 0.0f), Degree(180.0f));
        node1->attachObject(entity1);

        string strBullet = "Bullet";
        Entity* bullet[Max_Bullet];
        SceneNode* bulletNode[Max_Bullet];
        for (int i = 0; i < Max_Bullet; ++i)
        {
            bullet[i] = mSceneMgr->createEntity(strBullet + to_string(i), "fish.mesh");
            bulletNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode(strBullet + to_string(i), Vector3(0.0f, 0.0f, 0.0f));
            bulletNode[i]->attachObject(bullet[i]);
            bulletNode[i]->setScale(5.0f, 5.0f, 5.0f);
            bulletNode[i]->rotate(Vector3(0.0f, 1.0f, 0.0f), Degree(-270.0f));
            bulletNode[i]->setPosition(0.0f, 120.0f, -10.0f);
        }

        size_t windowHnd = 0;
        std::ostringstream windowHndStr;
        OIS::ParamList pl;
        mWindow->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
        pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
        mInputManager = OIS::InputManager::createInputSystem(pl);


        mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
        mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

        InputController* inputController = new InputController(mRoot, mKeyboard, mMouse);
        mRoot->addFrameListener(inputController);

        mESCListener = new ESCListener(mKeyboard);
        mRoot->addFrameListener(mESCListener);

        mMainListener = new MainListener(mRoot, mKeyboard);
        mRoot->addFrameListener(mMainListener);

        mRoot->startRendering();

        mInputManager->destroyInputObject(mKeyboard);
        mInputManager->destroyInputObject(mMouse);
        OIS::InputManager::destroyInputSystem(mInputManager);

        delete inputController;

        delete mRoot;
    }

private:
    void _drawGridPlane(void)
    {
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
};


#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        LectureApp app;

        try {

            app.go();

        }
        catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif

