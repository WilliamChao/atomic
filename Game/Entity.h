#ifndef __atomic_Game_Entity__
#define __atomic_Game_Entity__

#include "EntityClass.h"

#define atomicImplementEntity(class_name, category_id, class_id)\
class class_name;                                               \
template<> struct EntityTraits<class_name>                      \
{                                                               \
    enum {                                                      \
    CATEGORY_ID = category_id,                                  \
    CLASS_ID = class_id,                                        \
    };                                                          \
};                                                              \
template<> IEntity* EntitySet::createEntity<class_name>()    \
{                                                               \
    class_name *t = istNew(class_name)();                      \
    typedef EntityTraits<class_name> traits;                    \
    addEntity(traits::CATEGORY_ID, traits::CLASS_ID, t);        \
    return t;                                                   \
}



namespace atomic {


class IEntity;
template<class T> struct EntityTraits;




class IEntity
{
friend class EntitySet;
private:
    EntityHandle m_ehandle;

    bool isValidHandle(EntityHandle h);
    void setHandle(uint32 h) { m_ehandle=h; }

public:
    IST_INTROSPECTION_INTERFACE(IEntity);

    // �R���X�g���N�^�ł̓����o�ϐ��������ȊO�̏������s�Ȃ��Ă͂Ȃ�Ȃ��B���� initialize() �ōs���B
    // (ID ���R���X�g���N�^�̌�Ɍ��܂邽�߁A�q�I�u�W�F�N�g�̏������Ȃǂ�K�؂ɍs���ɂ͂�������K�v������)
    IEntity() : m_ehandle(0) {}
    virtual ~IEntity() {}
    uint32 getHandle() const { return m_ehandle; }

    // ����������
    virtual void initialize() {}

    // �I�������B
    // EntitySet ����J�����ꂽ�^�C�~���O�ŌĂ΂��B
    // �Q�ƃJ�E���^���������ꍇ�Ȃǂ́AEntitySet ���������� (finalize() ���Ă΂�) �Ă� delete �͂���Ȃ��P�[�X�����肤�邽�߁A
    // �f�X�g���N�^�Ǝg��������K�v���o�Ă���B
    virtual void finalize() {}

    // �����X�V
    virtual void update(float32 dt) {}

    // �񓯊��X�V�B
    // Entity �Ԃ̍X�V�͕���ɍs���邪�A���̊ԁA�Փ˔����`��Ȃǂ̑��̃��W���[���̍X�V�͍s���Ȃ��B
    // (������ Entity �̍X�V���S�ďI����Ă���s����)
    // �ʒu�Ȃǂ̍X�V�� 1 �t���[���x�点�đ����W���[���Ƃ�����ɍX�V�������������A���ꂾ�Ƃǂ����Ă��Փ˂̉����Ԃ����s���R�ɂȂ邽�߁A�����Ȃ����B
    virtual void asyncupdate(float32 dt) {}

    // �`��f�[�^�� Renderer �ɓn���B
    // (�n�������B���̒��� i3d::Device �Ȃǂ̕`�� API �𒼐ڌĂ�ł͂Ȃ�Ȃ�)
    virtual void draw() {}


    // call_id �ɑΉ����郁�\�b�h������ v �ŌĂԁB (��� setHoge() �n)
    // Routine ��O���X�N���v�g�Ƃ̘A���p�B
    virtual bool call(uint32 call_id, const variant &v) { return false; }

    // query_id �ɑΉ����郁�\�b�h���Ă�� v �Ɍ��ʂ��i�[����B(��� getHoge() �n)
    // Routine ��O���X�N���v�g�Ƃ̘A���p�B
    virtual bool query(uint32 query_id, variant &v) const { return false; }
};



class EntitySet : public IAtomicGameModule
{
public:
    typedef stl::vector<EntityHandle> HandleCont;
    typedef stl::vector<IEntity*> EntityCont;
    typedef stl::vector<Task*> TaskCont;

private:
    HandleCont m_vacant[ECID_END][ESID_MAX];
    EntityCont m_entities[ECID_END][ESID_MAX];
    EntityCont m_new_entities;
    HandleCont m_all;
    TaskCont m_tasks;

    void addEntity(uint32 categoryid, uint32 classid, IEntity *e);
    void resizeTasks(uint32 n);

public:
    EntitySet();
    ~EntitySet();

    void frameBegin();
    void update(float32 dt);

    // �Ȃɂ����Ȃ��B
    // �s�{�ӂȂ��� Entity �̔񓯊��X�V�� update() ���ōs���B (IEntity ���Q��)
    void asyncupdate(float32 dt);

    void draw();
    void frameEnd();

    IEntity* getEntity(EntityHandle h);
    void deleteEntity(EntityHandle h);
    template<class T> IEntity* createEntity();
};



} // namespace atomic
#endif // __atomic_Game_Entity__
