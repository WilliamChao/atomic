#ifndef __ist_Concurrency_Mutex_h__
#define __ist_Concurrency_Mutex_h__

#include "ist/Concurrency/Atomic.h"
#include "ist/Concurrency/ThreadCommon.h"

namespace ist {


template<class T>
class ScopedLock
{
public:
    ScopedLock(T &m) : m_mutex(m) { m_mutex.lock(); }

    template<class F>
    ScopedLock(T &m, const F &f) : m_mutex(m)
    {
        while(!m_mutex.tryLock()) { f(); }
    }

    ~ScopedLock() { m_mutex.unlock(); }

private:
    T &m_mutex;
};


/// CAS (compare and swap) �ɂ�� spin lock�B
/// atomic operation �� busy loop �ő҂��߁A�����Œ����ԑ҂ƃV�X�e���S�̂ɐ[���ȃp�t�H�[�}���X�̈��e�����y�ڂ��܂��B
/// �g���ꍇ�A�K�� light_mutex �Ɣ�r���Ė{���ɑ����Ȃ邩���؂��K�v�ł��B
/// ���� Windows �̏ꍇ priority boost ��������Ȃ����߂������̏ꍇ���ʂ� mutex ���x���Ȃ�܂��B
/// android �ŗL���ɋ@�\���邱�Ƃ����邩���؂��āA���߂Ȃ���������������Ǝv���܂��B
class istInterModule SpinMutex
{
public:
    typedef ScopedLock<SpinMutex> ScopedLock;

    void lock()
    {
        while(m_lockobj.compare_and_swap(1, 0) != 0) {}
    }

    bool tryLock()
    {
        return m_lockobj.compare_and_swap(1, 0) == 0;
    }

    void unlock()
    {
        m_lockobj.swap(0);
    }

private:
    atomic_int32 m_lockobj;
};


/// mutex
/// Windows �ł� CRITICAL_SECTION �ɂ������B
/// recursive �Ή�
class istInterModule Mutex
{
public:
    typedef ScopedLock<Mutex> ScopedLock;
#ifdef istWindows
    typedef CRITICAL_SECTION Handle;
#else
    typedef pthread_mutex_t Handle;
#endif // istWindows

    Mutex();
    ~Mutex();
    void lock();
    bool tryLock();
    void unlock();

    Handle getHandle() const { return m_lockobj; }

private:
    Handle m_lockobj;
};


} // namespace ist

#endif // __ist_Concurrency_Mutex_h__
