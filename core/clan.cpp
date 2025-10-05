#include "clan.h"
#include "alchemist/object.h"
#include <stdlib.h>

const char * clan_names[] = {"Animal", "Human", "Dwarf", "Elf", "Niziolek"};

Clan::Clan(ClanId id):  NetworkObject(Class_Clan), id(id)
{    
    skills = new Skills();
}

Human_clan::Human_clan() : Clan(Clan_Human)
{    
    skills->add(SK_RZEMIOSLO);
    skills->add(SK_WALKA);
    skills->add(SK_ZIELARSTWO);
    skills->add(SK_WIAZANIE);
    skills->add(SK_KAMIENIARSTWO);
    skills->add(SK_WEDKARSTWO);
    skills->add(SK_HODOWLA);
    skills->add(SK_ZDUNSTWO);
}

Dwarf_clan::Dwarf_clan() : Clan(Clan_Dwarf)
{
    skills->add(SK_RZEMIOSLO);
    skills->add(SK_WALKA);
    skills->add(SK_KAMIENIARSTWO);
}

Elf_clan::Elf_clan() : Clan(Clan_Elf)
{

    skills->add(SK_WALKA);
    skills->add(SK_MAGIA);
    skills->add(SK_ZIELARSTWO);
}

Niziolek_clan::Niziolek_clan() : Clan(Clan_Niziolek)
{    
    skills->add(SK_RZEMIOSLO);
    skills->add(SK_ZIELARSTWO);
    skills->add(SK_WIAZANIE);
    skills->add(SK_WEDKARSTWO);
    skills->add(SK_HODOWLA);
    skills->add(SK_ZDUNSTWO);
}

Clan * get_clan_by_id(ClanId id)
{
    switch ((int) id)
    {
        case 0: return new Human_clan();
        case 1: return new Dwarf_clan();
        case 2: return new Elf_clan();
        case 3: return new Niziolek_clan();
    }
    return nullptr;
}
Clan *get_random_clan()
{
    int c = rand() % 4;
    return get_clan_by_id((ClanId) c);
}

