#include "stdafx.h"
#include "../Sound.h"

namespace ist {
namespace sound {

int Listener::getI(ALenum param) const
{
    int v = 0;
    alGetListeneri(param, &v);
    return v;
}

float Listener::getF(ALenum param) const
{
    float r;
    alGetListenerf(param, &r);
    return r;
}

float4 Listener::get3F(ALenum param) const
{
    float4 r;
    alGetListenerfv(param, (ALfloat*)&r);
    return r;
}

void Listener::setI(ALenum param, int v)
{
    alListeneri(param, v);
}

void Listener::setF(ALenum param, float v)
{
    alListenerf(param, v);
}

void Listener::set3F(ALenum param, const float4& v)
{
    alListenerfv(param, (ALfloat*)&v);
}


Listener::Listener(){}
Listener::~Listener() {}

float Listener::getGain() const { return getF(AL_GAIN); }
float4 Listener::getPosition() const { return get3F(AL_POSITION); }
float4 Listener::getVelocity() const { return get3F(AL_VELOCITY); }

void Listener::setGain(float v) { setF(AL_GAIN, v); }
void Listener::setPosition(const float4& v) { set3F(AL_POSITION, v); }
void Listener::setVelocity(const float4& v) { set3F(AL_VELOCITY, v); }


} // namespace sound
} // namespace ist