﻿#ifndef atm_Engine_Network_InputServer_h
#define atm_Engine_Network_InputServer_h

#include "Engine/Game/Input.h"
#include "Protocol.h"

namespace atm {

class IInputServer
{
public:
    enum IS_TypeID {
        IS_Local,
        IS_Replay,
        IS_Network,
    };

    virtual ~IInputServer() {}
    virtual IS_TypeID getTypeID() const=0;

    virtual void update()=0;
    virtual void draw()=0;
    virtual bool sync()=0;
    virtual void addPlayer(PlayerID pid, const PlayerName &name, uint32 equip)=0;
    virtual void erasePlayer(PlayerID pid)=0;
    virtual void pushInput(PlayerID pid, const RepInput &v)=0;
    virtual void pushLevelEditorCommand(const LevelEditorCommand &v)=0;
    virtual void handlePMessage(const PMessage &v)=0;
    virtual const InputState& getInput(PlayerID pid) const=0;

    virtual bool save(const char *path)=0;
    virtual bool load(const char *path)=0;
    virtual uint32 getPlayLength() const=0;
    virtual uint32 getPlayPosition() const=0;
};

IInputServer* CreateInputServerLocal();
IInputServer* CreateInputServerReplay();
IInputServer* CreateInputServerNetwork();

} // namespace atm
#endif // atm_Engine_Network_InputServer_h
