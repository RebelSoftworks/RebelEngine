//========= Copyright (c) 2026 Mohamed Ashraf, All rights reserved. ============//
//
// This software is proprietary and confidential.
//
// You are granted a limited, non-exclusive, non-transferable license to use and
// modify this software for personal or internal purposes only. You may distribute
// compiled binary builds of the software provided that such distribution does not
// expose, include, or facilitate access to the source code.
//
// You may not distribute, publish, disclose, or otherwise make available the
// source code, in whole or in part, whether original or modified. All rights
// not expressly granted herein are reserved by the copyright holder.
//
// This software is provided "as is", without warranty of any kind, express or
// implied.
//
//=============================================================================//

#include <vphysics2_interface.h>
#include "../engine2/gametimer.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

IPhysics2ActorManager* g_pPhysics2ActorManager = nullptr;

static constexpr float PHYSICS_GRAVITY = -600.0f;
static constexpr float PHYSICS_RESTITUTION = 0.15f;
static constexpr float PHYSICS_FRICTION = 0.8f;
static constexpr float PHYSICS_EPSILON = 0.0001f;

// TODO: Should we move this to mathlib?
template <typename T>
const T& clamp(const T& val, const T& low, const T& high) {
    return (val < low) ? low : (high < val) ? high : val;
}

class CPhysics2CookedPolytope : public IPhysics2CookedPolytope
{
public:
    unsigned int GetType() const override { return PHYSICS2_MESH_TYPE_POLYTOPE; }
    unsigned int NumVertices() const override { return 0; }
    unsigned int NumTriangles() const override { return 0; }
    void GetVertices(float* pVertsOut, unsigned int nByteStride, const fltx4& factor) override {}
    void GetTriangles(int* pTrisOut, unsigned int nByteStride) override {}
    unsigned int GetSizeOf() const override { return sizeof(CPhysics2CookedPolytope); }
    CPhysics2Polytope* GetPolytope() override { return nullptr; }
};

class CPhysics2CookedPolysoup : public IPhysics2CookedPolysoup
{
public:
    unsigned int GetType() const override { return PHYSICS2_MESH_TYPE_POLYSOUP; }
    unsigned int NumVertices() const override { return 0; }
    unsigned int NumTriangles() const override { return 0; }
    void GetVertices(float* pVertsOut, unsigned int nByteStride, const fltx4& factor) override {}
    void GetTriangles(int* pTrisOut, unsigned int nByteStride) override {}
    unsigned int GetSizeOf() const override { return sizeof(CPhysics2CookedPolysoup); }
    CPhysics2Polysoup* GetPolysoup() override { return m_pPolysoup; }
    void ExportObj(const char* szPath) override {}

private:
    CPhysics2Polysoup m_Polysoup;
    CPhysics2Polysoup* m_pPolysoup = &m_Polysoup;
};

class CPhysics2CookedMopp : public IPhysics2CookedMopp
{
public:
    unsigned int GetType() const override { return PHYSICS2_MESH_TYPE_MOPP; }
    unsigned int NumVertices() const override { return 0; }
    unsigned int NumTriangles() const override { return 0; }
    void GetVertices(float* pVertsOut, unsigned int nByteStride, const fltx4& factor) override {}
    void GetTriangles(int* pTrisOut, unsigned int nByteStride) override {}
    unsigned int GetSizeOf() const override { return sizeof(CPhysics2CookedMopp); }
    CPhysics2Mopp* GetMopp() override { return nullptr; }
};

class CPhysics2CookedInertia : public IPhysics2CookedInertia
{
public:
    CPhysics2Inertia* GetInertia() override { return nullptr; }
};

class CPhysics2Level : public IPhysics2Level
{
public:
    bool HasMopp() override { return false; }
};

class CPhysics2BoxInternal : public CPhysics2Box
{
public:
    CPhysics2BoxInternal(float x, float y, float z)
    {
        m_HalfSize.m128_f32[0] = x;
        m_HalfSize.m128_f32[1] = y;
        m_HalfSize.m128_f32[2] = z;
        m_HalfSize.m128_f32[3] = 0.0f;
    }

    fltx4 m_HalfSize{};
};

class CPhysics2Inertia
{
public:
    float m_Mass = 1.0f;
    float m_InvMass = 1.0f;
};

class CPhysics2ActorInternal : public CPhysics2Actor
{
public:
    CPhysics2Shape* m_pShape = nullptr;
    CPhysics2Inertia* m_pInertia = nullptr;

