﻿#ifndef atomic_FunctionID_h
#define atomic_FunctionID_h

namespace atomic {

enum FunctionID
{
    FID_kill,
    FID_destroy,
    FID_setRefCount,
    FID_addRefCount,
    FID_release,
    FID_eventCollide,
    FID_eventFluid,
    FID_eventDamage,
    FID_eventDestroy,
    FID_eventKill,
    FID_damage,
    FID_setDiffuseColor,
    FID_setGlowColor,
    FID_setModel,
    FID_setCollisionShape,
    FID_setCollisionFlags,
    FID_setHealth,
    FID_setRoutine,
    FID_setOwner,
    FID_setVelocity,
    FID_setAccel,
    FID_setPower,
    FID_setPosition,
    FID_setScale,
    FID_setAxis,
    FID_setAxis1 = FID_setAxis,
    FID_setAxis2,
    FID_setRotate,
    FID_setRotate1 = FID_setRotate,
    FID_setRotate2,
    FID_setRotateSpeed,
    FID_setRotateSpeed1 = FID_setRotateSpeed,
    FID_setRotateSpeed2,
    FID_setDirection,
    FID_setSpeed,
    FID_setLightRadius,
    FID_setExplosionSE,
    FID_setExplosionChannel,

    FID_getRefCount,
    FID_getDiffuseColor,
    FID_getGlowColor,
    FID_getModel,
    FID_getCollisionHandle,
    FID_getCollisionFlags,
    FID_getHealth,
    FID_getOwner,
    FID_getVelocity,
    FID_getPower,
    FID_getPosition,
    FID_getScale,
    FID_getAxis,
    FID_getAxis1 = FID_getAxis,
    FID_getAxis2,
    FID_getRotate,
    FID_getRotate1 = FID_getRotate,
    FID_getRotate2,
    FID_getDirection,
    FID_getSpeed,
    FID_getRotateSpeed,
    FID_getRotateSpeed1 = FID_setRotateSpeed,
    FID_getRotateSpeed2,

    FID_End,
};

} // namespace atomic

#endif atomic_FunctionID_h
