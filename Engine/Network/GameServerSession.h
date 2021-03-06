﻿#ifndef atm_Engine_Network_GameServerSession_h
#define atm_Engine_Network_GameServerSession_h
#include "externals.h"
#include "Protocol.h"

namespace atm {

#ifdef atm_enable_GameServer

class GameServerSessionFactory : public Poco::Net::TCPServerConnectionFactory
{
public:
    virtual Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket &ss);
};


class GameServerSession
    : public Poco::Net::TCPServerConnection
    , public PMessenger
{
typedef Poco::Net::TCPServerConnection super;
public:
    GameServerSession(const Poco::Net::StreamSocket &ss);
    virtual void run();
    void messageLoop();

    uint32 getFrame() const;
    uint32 getAveragePing() const;

private:
    void processReceivingMessage(PMessageCont &mes);

    uint32 m_pid;
    uint32 m_frame;
    uint32 m_ping;
};

#endif // atm_enable_GameServer

} // namespace atm
#endif // atm_Engine_Network_GameServerSession_h