    fltx4 m_Position{};
    fltx4 m_Velocity{};

    Quaternion m_Orientation = Quaternion(0, 0, 0, 1);

    unsigned int* m_pUserData = nullptr;

    unsigned int m_Flags = 0;

    bool m_Active = true;

    float GetInvMass() const
    {
        if (m_Flags & PHYSICS2_IS_ACTOR_STATIC)
            return 0.0f;

        if (!m_pInertia)
            return 1.0f;

        return static_cast<CPhysics2Inertia*>(m_pInertia)->m_InvMass;
    }

    CPhysics2BoxInternal* GetBox() const
    {
        if (!m_pShape)
            return nullptr;

        return static_cast<CPhysics2BoxInternal*>(m_pShape);
    }
};

static fltx4 PhysicsAdd(const fltx4& a, const fltx4& b)
{
    fltx4 result{};

    result.m128_f32[0] = a.m128_f32[0] + b.m128_f32[0];
    result.m128_f32[1] = a.m128_f32[1] + b.m128_f32[1];
    result.m128_f32[2] = a.m128_f32[2] + b.m128_f32[2];
    result.m128_f32[3] = 0.0f;

    return result;
}

static fltx4 PhysicsSubtract(const fltx4& a, const fltx4& b)
{
    fltx4 result{};

    result.m128_f32[0] = a.m128_f32[0] - b.m128_f32[0];
    result.m128_f32[1] = a.m128_f32[1] - b.m128_f32[1];
    result.m128_f32[2] = a.m128_f32[2] - b.m128_f32[2];
    result.m128_f32[3] = 0.0f;

    return result;
}

static fltx4 PhysicsMultiply(const fltx4& value, float scalar)
{
    fltx4 result{};

    result.m128_f32[0] = value.m128_f32[0] * scalar;
    result.m128_f32[1] = value.m128_f32[1] * scalar;
    result.m128_f32[2] = value.m128_f32[2] * scalar;
    result.m128_f32[3] = 0.0f;

    return result;
}

static float PhysicsDot(const fltx4& a, const fltx4& b)
{
    return
        a.m128_f32[0] * b.m128_f32[0] +
        a.m128_f32[1] * b.m128_f32[1] +
        a.m128_f32[2] * b.m128_f32[2];
}

static float PhysicsLength(const fltx4& value)
{
    return std::sqrt(PhysicsDot(value, value));
}

static fltx4 PhysicsNormalize(const fltx4& value)
{
    float length = PhysicsLength(value);

    if (length <= PHYSICS_EPSILON)
        return fltx4{};

    return PhysicsMultiply(value, 1.0f / length);
}

static bool PhysicsAABBOverlap(
    const CPhysics2ActorInternal* a,
    const CPhysics2ActorInternal* b,
    fltx4& normal,
    float& penetration)
{
    CPhysics2BoxInternal* boxA = a->GetBox();
    CPhysics2BoxInternal* boxB = b->GetBox();

    if (!boxA || !boxB)
        return false;

    fltx4 delta = PhysicsSubtract(b->m_Position, a->m_Position);

    float overlapX =
        boxA->m_HalfSize.m128_f32[0] +
        boxB->m_HalfSize.m128_f32[0] -
        std::fabs(delta.m128_f32[0]);

    float overlapY =
        boxA->m_HalfSize.m128_f32[1] +
        boxB->m_HalfSize.m128_f32[1] -
        std::fabs(delta.m128_f32[1]);

    float overlapZ =
        boxA->m_HalfSize.m128_f32[2] +
        boxB->m_HalfSize.m128_f32[2] -
        std::fabs(delta.m128_f32[2]);

    if (overlapX <= 0.0f ||
        overlapY <= 0.0f ||
        overlapZ <= 0.0f)
    {
        return false;
    }

    penetration = overlapX;

    normal = fltx4{};
    normal.m128_f32[0] = delta.m128_f32[0] >= 0.0f ? 1.0f : -1.0f;

    if (overlapY < penetration)
    {
        penetration = overlapY;

        normal = fltx4{};
        normal.m128_f32[1] = delta.m128_f32[1] >= 0.0f ? 1.0f : -1.0f;
    }

    if (overlapZ < penetration)
    {
        penetration = overlapZ;

        normal = fltx4{};
        normal.m128_f32[2] = delta.m128_f32[2] >= 0.0f ? 1.0f : -1.0f;
    }

    return true;
}

