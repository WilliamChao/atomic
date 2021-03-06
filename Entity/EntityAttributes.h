﻿#ifndef atm_Entity_EntityAttributes_h
#define atm_Entity_EntityAttributes_h

#include "Util.h"
#include "Engine/Game/CollisionModule.h"
#include "Engine/Game/FluidModule.h"
#include "Engine/Graphics/Renderer.h"
#include "psym/psym.h"

namespace atm {
typedef psym::Particle FluidMessage;
//struct FluidMessage
//{
//    const psym::Particle *fluids;
//    size_t num_fluids;
//};


class Attr_Null
{
    istSerializeBlock()
public:
    atmECallBlock()
    wdmScope( void addDebugNodes(const wdmString &path) {} )

public:
    Attr_Null() {}
    void update(float32 dt) {}
    void asyncupdate(float32 dt) {}
};

class Attr_PastTime
{
private:
    float32 m_time;

    istSerializeBlock(
        istSerialize(m_time)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(getPastTime)
            atmECall(setPastTime)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_time", &m_time);
    }
    )

public:
    Attr_PastTime() : m_time() {}
    float32 getPastTime() const     { return m_time; }
    void    setPastTime(float32 v)  { m_time=v; }
    void update(float32 dt) { m_time+=dt; }
    void asyncupdate(float32 dt) {}
};

class Attr_RefCount
{
private:
    uint32 m_refcount;

    istSerializeBlock(
        istSerialize(m_refcount)
    )
protected:
    void setRefCount(uint32 v) { m_refcount=v; }

public:
    atmECallBlock(
        atmMethodBlock(
        atmECall(setRefCount)
        atmECall(incRefCount)
        atmECall(decRefCount)
        atmECall(getRefCount)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_refcount", (const uint32*)&m_refcount);
    }
    )

    Attr_RefCount() : m_refcount(0) {}
    uint32 getRefCount() const  { return m_refcount; }
    uint32 incRefCount()        { return ++m_refcount; }
    uint32 decRefCount()        { return --m_refcount; }
    void update(float32 dt)     {}
    void asyncupdate(float32 dt){}
};


class Attr_ParticleSet
{
private:
    vec4 m_diffuse_color;
    vec4 m_glow_color;
    PSET_RID m_psetid;

    istSerializeBlock(
        istSerialize(m_diffuse_color)
        istSerialize(m_glow_color)
        istSerialize(m_psetid)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(getDiffuseColor)
            atmECall(setDiffuseColor)
            atmECall(getGlowColor)
            atmECall(setGlowColor)
            atmECall(getModel)
            atmECall(setModel)
        )
    )

    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
		wdmAddNode(path + "/m_diffuse_color", &m_diffuse_color, wdmMakeRange(0.0f, 1.0f));
        wdmAddNode(path+"/m_glow_color", &m_glow_color, wdmMakeRange(0.0f, 1.0f));
    }
    )

public:
    Attr_ParticleSet() : m_psetid(PSET_CUBE_SMALL)
    {}

    void setDiffuseColor(const vec4 &v) { m_diffuse_color=v; }
    void setGlowColor(const vec4 &v)    { m_glow_color=v; }
    void setModel(PSET_RID v)           { m_psetid=v; }
    const vec4& getDiffuseColor() const { return m_diffuse_color; }
    const vec4& getGlowColor() const    { return m_glow_color; }
    PSET_RID getModel() const           { return m_psetid; }

    void drawModel(const mat4 &trans)
    {
        PSetInstance inst;
        inst.diffuse = getDiffuseColor();
        inst.glow = getGlowColor();
        inst.flash = vec4();
        inst.elapsed = 0.0f;
        inst.appear_radius = 10000.0f;
        inst.transform = trans;
        atmGetFluidPass()->addParticles(getModel(), inst);
    }

    void update(float32 dt)     {}
    void asyncupdate(float32 dt){}
};


class Attr_Collision
{
private:
    CollisionHandle m_collision;
    EntityHandle m_owner_handle;

    istSerializeBlock(
        istSerialize(m_collision)
        istSerialize(m_owner_handle)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(getCollisionFlags)
            atmECall(setCollisionFlags)
            atmECall(getCollisionGroup)
            atmECall(setCollisionGroup)
            atmECall(getCollisionHandle)
            atmECall(setCollisionShape)
        )
    )
    wdmScope(void addDebugNodes(const wdmString &path) {})

