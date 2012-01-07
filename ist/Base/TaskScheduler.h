#ifndef __ist_TaskScheduler__
#define __ist_TaskScheduler__

#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x0500
  #define WINVER 0x0500
#endif

#include <EASTL/vector.h>
#include <EASTL/list.h>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/interprocess/detail/atomic.hpp>


namespace ist {

// �X���b�h�ɖ��O��ݒ肵�܂��B�f�o�b�K���环�ʂ��₷���Ȃ�܂��B 
// ��������̑��X���b�h�ɑ΂��Ă͎��s���邱�Ƃ�����A�X���b�h���g�������ɂ���̂��]�܂����ł��B
void SetThreadName(uint32_t thread_id, const char *name);
void SetThreadName(const char *name);


class SpinMutex
{
public:
    class Lock
    {
    private:
        SpinMutex &m_mutex;

    public:
        Lock(SpinMutex &m) : m_mutex(m) { m_mutex.lock(); }
        ~Lock() { m_mutex.unlock(); }
    };

private:
    volatile LONG m_lock;

public:
    SpinMutex() : m_lock(0) { }

    void lock()
    {
        while(::InterlockedCompareExchange(&m_lock, 1, 0) != 0) {
            ::Sleep(0);
        }
    }

    void unlock()
    {
        ::InterlockedExchange(&m_lock, 0);
    }
};


class Task;
typedef Task* TaskPtr;


namespace impl
{
    class TaskThread;
    class TaskQueue;
} // namespace impl



class Task
{
friend class TaskScheduler;
friend class impl::TaskQueue;
friend class impl::TaskThread;
private:
    bool m_finished;
    int m_priority;

private:
    void beforeExec();
    void afterExec();

public:
    Task();
    virtual ~Task();    // �f�X�g���N�^�� join() �͂��Ȃ��̂Œ��ӁB

    bool isFinished() const { return m_finished; }
    int getPriority() const { return m_priority; }
    void setPriority(int v) { m_priority=v; }

    void kick();
    void join();

    virtual void exec()=0;
};



class TaskScheduler
{
private:
    typedef boost::shared_ptr<impl::TaskThread> thread_ptr;
    typedef stl::vector<thread_ptr> thread_cont;
    typedef boost::scoped_ptr<impl::TaskQueue> queue_ptr;

    static TaskScheduler *s_instance;
    queue_ptr m_task_queue;
    thread_cont m_threads;

private:
    TaskScheduler();
    void initialize(size_t num_thread);
    void finalize();

public:
    ~TaskScheduler();

    /// num_thread: �X���b�h���B0 �̏ꍇ CPU �̐��Ɏ��������B 
    static void initializeInstance(size_t num_thread=0);
    /// ���ݏ������̃^�X�N�̊�����҂��Ĕj���B(�^�X�N�L���[����ɂȂ�̂�҂��Ȃ�) 
    static void finalizeInstance();
    static TaskScheduler* getInstance();

    /// ���s�҂��^�X�N������΂������������B�Ȃ���΋U��Ԃ��B 
    static bool wait();
    /// �w��^�X�N�̊�����҂B���s�҂��^�X�N������ꍇ�A�Ăяo�����X���b�h�͑҂��Ă�ԃ^�X�N�����ɉ����B 
    /// �^�X�N�Ԃő��݂ɑ҂悤�ȃV�`���G�[�V����������Ɖi�v��~����\��������̂Œ��ӁB 
    static void waitFor(TaskPtr task);
    static void waitFor(TaskPtr tasks[], size_t num);
    /// �w��^�X�N�̊�����҂BwaitFor() �ƈႢ�A�Ăяo�����X���b�h�̓^�X�N�����ɉ����Ȃ��B
    static void waitExclusive(TaskPtr task);
    static void waitExclusive(TaskPtr tasks[], size_t num);
    /// �S�^�X�N�̊�����҂B���s�҂��^�X�N������ꍇ�A�Ăяo�����X���b�h�͑҂��Ă�ԃ^�X�N�����ɉ����B 
    /// �^�X�N������ĂԂƉi�v��~����̂ł������_���B 
    static void waitForAll();

    /// �^�X�N�̃X�P�W���[�����O���s���B 
    static void addTask(TaskPtr task);
    static void addTask(TaskPtr tasks[], size_t num);

    static size_t getThreadCount();
    static boost::thread::id getThreadId(size_t i);


    /// ���������p
    impl::TaskQueue* getTaskQueue();
};




// �ȉ��A���[�e�B���e�B�n


// �q/�`�F�C�������^�X�N
class ChainedTask : public Task
{
protected:
    stl::vector<TaskPtr> m_children;
    stl::vector<TaskPtr> m_chain;

public:
    void clear()
    {
        m_children.clear();
        m_chain.clear();
    }

    void appendChild(TaskPtr p) { m_children.push_back(p); }
    void appendChain(TaskPtr p) { m_chain.push_back(p); }

    void exec()
    {
        TaskScheduler& scheduler = *TaskScheduler::getInstance();
        scheduler.addTask(&m_children[0], m_children.size());
        scheduler.waitFor(&m_children[0], m_children.size());
        scheduler.addTask(&m_chain[0], m_chain.size());
    }
};

template<class Functor>
class FunctionalTask : public Task
{
private:
    Functor m_func;

public:
    FunctionalTask(const Functor& v) : m_func(v) {}

    void exec()
    {
        m_func();
    }
};


} // namespace ist
#endif // __ist_TaskScheduler__ 
