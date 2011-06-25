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

namespace ist
{

class Task;
typedef Task* TaskPtr;


namespace impl
{
    class TaskThread;
    class TaskQueue;
} // namespace impl



// todo: �D�揇��
class Task
{
friend class TaskScheduler;
friend class impl::TaskQueue;
friend class impl::TaskThread;
private:
    bool m_finished;

private:
    void beforeExec();
    void afterExec();

public:
    Task();
    virtual ~Task();
    bool isFinished() const;

    virtual void exec()=0;
};




namespace impl
{

    class TaskQueue;
    class TaskThread;


} // namespace impl



class TaskScheduler
{
private:
    typedef boost::shared_ptr<impl::TaskThread> thread_ptr;
    typedef eastl::vector<thread_ptr> thread_cont;
    typedef boost::scoped_ptr<impl::TaskQueue> queue_ptr;

    static TaskScheduler *s_instance;
    queue_ptr m_task_queue;
    thread_cont m_threads;

private:
    TaskScheduler();
    ~TaskScheduler();
    void initialize(size_t num_thread);

public:
    /// num_thread: �X���b�h���B0 �̏ꍇ CPU �̐��Ɏ��������B 
    static void initializeSingleton(size_t num_thread=0);
    /// ���ݏ������̃^�X�N�̊�����҂��Ĕj���B(�^�X�N�L���[����ɂȂ�̂�҂��Ȃ�) 
    static void finalizeSingleton();
    static TaskScheduler* getInstance();


    /// �S�^�X�N�̊�����҂B�^�X�N�L���[����ł͂Ȃ��ꍇ�A�Ăяo�����X���b�h���^�X�N�����ɉ����B 
    /// �^�X�N������ĂԂƉi�v��~����̂ł������_���B 
    void waitForAll();
    /// �w��^�X�N�̊�����҂B�^�X�N�L���[����ł͂Ȃ��ꍇ�A�Ăяo�����X���b�h���^�X�N�����ɉ����B 
    void waitFor(TaskPtr task);
    /// �͈͎w��o�[�W���� 
    void waitFor(TaskPtr tasks[], size_t num);


    /// �^�X�N�̃X�P�W���[�����O���s���B 
    void schedule(TaskPtr task);
    // �͈͎w��o�[�W���� 
    void schedule(TaskPtr tasks[], size_t num);


    size_t getThreadCount() const;
    boost::thread::id getThreadId(size_t i) const;


    /// ���������p
    impl::TaskQueue* getTaskQueue();
};




// �ȉ��A���[�e�B���e�B�n


// �q/�`�F�C�������^�X�N
class ChainedTask : public Task
{
protected:
    eastl::vector<TaskPtr> m_children;
    eastl::vector<TaskPtr> m_chain;

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
        scheduler.schedule(&m_children[0], m_children.size());
        scheduler.waitFor(&m_children[0], m_children.size());
        scheduler.schedule(&m_chain[0], m_chain.size());
    }
};


} // namespace ist
#endif // __ist_TaskScheduler__ 
