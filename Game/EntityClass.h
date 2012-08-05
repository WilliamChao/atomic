#ifndef __atomic_Game_EntityClass__
#define __atomic_Game_EntityClass__

namespace atomic {
    enum ENTITY_CATEGORY_ID
    {
        ECID_Unknown,
        ECID_Player,
        ECID_Enemy,
        ECID_Obstruct,
        ECID_Bullet,
        ECID_Level,
        ECID_VFX,

        ECID_End,
    };

    enum ENTITY_PLAYER_CLASS_ID
    {
        ESID_Player,
        ESID_Player_End,
    };

    enum ENTITY_ENEMY_CLASS_ID
    {
        ESID_Enemy_Test,
        ESID_Enemy_End,
    };

    enum ENTITY_OBSTACLE_CLASS_ID
    {
        ESID_Obstacle,
        ESID_Obstacle_End,
    };

    enum ENTITY_BULLET_CLASS_ID
    {
        ESID_Bullet_Simple,
        ESID_Bullet_Particle,
        ESID_Bullet_Laser,
        ESID_Bullet_End,
    };

    enum ENTITY_LEVEL_CLASS_ID
    {
        ESID_Level_Test,
        ESID_Level_End,
    };

    enum ENTITY_VFX_CLASS_ID
    {
        ESID_VFX_Scintilla,
        ESID_VFX_End,
    };

    enum {
        ESID_MAX = 16
    };
    BOOST_STATIC_ASSERT(ESID_MAX >= ESID_Player_End);
    BOOST_STATIC_ASSERT(ESID_MAX >= ESID_Enemy_End);
    BOOST_STATIC_ASSERT(ESID_MAX >= ESID_Obstacle_End);
    BOOST_STATIC_ASSERT(ESID_MAX >= ESID_Bullet_End);
    BOOST_STATIC_ASSERT(ESID_MAX >= ESID_VFX_End);

    // EntityHandle: ��� 4 bit ���J�e�S�� (ENTITY_CATEGORY_ID)�A���̎� 8 bit ���J�e�S������� (ENTITY_*_CLASS_ID)�A����ȉ��� ID �̃t�B�[���h
    inline uint32 EntityGetCategory(EntityHandle e) { return (e & 0xF0000000) >> 28; }
    inline uint32 EntityGetClass(EntityHandle e)    { return (e & 0x0FF00000) >> 20; }
    inline uint32 EntityGetID(EntityHandle e)       { return (e & 0x000FFFFF) >>  0; }
    inline uint32 EntityCreateHandle(uint32 cid, uint32 sid, uint32 id) { return (cid<<28) | (sid<<20) | id; }


    class Player;

    class Enemy_Test;

    class Bullet_Simple;

    class Level_Test;

} // namespace atomic
#endif //__atomic_Game_EntityClass__