public:
    Attr_Collision() : m_collision(0), m_owner_handle(0)
    {
    }

    ~Attr_Collision()
    {
        finalizeCollision();
    }

    void setCollisionFlags(int32 v)
    {
        if(CollisionEntity *ce=atmGetCollision(m_collision)) {
            ce->setFlags(v);
        }
    }

    uint32 getCollisionFlags() const
    {
        if(CollisionEntity *ce=atmGetCollision(m_collision)) {
            return ce->getFlags();
        }
        return 0;
    }

    void setCollisionGroup(CollisionGroup v)
    {
        if(CollisionEntity *ce=atmGetCollision(m_collision)) {
            ce->setCollisionGroup(v);
        }
    }

    CollisionGroup getCollisionGroup() const
    {
        if(CollisionEntity *ce=atmGetCollision(m_collision)) {
            return ce->getCollisionGroup();
        }
        return 0;
    }

    void initializeCollision(EntityHandle h)
    {
        m_owner_handle = h;
    }

    void finalizeCollision()
    {
        if(m_collision!=0) {
            atmDeleteCollision(m_collision);
            m_collision = 0;
        }
    }

    void setCollisionShape(CollisionShapeType cs)
    {
        finalizeCollision();
        if(cs==CS_Null) {
            m_collision = 0;
            return;
        }
        CollisionEntity *ce = nullptr;
        switch(cs) {
        case CS_Box:    ce = atmCreateCollision(CollisionBox);   break;
        case CS_Sphere: ce = atmCreateCollision(CollisionSphere);break;
        default: istAssert(false); return;
        }
        ce->setEntityHandle(m_owner_handle);
        m_collision = ce->getCollisionHandle();
    }

    CollisionHandle getCollisionHandle() const { return m_collision; }
    CollisionSphere& getCollisionSphere() {
        CollisionEntity *ce = atmGetCollision(m_collision);
        istAssert(ce!=nullptr && ce->getShapeType()==CS_Sphere);
        return *static_cast<CollisionSphere*>(ce);
    }
    CollisionBox& getCollisionBox() {
        CollisionEntity *ce = atmGetCollision(m_collision);
        istAssert(ce!=nullptr && ce->getShapeType()==CS_Box);
        return *static_cast<CollisionBox*>(ce);
    }

    void updateCollision(const mat4 &t)
    {
        if(CollisionEntity *ce = atmGetCollision(m_collision)) {
            switch(ce->getShapeType()) {
            case CS_Sphere:
                {
                    CollisionSphere &shape = *static_cast<CollisionSphere*>(ce);
                    vec3 pos = vec3(t * vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    UpdateCollisionSphere(shape, pos, shape.pos_r.w);
                }
                break;
            case CS_Box:
                {
                    CollisionBox &shape = *static_cast<CollisionBox*>(ce);
                    UpdateCollisionBox(shape, t, vec3(shape.size));
                }
                break;
            }
        }
    }

    void updateCollisionAsSphere(const mat4 &t, float32 radius)
    {
        if(CollisionEntity *ce = atmGetCollision(m_collision)) {
            switch(ce->getShapeType()) {
            case CS_Sphere:
                {
                    vec3 pos = vec3(t * vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    UpdateCollisionSphere(*static_cast<CollisionSphere*>(ce), pos, radius);
                }
                break;
            }
        }
    }

    void updateCollisionByParticleSet(PSET_RID psid, const mat4 &t, const vec3 &scale=vec3(1.0f))
    {
        if(CollisionEntity *ce = atmGetCollision(m_collision)) {
            switch(ce->getShapeType()) {
            case CS_Sphere:
                {
                    vec3 pos = vec3(t * vec4(0.0f, 0.0f, 0.0f, 1.0f));
                    float radius = atmGetRigidInfo(psid)->sphere_radius * scale.x;
                    UpdateCollisionSphere(*static_cast<CollisionSphere*>(ce), pos, radius);
                }
                break;
            case CS_Box:
                {
                    vec3 box_size = (vec3&)atmGetRigidInfo(psid)->box_size * scale;
                    UpdateCollisionBox(*static_cast<CollisionBox*>(ce), t, box_size);
                }
                break;
            }
        }
    }

    void update(float32 dt)     {}
    void asyncupdate(float32 dt){}
};



struct CollideMessage;
struct DamageMessage;
struct DestroyMessage;
struct KillMessage;

class Attr_MessageHandler
{

    istSerializeBlock()

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(eventCollide)
            atmECall(eventFluid)
            atmECall(eventDamage)
            atmECall(eventDestroy)
            atmECall(eventKill)
        )
    )
    wdmScope(void addDebugNodes(const wdmString &path) {})

    virtual void eventCollide(const CollideMessage *m)  {}
    virtual void eventFluid(const FluidMessage *m)      {}
    virtual void eventDamage(const DamageMessage *m)    {}
    virtual void eventDestroy(const DestroyMessage *m)  {}
    virtual void eventKill(const KillMessage *m)        {}
    void update(float32 dt)     {}
    void asyncupdate(float32 dt){}
};


