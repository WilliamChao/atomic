﻿#ifndef __ist_Concurrency_Condition_h__
#define __ist_Concurrency_Condition_h__

#include "ist/Concurrency/ThreadCommon.h"
#ifndef ist_env_Windows
#include "ist/Concurrency/Mutex.h"
#endif // ist_env_Windows

namespace ist {

class istInterModule Condition
{
public:
#ifdef ist_env_Windows
    typedef HANDLE Handle;
#else
    typedef pthread_cond_t Handle;
#endif // ist_env_Windows

    Condition();
    ~Condition();
    void wait();
    void timedWait(uint32 millisec);
    /// 誰も待っていない状態で signalOne() した場合、signal 状態が継続します。(Windows の Event 方式)
    /// * 非 Windows の場合、2 つのスレッドに signal が伝わってしまう可能性があります。
    ///   遺憾ですが、1 つのスレッドしか起きないという前提のコードは書かないでください。
    void signalOne();
    /// こちらは誰も待っていなくても signal 状態は継続しません
    void signalAll();

    Handle& getHandle() { return m_lockobj; }

private:
#ifdef ist_env_Windows
    Handle m_lockobj;
#else
    Handle m_lockobj;
    Mutex m_mutex;
    atomic_int32 m_signal;
#endif // ist_env_Windows
};

} // namespace ist

#endif // __ist_Concurrency_Condition_h__
