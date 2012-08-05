#ifndef __ist_Concurrency_Condition_h__
#define __ist_Concurrency_Condition_h__

#include "ist/Concurrency/ThreadCommon.h"
#ifndef istWindows
#include "ist/Concurrency/Mutex.h"
#endif // istWindows

namespace ist {

class istInterModule Condition
{
public:
#ifdef istWindows
    typedef HANDLE Handle;
#else
    typedef pthread_cond_t Handle;
#endif // istWindows

    Condition();
    ~Condition();
    void wait();
    /// �N���҂��Ă��Ȃ���Ԃ� signalOne() �����ꍇ�Asignal ��Ԃ��p�����܂��B(Windows �� Event ����)
    /// * �� Windows �̏ꍇ�A2 �̃X���b�h�� signal ���`����Ă��܂��\��������܂��B
    ///   �⊶�ł����A1 �̃X���b�h�����N���Ȃ��Ƃ����O��̃R�[�h�͏����Ȃ��ł��������B
    void signalOne();
    /// ������͒N���҂��Ă��Ȃ��Ă� signal ��Ԃ͌p�����܂���
    void signalAll();

    Handle& getHandle() { return m_lockobj; }

private:
#ifdef istWindows
    Handle m_lockobj;
#else
    Handle m_lockobj;
    Mutex m_mutex;
    atomic_int32 m_signal;
#endif // istWindows
};

} // namespace ist

#endif // __ist_Concurrency_Condition_h__