static void PhysicsResolveCollision(
    CPhysics2ActorInternal* a,
    CPhysics2ActorInternal* b,
    const fltx4& normal,
    float penetration)
{
    float invMassA = a->GetInvMass();
    float invMassB = b->GetInvMass();

    float totalInvMass = invMassA + invMassB;

    if (totalInvMass <= PHYSICS_EPSILON)
        return;

    fltx4 correction = PhysicsMultiply(
        normal,
        penetration / totalInvMass
    );

    if (invMassA > 0.0f)
    {
        a->m_Position = PhysicsSubtract(
            a->m_Position,
            PhysicsMultiply(correction, invMassA)
        );
    }

    if (invMassB > 0.0f)
    {
        b->m_Position = PhysicsAdd(
            b->m_Position,
            PhysicsMultiply(correction, invMassB)
        );
    }

    fltx4 relativeVelocity = PhysicsSubtract(
        b->m_Velocity,
        a->m_Velocity
    );

    float velocityAlongNormal = PhysicsDot(
        relativeVelocity,
        normal
    );

    if (velocityAlongNormal > 0.0f)
        return;

    float impulseMagnitude =
        -(1.0f + PHYSICS_RESTITUTION) *
        velocityAlongNormal /
        totalInvMass;

    fltx4 impulse = PhysicsMultiply(
        normal,
        impulseMagnitude
    );

    if (invMassA > 0.0f)
    {
        a->m_Velocity = PhysicsSubtract(
            a->m_Velocity,
            PhysicsMultiply(impulse, invMassA)
        );
    }

    if (invMassB > 0.0f)
    {
        b->m_Velocity = PhysicsAdd(
            b->m_Velocity,
            PhysicsMultiply(impulse, invMassB)
        );
    }

    fltx4 tangent = PhysicsSubtract(
        relativeVelocity,
        PhysicsMultiply(
            normal,
            velocityAlongNormal
        )
    );

    float tangentLength = PhysicsLength(tangent);

    if (tangentLength <= PHYSICS_EPSILON)
        return;

    tangent = PhysicsNormalize(tangent);

    float frictionImpulse =
        -PhysicsDot(relativeVelocity, tangent) /
        totalInvMass;

    float maxFriction =
        impulseMagnitude *
        PHYSICS_FRICTION;

    frictionImpulse = clamp(
        frictionImpulse,
        -maxFriction,
        maxFriction
    );

    fltx4 friction = PhysicsMultiply(
        tangent,
        frictionImpulse
    );

    if (invMassA > 0.0f)
    {
        a->m_Velocity = PhysicsSubtract(
            a->m_Velocity,
            PhysicsMultiply(friction, invMassA)
        );
    }

    if (invMassB > 0.0f)
    {
        b->m_Velocity = PhysicsAdd(
            b->m_Velocity,
            PhysicsMultiply(friction, invMassB)
        );
    }
}

class CPhysics2World : public IPhysics2World
{
public:
    CPhysics2Actor* AddActor(CPhysics2Shape* pShape, CPhysics2Inertia* pInertia, unsigned int flags = 0) override
    {
        if (!pShape)
            return nullptr;

        auto actor = std::make_unique<CPhysics2ActorInternal>();

        actor->m_pShape = pShape;
        actor->m_pInertia = pInertia;
        actor->m_Flags = flags;

        actor->m_Position = fltx4{};
        actor->m_Velocity = fltx4{};

        CPhysics2Actor* result = actor.get();

        m_Actors.push_back(std::move(actor));

        return result;
    }

    void Destroy(CPhysics2Actor* pActor) override
    {
        if (!pActor)
            return;

        auto it = std::find_if(
            m_Actors.begin(),
            m_Actors.end(),
            [pActor](const std::unique_ptr<CPhysics2ActorInternal>& actor)
            {
                return actor.get() == pActor;
            }
        );

        if (it != m_Actors.end())
            m_Actors.erase(it);
    }

    int NumActiveActors() override
    {
        int count = 0;

        for (const auto& actor : m_Actors)
        {
            if (actor->m_Active)
                ++count;
        }

        return count;
    }

    void GetActiveActors(CPhysics2Actor** ppActors) override
    {
        if (!ppActors)
            return;

        int index = 0;

        for (const auto& actor : m_Actors)
        {
            if (!actor->m_Active)
                continue;

            ppActors[index++] = actor.get();
        }
    }

    void Reset() override
    {
        m_Actors.clear();
    }

