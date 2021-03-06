﻿#include "istPCH.h"
#include "../Base.h"
#include "../Sound.h"

namespace ist {
namespace isd {


Device::Device()
    : m_device(NULL)
    , m_context(NULL)
{
    m_device = alcOpenDevice(NULL);
    if(!m_device) {
        istPrint("alcOpenDevice() failed");
        return;
    }

    m_context = alcCreateContext(m_device, NULL);
    if(!m_context) {
        alcCloseDevice(m_device); m_device=NULL;
        istPrint("alcCreateContext() failed");
        return;
    }

    alcMakeContextCurrent(m_context);
}

Device::~Device()
{
    for(uint32 i=0; i<m_resources.size(); ++i) {
        istSafeRelease(m_resources[i]);
    }
    m_resources.clear();

    alcMakeContextCurrent(0);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

void Device::addResource( DeviceResource *v )
{
    if(!v) { return; }

    if(!m_vacant.empty()) {
        ResourceHandle drh = m_vacant.back();
        m_vacant.pop_back();
        m_resources[drh] = v;
        v->setDeviceResourceHandle(drh);
    }
    else {
        v->setDeviceResourceHandle(m_resources.size());
        m_resources.push_back(v);
    }
}

void Device::deleteResource( ResourceHandle v )
{
    istSafeDelete(m_resources[v]);
    m_vacant.push_back(v);
}

Buffer* Device::createBuffer()
{
    Buffer *r = istNew(Buffer)(this);
    addResource(r);
    return r;
}

Source* Device::createSource()
{
    Source *r = istNew(Source)(this);
    addResource(r);
    return r;
}

Listener* Device::createListener()
{
    Listener *r = istNew(Listener)(this);
    addResource(r);
    return r;
}

Device* CreateDevice()
{
    return istNew(Device)();
}

} // namespace isd
} // namespace ist
