#include "stdafx.h"
#include "ist/ist.h"
#include "types.h"
#include "AtomicApplication.h"
#include "AtomicGame.h"
#include "Game/Message.h"
#include "Game/SPHManager.h"
#include "Game/Entity.h"
#include "Game/Collision.h"
#include "Game/VFX.h"
#include "Game/World.h"
#include "Graphics/Renderer.h"
#include "EntityQuery.h"
#include "EntityClass.h"
#include "Util.h"

using namespace ist::i3dgl;


namespace atomic {



World::World()
: m_collision_set(NULL)
, m_entity_set(NULL)
, m_sph(NULL)
, m_vfx(NULL)
, m_frame(0.0f)
{
    m_collision_set = istNew(CollisionSet)();
    m_modules.push_back(m_collision_set);

    m_sph = istNew(SPHManager)();
    m_modules.push_back(m_sph);

    m_entity_set = istNew(EntitySet)();
    m_modules.push_back(m_entity_set);

    m_vfx = istNew(VFXSet)();
    m_modules.push_back(m_vfx);

    m_module_update_tasks.push_back( istNew(ModuleUpdateTask)(this) );
    m_module_update_tasks.push_back( istNew(ModuleUpdateTask)(m_collision_set) );
    m_module_update_tasks.push_back( istNew(ModuleUpdateTask)(m_entity_set) );
    m_module_update_tasks.push_back( istNew(ModuleUpdateTask)(m_sph) );

    const uvec2 &wsize = atomicGetWindowSize();
    m_camera.setAspect((float32)wsize.x/(float32)wsize.y);
}

World::~World()
{
    for(ModuleUpdateTaskCont::reverse_iterator i=m_module_update_tasks.rbegin(); i<m_module_update_tasks.rend(); ++i) {
        istDelete(*i);
    }
    for(ModuleCont::reverse_iterator i=m_modules.rbegin(); i!=m_modules.rend(); ++i) {
        istDelete(*i);
    }
}

void World::initialize()
{
    m_camera.setPosition(vec4(0.0f, 0.0f, 3.0f, 0.0f));
    m_camera.setZNear(0.01f);
    m_camera.setZFar(10.0f);
}


void World::frameBegin()
{
    m_collision_set->frameBegin();
    m_sph->frameBegin();
    m_entity_set->frameBegin();
}

void World::update(float32 dt)
{
    ++m_frame;

    if(m_frame==1) {
        m_entity_set->createEntity<Level_Test>();
    }

    for(ModuleCont::iterator i=m_modules.begin(); i!=m_modules.end(); ++i) {
        (*i)->update(dt);
    }

    m_camera.updateMatrix();
    m_frustum.constructFromViewProjectionMatrix(m_camera.getViewProjectionMatrix());
}

void World::asyncupdateBegin(float32 dt)
{
    for(uint32 i=0; i<m_module_update_tasks.size(); ++i) {
        m_module_update_tasks[i]->setArg(dt);
    }
    ist::EnqueueTasks(&m_module_update_tasks[0], m_module_update_tasks.size());
}

void World::asyncupdateEnd()
{
    ist::WaitTasks(&m_module_update_tasks[0], m_module_update_tasks.size());
}

void World::asyncupdate(float32 dt)
{
    //mat4 rot = glm::rotate(mat4(), 0.05f, vec3(0.0f, 1.0f, 0.0f));
    //m_camera.setPosition(rot * m_camera.getPosition());
    //m_camera.setAspect(atomicGetWindowAspectRatio());
}

void World::draw()
{
    for(ModuleCont::iterator i=m_modules.begin(); i!=m_modules.end(); ++i) {
        (*i)->draw();
    }
}

void World::frameEnd()
{
    for(ModuleCont::reverse_iterator i=m_modules.rbegin(); i!=m_modules.rend(); ++i) {
        (*i)->frameEnd();
    }
}


} // namespace atomic
