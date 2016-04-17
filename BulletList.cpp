#include "BulletList.h"

void CBulletList::Init()
{
    CNode *start = head.m_next;
    CNode *temp = nullptr;

    while (start->m_next != nullptr)
    {
        temp = start;
        start = start->m_next;
        delete temp;
    }

    head.m_next = start;
    bulletCount = 0;
}

void CBulletList::Add(float x, float y, float z, float dx, float dy, float dz)
{
    if (bulletCount >= Max_Bullet) { } // ÃÑ¾Ë ´Ù »ç¿ë
    else
    {
        CNode* newNode = new CNode(x, y, z, dx, dy, dz, head.m_next, bulletCount++);
        head.m_next = newNode;
    }
}

void CBulletList::Remove(CNode *die)
{
    CNode* start = head.m_next;
    CNode* pred = &head;

    while (start != die)
    {
        pred = start;
        start = start->m_next;
    }

    pred->m_next = start->m_next;
    delete start;
}

void CBulletList::Update(float deltaTime)
{
    CNode *start = head.m_next;
    if (start == &tail) return;
    else
    {
        while (start != &tail)
        {
            start->mLifeTime -= deltaTime;
            if (start->mLifeTime <= 0.0f) Remove(start);
            else
            {
                start->mPos.x += start->mDir.x * start->bulletSpeed * deltaTime;
                start->mPos.y += start->mDir.y * start->bulletSpeed * deltaTime;
                start->mPos.z += start->mDir.z * start->bulletSpeed * deltaTime;
            }
            start = start->m_next;
        }
    }
}