    void Simulate(float deltaTime) override
    {
        if (deltaTime <= 0.0f)
            return;

        deltaTime = min(deltaTime, 0.1f);

        for (auto& actor : m_Actors)
        {
            if (!actor->m_Active)
                continue;

            if (actor->m_Flags & PHYSICS2_IS_ACTOR_STATIC)
                continue;

            actor->m_Velocity.m128_f32[2] +=
                PHYSICS_GRAVITY * deltaTime;

            actor->m_Position = PhysicsAdd(
                actor->m_Position,
                PhysicsMultiply(
                    actor->m_Velocity,
                    deltaTime
                )
            );

            CPhysics2BoxInternal* box = actor->GetBox();

            if (!box)
                continue;

            float bottom =
                actor->m_Position.m128_f32[2] -
                box->m_HalfSize.m128_f32[2];

            if (bottom < 0.0f)
            {
                actor->m_Position.m128_f32[2] -= bottom;

                if (actor->m_Velocity.m128_f32[2] < 0.0f)
                {
                    actor->m_Velocity.m128_f32[2] =
                        -actor->m_Velocity.m128_f32[2] *
                        PHYSICS_RESTITUTION;

                    if (std::fabs(actor->m_Velocity.m128_f32[2]) < 1.0f)
                        actor->m_Velocity.m128_f32[2] = 0.0f;
                }

                actor->m_Velocity.m128_f32[0] *= 0.98f;
                actor->m_Velocity.m128_f32[1] *= 0.98f;
            }
        }

        for (int iteration = 0; iteration < 4; ++iteration)
        {
            for (size_t i = 0; i < m_Actors.size(); ++i)
            {
                for (size_t j = i + 1; j < m_Actors.size(); ++j)
                {
                    CPhysics2ActorInternal* a =
                        m_Actors[i].get();

                    CPhysics2ActorInternal* b =
                        m_Actors[j].get();

                    if (!a->m_Active || !b->m_Active)
                        continue;

                    if ((a->m_Flags & PHYSICS2_IS_ACTOR_STATIC) &&
                        (b->m_Flags & PHYSICS2_IS_ACTOR_STATIC))
                    {
                        continue;
                    }

                    fltx4 normal{};
                    float penetration = 0.0f;

                    if (!PhysicsAABBOverlap(
                        a,
                        b,
                        normal,
                        penetration))
                    {
                        continue;
                    }

                    PhysicsResolveCollision(
                        a,
                        b,
                        normal,
                        penetration
                    );
                }
            }
        }
    }

private:
    std::vector<std::unique_ptr<CPhysics2ActorInternal>> m_Actors;
};

class CPhysics2ActorManager : public IPhysics2ActorManager
{
public:
    unsigned int* GetUserData(
        const CPhysics2Actor* pActor) override
    {
        if (!pActor)
            return nullptr;

        return static_cast<const CPhysics2ActorInternal*>(
            pActor
            )->m_pUserData;
    }

    void SetUserData(
        CPhysics2Actor* pActor,
        unsigned int* userData) override
    {
        if (!pActor)
            return;

        static_cast<CPhysics2ActorInternal*>(
            pActor
            )->m_pUserData = userData;
    }

    const fltx4 GetPosition(
        const CPhysics2Actor* pActor) override
    {
        if (!pActor)
            return fltx4{};

        return static_cast<const CPhysics2ActorInternal*>(
            pActor
            )->m_Position;
    }

    const Quaternion GetOrientation(
        const CPhysics2Actor* pActor) override
    {
        if (!pActor)
            return Quaternion(0, 0, 0, 1);

        return static_cast<const CPhysics2ActorInternal*>(
            pActor
            )->m_Orientation;
    }

    void SetPosition(
        const CPhysics2Actor* pActor,
        const fltx4& pos) override
    {
        if (!pActor)
            return;

        CPhysics2ActorInternal* actor =
            const_cast<CPhysics2ActorInternal*>(
                static_cast<const CPhysics2ActorInternal*>(pActor)
                );

        actor->m_Position = pos;
    }

    void SetOrientation(
        const CPhysics2Actor* pActor,
        const Quaternion& orientation) override
    {
        if (!pActor)
            return;

        CPhysics2ActorInternal* actor =
            const_cast<CPhysics2ActorInternal*>(
                static_cast<const CPhysics2ActorInternal*>(pActor)
                );

        actor->m_Orientation = orientation;
    }

    void SetVelocity(
        const CPhysics2Actor* pActor,
        const fltx4& vel) override
    {
        if (!pActor)
            return;

        CPhysics2ActorInternal* actor =
            const_cast<CPhysics2ActorInternal*>(
                static_cast<const CPhysics2ActorInternal*>(pActor)
                );

        actor->m_Velocity = vel;
    }

