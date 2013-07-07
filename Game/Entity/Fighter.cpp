﻿#include "stdafx.h"
#include "Game/Entity/EntityCommon.h"
#include "Game/Entity/Enemy.h"
#include "Game/Entity/Routine.h"

namespace atm {

    /*
    EC_BreakableParts,
    EC_BreakableCore,
    EC_SmallFighter,
    EC_MediumFighter,
    EC_LargeFighter,
    EC_Shell,
    EC_Tortoise,
    EC_Zab,
    */

class dpPatch BreakableParts : public Breakable<Entity_Orientation>
{
typedef Breakable<Entity_Orientation> super;
private:
    istSerializeBlock(
        istSerializeBase(super)
    )

public:
};
atmImplementEntity(BreakableParts);
atmExportClass(BreakableParts);


class dpPatch BreakableCore : public Breakable<Entity_Orientation>
{
typedef Breakable<Entity_Orientation> super;
private:
    istSerializeBlock(
        istSerializeBase(super)
    )

public:
};
atmImplementEntity(BreakableCore);
atmExportClass(BreakableCore);


class dpPatch SmallFighter : public Breakable<Entity_Orientation>
{
typedef Breakable<Entity_Orientation> super;
private:
    istSerializeBlock(
        istSerializeBase(super)
    )

public:
};
atmImplementEntity(SmallFighter);
atmExportClass(SmallFighter);


class dpPatch MediumFighter : public Breakable<Entity_Orientation>
{
typedef Breakable<Entity_Orientation> super;
private:
    istSerializeBlock(
        istSerializeBase(super)
    )

public:
};
atmImplementEntity(MediumFighter);
atmExportClass(MediumFighter);


class dpPatch LargeFighter : public Breakable<Entity_Orientation>
{
typedef Breakable<Entity_Orientation> super;
private:
    istSerializeBlock(
        istSerializeBase(super)
    )

public:
};
atmImplementEntity(LargeFighter);
atmExportClass(LargeFighter);


class dpPatch Shell : public Breakable<Entity_Orientation>
{
typedef Breakable<Entity_Orientation> super;
private:
    istSerializeBlock(
        istSerializeBase(super)
    )

public:
};
atmImplementEntity(Shell);
atmExportClass(Shell);


class dpPatch Zab : public Breakable<Entity_Orientation>
{
typedef Breakable<Entity_Orientation> super;
private:
    istSerializeBlock(
        istSerializeBase(super)
    )

public:
};
atmImplementEntity(Zab);
atmExportClass(Zab);


class dpPatch Nucleus : public Breakable<Entity_Orientation>
{
typedef Breakable<Entity_Orientation> super;
private:
    istSerializeBlock(
        istSerializeBase(super)
    )

public:
};
atmImplementEntity(Nucleus);
atmExportClass(Nucleus);



} // namespace atm