// 流体を浴びた時血痕を残すエフェクトを実現する
class Attr_Bloodstain
{
private:
    // 血痕を残す頻度。流体がこの回数衝突するたびに一つ血痕を残す。
    static const uint32 bloodstain_frequency = 256;

    ist::raw_vector<BloodstainParticle> m_bloodstain;
    uint32 m_bloodstain_hitcount;

    istSerializeBlock(
        istSerialize(m_bloodstain)
        istSerialize(m_bloodstain_hitcount)
    )

public:
    atmECallBlock(
        atmMethodBlock(
            atmECall(addBloodstain)
        )
    )
    wdmScope(void addDebugNodes(const wdmString &path) {})

public:
    Attr_Bloodstain() : m_bloodstain_hitcount(0)
    {
        m_bloodstain.reserve(256);
    }

    void addBloodstain(const mat4 &imat, const vec4& pos)
    {
        if(!atmGetConfig()->show_bloodstain) { return; }
        if(m_bloodstain.size()==m_bloodstain.capacity()) { return; }

        if(++m_bloodstain_hitcount % bloodstain_frequency == 0) {
            BloodstainParticle tmp;
            tmp.position = imat * pos;
            tmp.lifetime = 1.0f;
            m_bloodstain.push_back(tmp);
        }
    }

    void updateBloodstain(float32 dt)
    {
        uint32 n = m_bloodstain.size();
        for(uint32 i=0; i<n; ++i) {
            BloodstainParticle &bsp = m_bloodstain[i];
            bsp.lifetime -= 0.002f*dt;
        }
        erase(m_bloodstain, [](BloodstainParticle &bsp){ return bsp.lifetime <= 0.0f; });
    }

    uint32 getNumBloodstainParticles() const { return m_bloodstain.size(); }
    const BloodstainParticle* getBloodStainParticles() const { return m_bloodstain.empty() ? nullptr : &m_bloodstain[0]; }

    void update(float32 dt)     {}
    void asyncupdate(float32 dt){ updateBloodstain(dt); }
};


class Attr_Spin
{
private:
    vec3 m_spin_center;
    float32 m_spin_angle;
    float32 m_spin_min_angle;
    float32 m_spin_max_angle;
    float32 m_spin_speed;
    float32 m_spin_return_speed;
    float32 m_spin_max_speed;
    float32 m_spin_resist;
    float32 m_spin_decel;
    float32 m_spin_oneway;

