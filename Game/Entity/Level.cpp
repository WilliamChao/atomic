#include "stdafx.h"
#include "types.h"
#include "Sound/AtomicSound.h"
#include "Graphics/ResourceManager.h"
#include "Graphics/Renderer.h"
#include "Game/AtomicApplication.h"
#include "Game/AtomicGame.h"
#include "Game/World.h"
#include "Game/SPHManager.h"
#include "Game/Collision.h"
#include "Game/Message.h"
#include "GPGPU/SPH.cuh"
#include "Util.h"
#include "Enemy.h"

namespace atomic {


class Level_Test : public IEntity
{
typedef IEntity super;
private:
    int m_frame;
    CollisionHandle m_planes[4];

public:
    Level_Test() : m_frame(0)
    {
    }

    void initialize()
    {
        super::initialize();

        const vec4 field_size = vec4(2.56f);
        atomicGetWorld()->setFieldSize(field_size);

        vec4 planes[] = {
            vec4(-1.0f, 0.0f, 0.0f, field_size.x),
            vec4( 1.0f, 0.0f, 0.0f, field_size.x),
            vec4( 0.0f,-1.0f, 0.0f, field_size.y),
            vec4( 0.0f, 1.0f, 0.0f, field_size.y),
        };
        for(uint32 i=0; i<_countof(planes); ++i) {
            CollisionPlane *p = atomicCreateCollision(CollisionPlane);
            p->setGObjHandle(getHandle());
            p->setFlags(CF_SENDER|CF_AFFECT_SPH);
            p->plane = planes[i];
            m_planes[i] = p->getCollisionHandle();
        }
    }

    void finalize()
    {
        for(uint32 i=0; i<4; ++i) {
            atomicDeleteCollision(m_planes[i]);
        }
        super::finalize();
    }

    IEntity* createRandomEnemy()
    {
        IEntity *e = NULL;
        switch(atomicGetRandom()->genInt32() % 2) {
        case 0: e = atomicCreateEntity(Enemy_CubeBasic);  atomicCall(e, setModel, PSET_CUBE_MEDIUM); break;
        case 1: e = atomicCreateEntity(Enemy_SphereBasic);atomicCall(e, setModel, PSET_SPHERE_MEDIUM);  break;
        }
        
        atomicCall(e, setPosition, GenRandomVector2() * 2.56f);
        atomicCall(e, setHealth, 75.0f);
        atomicCall(e, setAxis1, GenRandomUnitVector3());
        atomicCall(e, setAxis2, GenRandomUnitVector3());
        atomicCall(e, setRotateSpeed1, 0.4f);
        atomicCall(e, setRotateSpeed2, 0.4f);
        return e;
    }

    void update(float32 dt)
    {
        ++m_frame;
        updateCamera();

        if(m_frame==1) {
            {
                IEntity *e =  atomicCreateEntity(Player);
                atomicCall(e, setPosition, vec4(0.0f, 0.0f, 0.0f, 1.0f));
            }
            {
                IEntity *e =  atomicCreateEntity(Enemy_CubeBasic);
                atomicCall(e, setModel, PSET_CUBE_MEDIUM);
                atomicCall(e, setPosition, vec4(0.5f, 0.0f, 0.0f, 1.0f));
                atomicCall(e, setAxis1, GenRandomUnitVector3());
                atomicCall(e, setAxis2, GenRandomUnitVector3());
                atomicCall(e, setRotateSpeed1, 0.4f);
                atomicCall(e, setRotateSpeed2, 0.4f);
            }
            {
                IEntity *e =  atomicCreateEntity(Enemy_SphereBasic);
                atomicCall(e, setModel, PSET_SPHERE_MEDIUM);
                atomicCall(e, setPosition, vec4(-0.5f, 0.0f, 0.0f, 1.0f));
                atomicCall(e, setAxis1, GenRandomUnitVector3());
                atomicCall(e, setAxis2, GenRandomUnitVector3());
                atomicCall(e, setRotateSpeed1, 0.4f);
                atomicCall(e, setRotateSpeed2, 0.4f);
            }
        }

        if(m_frame % 300 == 0) {
            IEntity *e = createRandomEnemy();
        }
    }

    void updateCamera()
    {
        PerspectiveCamera *pcam = atomicGetCamera();
        if(IEntity *player = atomicGetEntity( EntityCreateHandle(ECID_PLAYER, ESID_PLAYER, 0) )) {
            variant v;
            if(atomicQuery(player, getPosition, v)) {
                vec4 player_pos = v.cast<vec4>();
                vec4 cpos = pcam->getPosition();
                vec4 tpos = pcam->getTarget();
                vec4 cpos2 = cpos + (player_pos-cpos)*0.03f;
                vec4 tpos2 = tpos + (player_pos-tpos)*0.03f;
                cpos2.z = cpos.z;
                tpos2.z = tpos.z;
                pcam->setPosition(cpos2);
                pcam->setTarget(tpos2);

                atomicSetListenerPosition(cpos2);
            }
        }
    }
};

atomicImplementEntity(Level_Test, ECID_LEVEL, ESID_LEVEL_TEST);

} // namespace atomic