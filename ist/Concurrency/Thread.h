#ifndef __ist_Concurrency_Thread_h__
#define __ist_Concurrency_Thread_h__

#include "ist/Base/SharedObject.h"
#include "ist/Concurrency/ThreadCommon.h"

namespace ist {

class istInterModule Thread : public SharedObject
{
public:
    typedef void(*EntryPoint)(void*);
#ifdef istWindows
    typedef HANDLE Handle;
#else // istWindows
    typedef pthread_t Handle;
#endif // istWindows
    enum Priority {
#ifdef istWindows
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
    virtual ~Thread(); /// �f�X�g���N�^�� join() �͂��Ȃ��̂Œ���

    Handle& getHandle() { return m_handle; }
    /// �ȉ��� set �n�֐��� run() �̑O�ɌĂ΂Ȃ��Ɣ��f����Ȃ��̂Œ��ӁB
    /// (pthread_t ���� thread id �𓾂�|�[�^�u���ȕ��@���Ȃ��̂ŁA�ΏۃX���b�h�������ŕς��邵���Ȃ�)
    void setName(const char *v)     { strncpy(m_name, v, _countof(m_name)); }
    void setAffinityMask(size_t v)  { m_affinity=v; }
    void setPriority(int v)         { m_priority=v; }
    void setStaskSize(size_t v)     { m_stacksize=v; }

    /// run() �ŃX���b�h�𐶐��A�������ꂽ�X���b�h���� exec() ���Ă΂��B
    void run();
    void join();

    /// �V�K�쐬���ꂽ�X���b�h����Ă΂��B�p����ŏ������e������
    virtual void exec()=0;

    void setParams();

private:
    Handle  m_handle;
    size_t  m_stacksize;
    char    m_name[64];
    size_t  m_affinity;
    int     m_priority;
};

} // namepspace ist

#endif // __ist_Concurrency_Thread_h__