    istSerializeBlock(
        istSerialize(m_spin_center)
        istSerialize(m_spin_angle)
        istSerialize(m_spin_min_angle)
        istSerialize(m_spin_max_angle)
        istSerialize(m_spin_speed)
        istSerialize(m_spin_return_speed)
        istSerialize(m_spin_max_speed)
        istSerialize(m_spin_resist)
        istSerialize(m_spin_decel)
        istSerialize(m_spin_oneway)
    )

public:
    wdmScope(
    void addDebugNodes(const wdmString &path)
    {
        wdmAddNode(path+"/m_spin_angle", &m_spin_angle);
        wdmAddNode(path+"/m_spin_speed", &m_spin_speed);
        wdmAddNode(path+"/m_spin_return_speed", &m_spin_return_speed);
        wdmAddNode(path+"/m_spin_max_speed", &m_spin_max_speed);
        wdmAddNode(path+"/m_spin_decel", &m_spin_decel);
        wdmAddNode(path+"/m_spin_accel", &m_spin_resist);
        wdmAddNode(path+"/m_spin_oneway", &m_spin_oneway);
    }
    )
    atmECallBlock(
        atmMethodBlock(
            atmECall(pulse)
            atmECall(addSpinSpeed)
            atmECall(getSpinAngle)
            atmECall(setSpinAngle)
            atmECall(getSpinMinAngle)
            atmECall(setSpinMinAngle)
            atmECall(getSpinMaxAngle)
            atmECall(setSpinMaxAngle)
            atmECall(getSpinSpeed)
            atmECall(setSpinSpeed)
            atmECall(getSpinReturnSpeed)
            atmECall(setSpinReturnSpeed)
            atmECall(getSpinMaxSpeed)
            atmECall(setSpinMaxSpeed)
            atmECall(getSpinResist)
            atmECall(setSpinResist)
            atmECall(getSpinDecel)
            atmECall(setSpinDecel)
            atmECall(getSpinOneWay)
            atmECall(setSpinOneWay)
        )
    )
    atmJsonizeBlock(
    )

public:
    Attr_Spin()
        : m_spin_angle(0.0f), m_spin_min_angle(-9999.0f), m_spin_max_angle(9999.0f)
        , m_spin_speed(0.0f), m_spin_return_speed(0.0f), m_spin_max_speed(0.5f)
        , m_spin_resist(0.00002f), m_spin_decel(0.99f)
        , m_spin_oneway(0.0f)
    {
    }

    void updateSpin(float32 dt, const vec3 &center)
    {
        m_spin_center = center;
        float32 speed = getSpinSpeed();
        float32 angle = getSpinAngle();
        setSpinAngle(angle + speed*dt);
        speed += (glm::sign(-angle)*getSpinReturnSpeed()) * dt;
        speed *= getSpinDecel();
        setSpinSpeed(speed);
    }

    void pulse(const vec3 &pos, const vec3 &force)
    {
        vec3 diff    = pos-m_spin_center; diff.z=0.0f;
        float32 dist = glm::length(diff);
        vec3 dir     = vec3( glm::rotateZ(diff/dist, 90.0f) );
        float32 lf   = glm::length(force);
        vec3 nf      = force/lf;
        float32 d    = glm::dot(dir, nf);
        float32 f    = lf * m_spin_resist * dist * d;
        if(m_spin_oneway==0.0f || glm::sign(m_spin_oneway)==glm::sign(f)) {
            addSpinSpeed(f);
        }
    }

    void addSpinSpeed(float32 v)        { setSpinSpeed(getSpinSpeed()+v); }
    float32 getSpinAngle() const        { return m_spin_angle; }
    float32 getSpinMinAngle() const     { return m_spin_min_angle; }
    float32 getSpinMaxAngle() const     { return m_spin_max_angle; }
    float32 getSpinSpeed() const        { return m_spin_speed; }
    float32 getSpinReturnSpeed() const  { return m_spin_return_speed; }
    float32 getSpinMaxSpeed() const     { return m_spin_max_speed; }
    float32 getSpinResist() const       { return m_spin_resist; }
    float32 getSpinDecel() const        { return m_spin_decel; }
    float32 getSpinOneWay() const       { return m_spin_oneway; }
    void setSpinAngle(float32 v) {
        if(v>=m_spin_max_angle) {
            m_spin_angle = m_spin_max_angle;
            m_spin_speed = glm::min(m_spin_speed, 0.0f);
        }
        else if(v<=m_spin_min_angle) {
            m_spin_angle = m_spin_min_angle;
            m_spin_speed = glm::max(m_spin_speed, 0.0f);
        }
        else {
            m_spin_angle = v;
        }
    }
    void setSpinMinAngle(float32 v)     { m_spin_min_angle=v; }
    void setSpinMaxAngle(float32 v)     { m_spin_max_angle=v; }
    void setSpinSpeed(float32 v)        { m_spin_speed=glm::sign(v)*glm::min(glm::abs(v), m_spin_max_speed); }
    void setSpinReturnSpeed(float32 v)  { m_spin_return_speed=v; }
    void setSpinMaxSpeed(float32 v)     { m_spin_max_speed=v; }
    void setSpinResist(float32 v)       { m_spin_resist=v; }
    void setSpinDecel(float32 v)        { m_spin_decel=v; }
    void setSpinOneWay(float32 v)       { m_spin_oneway=v; }
};

} // namespace atm
#endif // atm_Entity_EntityAttributes_h
