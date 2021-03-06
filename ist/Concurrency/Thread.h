﻿#ifndef ist_Concurrency_Thread_h
#define ist_Concurrency_Thread_h

#include "ist/Base/NonCopyable.h"
#include "ist/Base/SharedObject.h"
#include "ist/Concurrency/ThreadCommon.h"

namespace ist {

class Thread
{
istNonCopyable(Thread);
public:
    typedef void(*EntryPoint)(void*);
#ifdef ist_env_Windows
    typedef HANDLE Handle;
#else // ist_env_Windows
    typedef pthread_t Handle;
#endif // ist_env_Windows
    enum Priority {
#ifdef ist_env_Windows
        Priority_Low    = THREAD_PRIORITY_LOWEST,
        Priority_Normal = THREAD_PRIORITY_NORMAL,
        Priority_High   = THREAD_PRIORITY_HIGHEST,
#else
        Priority_Low,
        Priority_Normal,
        Priority_High,
#endif
    };

    static size_t getLogicalCpuCount();
    static Handle getCurrentThread();
    static void setNameToCurrentThread(const char *name);
    static void setAffinityMaskToCurrentThread(size_t mask);
    static void setPriorityToCurrentThread(int priority);

public:
    Thread();
    virtual ~Thread(); /// デストラクタで join() はしないので注意

    Handle& getHandle() { return m_handle; }
    /// 以下の set 系関数は run() の前に呼ばないと反映されないので注意。
    /// (pthread_t から thread id を得るポータブルな方法がないので、対象スレッドが自分で変えるしかない)
    void setName(const char *v)     { strncpy(m_name, v, _countof(m_name)); }
    void setLocale(const char *v)   { strncpy(m_locale, v, _countof(m_locale)); }
    void setAffinityMask(size_t v)  { m_affinity=v; }
    void setPriority(int v)         { m_priority=v; }
    void setStaskSize(size_t v)     { m_stacksize=v; }

    /// run() でスレッドを生成、生成されたスレッドから exec() が呼ばれる。
    void run();
    void join();
    void kill();

    /// 新規作成されたスレッドから呼ばれる。継承先で処理内容を実装
    virtual void exec()=0;

    void setParams();

private:
    Handle  m_handle;
    size_t  m_stacksize;
    char    m_name[64];
    char    m_locale[64];
    size_t  m_affinity;
    int     m_priority;
};


template<class FuncT=std::function<void ()> >
class FunctorThread : public Thread
{
typedef Thread Super;
public:
    FunctorThread() {}
    FunctorThread(const FuncT &f) { run(f); }

    void run(const FuncT &f)
    {
        m_func = f;
        Super::run();
    }

    void exec()
    {
        m_func();
    }

private:
    FuncT m_func;
};

} // namepspace ist

#endif // ist_Concurrency_Thread_h