    void Destroy(CPhysics2Actor* pActor) override
    {
        if (!pActor)
            return;

        static_cast<CPhysics2ActorInternal*>(
            pActor
            )->m_Active = false;
    }
};

class CPhysics2Collision : public IPhysics2Collision
{
public:
    void Login(IPhysics2Level*) override {}
    void Logout(IPhysics2Level*) override {}
};

class CPhysics2Cook : public IPhysics2Cook
{
public:
    IPhysics2CookedPolytope* CookPolytopeFromPlanes(
        const Vector4* pPlanes,
        int planeCount,
        float mergeDistance = 0.001f) override
    {
        return nullptr;
    }

    IPhysics2CookedPolytope* CookPolytopeFromVertices(
        const Vector4* pVertices,
        int vertexCount,
        float mergeDistance = 0.001f) override
    {
        return nullptr;
    }

    IPhysics2CookedPolytope* CookPolytope(
        IPhysics2CookedMeshBase* pMesh) override
    {
        return nullptr;
    }

    void* Serialize(CPhysics2Polytope* pShape) override
    {
        return nullptr;
    }

    void Destroy(IPhysics2CookedPolytope* pShape, unsigned flags = 0) override
    {
        delete pShape;
    }

    IPhysics2CookedPolysoup* CookPolysoupFromMeshes(IPhysics2CookedMeshBase* const* ppMeshes, int numMeshes) override
    {
        return nullptr;
    }

    void* Serialize(CPhysics2Polysoup* pPolysoup) override
    {
        return nullptr;
    }

    void Destroy(IPhysics2CookedPolysoup* pShape) override
    {
        delete pShape;
    }

    IPhysics2CookedInertia* CookInertia(CPhysics2Shape* pShape) override
    {
        if (!pShape)
            return nullptr;

        return new CPhysics2CookedInertia();
    }

    void* Serialize(CPhysics2Inertia* pInertia) override
    {
        return nullptr;
    }

    void Destroy(IPhysics2CookedInertia* pInertia, unsigned flags = 0) override
    {
        delete pInertia;
    }

    IPhysics2CookedMopp* CookMopp(IPhysics2CookedMeshBase* const* ppMeshes, int numMeshes) override
    {
        return nullptr;
    }

    void* Serialize(CPhysics2Mopp* pMopp) override
    {
        return nullptr;
    }

    void Destroy(IPhysics2CookedMopp* pShape) override
    {
        delete pShape;
    }

    void ExportObj(const char* szFileName, IPhysics2CookedMeshBase* const* ppMeshes, int numMeshes) override
    {
    }
};

class CPhysics2ResourceManager : public IPhysics2ResourceManager
{
public:
    CPhysics2Polytope* UnserializePolytope(const void* pBuffer) override
    {
        return nullptr;
    }

    void Release(CPhysics2Polytope* pShape, unsigned flag = 0) override
    {
        delete pShape;
    }

    CPhysics2Inertia* UnserializeInertia(const void* pBuffer) override
    {
        return nullptr;
    }

    void Release(CPhysics2Inertia* pInertia,unsigned flag = 0) override
    {
        delete pInertia;
    }

    CPhysics2Polysoup* UnserializePolysoup(
        const void* pBuffer) override
    {
        return nullptr;
    }

    void Release(
        CPhysics2Polysoup* pPolysoup,
        unsigned flag = 0) override
    {
        delete pPolysoup;
    }

    CPhysics2Mopp* UnserializeMopp(
        const void* pBuffer) override
    {
        return nullptr;
    }

    void Release(
        CPhysics2Mopp* pMopp,
        unsigned flag = 0) override
    {
        delete pMopp;
    }

    IPhysics2Level* UnserializeLevel(
        const struct dphyslevelV0_t* pRoot) override
    {
        return new CPhysics2Level();
    }

    void Release(IPhysics2Level* pLevel) override
    {
        delete pLevel;
    }

    CPhysics2Box* CreateBoxShape(
        const fltx4& halfSize) override
    {
        return new CPhysics2BoxInternal(
            max(halfSize.m128_f32[0], 0.001f),
            max(halfSize.m128_f32[1], 0.001f),
            max(halfSize.m128_f32[2], 0.001f)
        );
    }

    void Release(CPhysics2Box* pBox) override
    {
        delete static_cast<CPhysics2BoxInternal*>(pBox);
    }

