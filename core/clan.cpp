#include <stdlib.h>

#include "clan.h"
#include "alchemist/object.h"


const char * clan_names[] = {"Animal", "Human", "Dwarf", "Elf", "Niziolek"};

Clan::Clan(ClanId id) : id(id)
{
}

Human_clan::Human_clan() : Clan(Clan_Human)
{
    skills[SK_KAMIENIARSTWO].experience = 3;
    skills[SK_MAGIA].experience = 3;
    skills[SK_RZEMIOSLO].experience = 3;
    skills[SK_WALKA].experience = 3;
    skills[SK_WEDKARSTWO].experience = 3;
    skills[SK_WIAZANIE].experience = 3;
    skills[SK_ZIELARSTWO].experience = 3;
    skills[SK_HODOWLA].experience = 3;
    skills[SK_ZDUNSTWO].experience = 3;
}

Dwarf_clan::Dwarf_clan() : Clan(Clan_Dwarf)
{
    skills[SK_KAMIENIARSTWO].experience = 5;
    skills[SK_MAGIA].experience = 1;
    skills[SK_RZEMIOSLO].experience = 5;
    skills[SK_WALKA].experience = 5;
    skills[SK_WEDKARSTWO].experience = 1;
    skills[SK_WIAZANIE].experience = 1;
    skills[SK_ZIELARSTWO].experience = 1;
    skills[SK_HODOWLA].experience = 1;
    skills[SK_ZDUNSTWO].experience = 1;
}

Elf_clan::Elf_clan() : Clan(Clan_Elf)
{
    skills[SK_KAMIENIARSTWO].experience = 1;
    skills[SK_MAGIA].experience = 5;
    skills[SK_RZEMIOSLO].experience = 1;
    skills[SK_WALKA].experience = 5;
    skills[SK_WEDKARSTWO].experience = 1;
    skills[SK_WIAZANIE].experience = 1;
    skills[SK_ZIELARSTWO].experience = 5;
    skills[SK_HODOWLA].experience = 5;
    skills[SK_ZDUNSTWO].experience = 1;
}

Niziolek_clan::Niziolek_clan() : Clan(Clan_Niziolek)
{
    skills[SK_KAMIENIARSTWO].experience = 1;
    skills[SK_MAGIA].experience = 1;
    skills[SK_RZEMIOSLO].experience = 1;
    skills[SK_WALKA].experience = 1;
    skills[SK_WEDKARSTWO].experience = 5;
    skills[SK_WIAZANIE].experience = 5;
    skills[SK_ZIELARSTWO].experience = 5;
    skills[SK_HODOWLA].experience = 1;
    skills[SK_ZDUNSTWO].experience = 1;
}

Human_clan HumanClan;
Dwarf_clan DwarfClan;
Elf_clan ElfClan;
Niziolek_clan NiziolekClan;

Clan * get_clan_by_id(ClanId id)
{
    switch ((int)id)
    {
        case 1:
            return &HumanClan;
        case 2:
            return &DwarfClan;
        case 3:
            return &ElfClan;
        case 4:
            return &NiziolekClan;
    }
    return nullptr;
}
