#pragma once

struct POSITION
{
    float x;
    float y;
    float z;
};

class CNode
{
public:
    const int bulletSpeed = 10000.0f;
    float mLifeTime = 3.0f;
    POSITION mPos;
    POSITION mDir;
    int mKey;

    CNode *m_next;

    CNode() : m_next(nullptr), mKey(-1)
    { 
        mPos.x = 0.0f; mPos.y = 0.0f; mPos.z = 0.0f;
        mDir.x = 0.0f; mDir.y = 0.0f; mDir.z = 0.0f;
    }
    CNode(float x, float y, float z, float dx, float dy, float dz, CNode *next, int key) : m_next(next), mKey(key)
    {
        mPos.x = x; mPos.y = y; mPos.z = z;
        mDir.x = dx; mDir.y = dy; mDir.z = dz;
    }
    ~CNode() {}

    int getKey() { return mKey; }
};

class CBulletList
{
    const int Max_Bullet = 30;
    CNode head, tail;
    int bulletCount = 0;

public:
    CBulletList() { head.m_next = &tail; }
    ~CBulletList() {}

    void Init();
    void Add(float x, float y, float z, float dx, float dy, float dz);
    void Remove(CNode *die);
    void Update(float deltaTime);
    CNode * getHead() { return &head; }
    CNode * getTail() { return &tail; }
};