    CPhysics2Shape* GetStockShape(
        unsigned int nShape) override
    {
        switch (nShape)
        {
        case PHYSICS2_STOCK_BOX_1M:
            if (!m_pStockBox)
            {
                fltx4 halfSize{};
                halfSize.m128_f32[0] = 0.5f;
                halfSize.m128_f32[1] = 0.5f;
                halfSize.m128_f32[2] = 0.5f;

                m_pStockBox = CreateBoxShape(halfSize);
            }

            return m_pStockBox;

        case PHYSICS2_STOCK_SPHERE_1M:
            if (!m_pStockSphere)
            {
                fltx4 halfSize{};
                halfSize.m128_f32[0] = 0.5f;
                halfSize.m128_f32[1] = 0.5f;
                halfSize.m128_f32[2] = 0.5f;

                m_pStockSphere = CreateBoxShape(halfSize);
            }

            return m_pStockSphere;
        }

        return nullptr;
    }

    void Release(
        CPhysics2Shape* pShape,
        unsigned flag = 0) override
    {
        if (!pShape)
            return;

        if (pShape == m_pStockBox ||
            pShape == m_pStockSphere)
        {
            return;
        }

        delete static_cast<CPhysics2BoxInternal*>(pShape);
    }

    ~CPhysics2ResourceManager()
    {
        delete static_cast<CPhysics2BoxInternal*>(m_pStockBox);
        delete static_cast<CPhysics2BoxInternal*>(m_pStockSphere);

        m_pStockBox = nullptr;
        m_pStockSphere = nullptr;
    }

private:
    CPhysics2Shape* m_pStockBox = nullptr;
    CPhysics2Shape* m_pStockSphere = nullptr;
};

class CPhysics2 : public CBaseAppSystem<IPhysics2>
{
public:
    IPhysics2World* CreateWorld(
        unsigned int flags = 0) override
    {
        auto world = std::make_unique<CPhysics2World>();

        IPhysics2World* result = world.get();

        m_Worlds.push_back(std::move(world));

        return result;
    }

    int NumWorlds() override
    {
        return static_cast<int>(m_Worlds.size());
    }

    IPhysics2World* GetWorld(int nWorld) override
    {
        if (nWorld < 0 ||
            nWorld >= static_cast<int>(m_Worlds.size()))
        {
            return nullptr;
        }

        return m_Worlds[nWorld].get();
    }

    void Destroy(IPhysics2World* pWorld) override
    {
        if (!pWorld)
            return;

        auto it = std::find_if(
            m_Worlds.begin(),
            m_Worlds.end(),
            [pWorld](const std::unique_ptr<CPhysics2World>& world)
            {
                return world.get() == pWorld;
            }
        );

        if (it != m_Worlds.end())
            m_Worlds.erase(it);
    }

    IPhysics2Cook* GetCook() override
    {
        return &m_Cook;
    }

    IPhysics2ResourceManager* GetResourceManager() override
    {
        return &m_ResourceManager;
    }

    IPhysics2Collision* GetCollision() override
    {
        return &m_Collision;
    }

    IPhysics2ActorManager* GetActorManager() override
    {
        return &m_ActorManager;
    }

    int GetSerializeVersion() override
    {
        return 1;
    }

    int NumThreads() override
    {
        return 1;
    }

private:
    std::vector<std::unique_ptr<CPhysics2World>> m_Worlds;

    CPhysics2Cook m_Cook;
    CPhysics2ResourceManager m_ResourceManager;
    CPhysics2Collision m_Collision;
    CPhysics2ActorManager m_ActorManager;
};

CPhysics2 g_Physics2;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CPhysics2, IPhysics2, VPHSYICS2_INTERFACE_VERSION, g_Physics2);

ISource2Client* g_pSource2Client = nullptr;
ISource2ClientConfig* g_pSource2ClientConfig = nullptr;
IRenderDeviceMgr* g_pRenderDeviceManager = nullptr;
IRenderUtils* g_pRenderUtils = nullptr;
IRenderDevice* g_Device = nullptr;
IPrediction2* g_pSource2ClientPrediction = nullptr;
ISource2ServerConfig* g_pSource2ServerConfig = nullptr;
ISource2GameEntities* g_sSource2GameEntities = nullptr;
IInputSystem* g_pInputSystem = nullptr;
ISource2GameClients* g_pGameClients = nullptr;
IInputStackSystem* g_pInputStackSystem = nullptr;
ISource2Server* g_pSource2Server = nullptr;