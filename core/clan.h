#ifndef CLAN_H
#define CLAN_H


#include "alchemist/skills.h"

enum ClanId
{
    Clan_Animal = 0,
    Clan_Human,
    Clan_Dwarf,
    Clan_Elf,
    Clan_Niziolek
};

extern const char * clan_names[];

class Clan
{
public:
    ClanId id;
    Clan(ClanId id);
    Skills *skills;
};

class Human_clan: public Clan
{
public:
    Human_clan();
};

class Dwarf_clan: public Clan
{
public:
    Dwarf_clan();
};

class Elf_clan: public Clan
{
public:
    Elf_clan();
};

class Niziolek_clan: public Clan
{
public:
    Niziolek_clan();
};

Clan * get_random_clan();
Clan * get_clan_by_id(ClanId id);
#endif // CLAN_H
