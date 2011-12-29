#include "stdafx.h"
#include "types.h"
#include "Util.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Renderer.h"
#include "Game/AtomicApplication.h"
#include "Game/AtomicGame.h"
#include "Game/World.h"
#include "Game/SPHManager.h"
#include "Game/Collision.h"
#include "Game/Message.h"
#include "GPGPU/SPH.cuh"
#include "Enemy.h"

namespace atomic {


class Player
    : public Breakable
    , public TAttr_RotateSpeed<Attr_DoubleAxisRotation>
{
typedef Breakable super;
typedef TAttr_RotateSpeed<Attr_DoubleAxisRotation> transform;
private:
    static const PSET_RID pset_id = PSET_SPHERE_SMALL;

    vec4 m_boost;
    int32 m_cooldown;
    Attr_SphereCollision m_collision;
    Attr_SphereCollision m_barrier;

public:
    Player() : m_cooldown(0)
    {
    }

    virtual void initialize()
    {
        super::initialize();
        m_collision.initializeCollision(getHandle());
        m_barrier.initializeCollision(0);
        m_barrier.setCollisionFlag(CF_AFFECT_SPH);
    }

    virtual void update(float32 dt)
    {
        m_cooldown = std::max<int32>(0, m_cooldown-1);

        vec4 move = vec4(atomicGetInputs()->getMove()*0.01f, 0.0f, 0.0f);
        if(m_cooldown==0 && atomicGetInputs()->isButtonTriggered(1)) {
            m_boost += move * 2.0f;
            m_cooldown = 10;
        }

        vec4 pos = getPosition();
        pos += move;
        pos += m_boost;
        pos.z = 0.0f;
        setPosition(pos);

        m_boost *= 0.96f;

        {
            sphForcePointGravity pg;
            pg.position = (float4&)pos;
            pg.strength = 2.0f;
            atomicGetSPHManager()->addForce(pg);
        }


        super::update(dt);
        transform::update(dt);

        setTransform(computeMatrix());
        m_collision.updateCollision(pset_id, getTransform(), 0.5f);
        m_barrier.updateCollision(pset_id, getTransform(), 3.0f);
    }

    virtual void draw()
    {
        {
            PointLight l;
            l.setPosition(getPosition()+vec4(0.0f, 0.0f, 0.3f, 0.0f));
            l.setColor(vec4(0.1f, 0.2f, 1.0f, 1.0f));
            l.setRadius(1.0f);
            atomicGetPointLights()->addInstance(l);
        }
        {
            atomicGetSPHRenderer()->addPSetInstance(pset_id, getTransform(),
                vec4(0.6f, 0.6f, 0.6f, 1.0f), vec4(0.2f, 0.0f, 1.0f, 1.0f), vec4());
        }
    }

    virtual void destroy()
    {
    }

    virtual void eventCollide(const CollideMessage *m)
    {
        vec4 v = m->direction * m->direction.w * 0.2f;
        m_boost += v;
        m_boost.z = 0.0f;
        m_boost.w = 0.0f;
    }

    bool call(uint32 call_id, const variant &v)
    {
        return super::call(call_id, v) || transform::call(call_id, v);
    }

    bool query(uint32 query_id, variant &v) const
    {
        return super::query(query_id, v) || transform::query(query_id, v);
    }
};

atomicImplementEntity(Player, ECID_PLAYER, ESID_PLAYER);

} // namespace atomic