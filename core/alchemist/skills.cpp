#include <cstdio>
#include <cstring>
#include "skills_names.h"
#include "skills.h"

Skills * skills;

const char * gsn(Skill_type t)
{
    return skills_names[t];
}

Skill::Skill(Skill_type t)
{
    type = t;
    next = nullptr;
    experience = 1;
}

void Skill::show(bool details)
{
    if (details) printf("%s exp=%d\n", gsn(type), experience);
}

int Skill::use()
{
    printf("używam skill: %s\n", gsn(type));
    switch(type) {
        case SK_MAGIA: return use_magic();
        case SK_RZEMIOSLO: return use_crafting();
        case SK_WALKA: return use_fight();
        case SK_ZIELARSTWO: return use_herbalism();
        case SK_KAMIENIARSTWO: return use_masonry();
        case SK_WEDKARSTWO: return use_fishing();
        case SK_WIAZANIE: return use_catenation();
        case SK_HODOWLA: return use_farming();
        case SK_ZDUNSTWO: return use_stoving();
    default:
        experience++;
        break;
    }
    return 1;
} 

int Skill::use_catenation()
{
        experience++;
        return 2;
}

int Skill::use_stoving()
{
        experience++;
        return 20;
}

int Skill::use_farming()
{
        experience++;
        return 2;
}

int Skill::use_magic()
{
        experience++;
        return 100;
}

int Skill::use_crafting()
{
        experience++;
        return 1;
}

int Skill::use_fight()
{
        experience++;
        return 20;
}

int Skill::use_herbalism()
{
        experience++;
        return -1;
}

int Skill::use_masonry()
{
        experience++;
        return 10;
}

int Skill::use_fishing()
{
        experience++;
        return -1;
}

Skills::Skills(): ElementsList("skills")
{
}

Skill * Skills::find(Skill_type skill)
{
    return (Skill*) ElementsList::find(&skill);
}

void Skills::add(Skill_type t)
{
    Skill * s = new Skill(t);
    ElementsList::add(s);
}


void create_skills()
{
    skills = new Skills();
    skills->add(SK_RZEMIOSLO);
    skills->add(SK_WALKA);
    skills->add(SK_MAGIA);
    skills->add(SK_ZIELARSTWO);
    skills->add(SK_WIAZANIE);
    skills->add(SK_KAMIENIARSTWO);
    skills->add(SK_WEDKARSTWO);
    skills->add(SK_HODOWLA);

//    add("przetrwanie");
   // add("górnictwo");
    //add("pszczelarstwo");
   // add("rolnictwo");
    //add("kucharstwo");
    //add("kowalstwo");
//    add("kartografia");
//    add("drwal");
  //  add("tkactwo");
    //add("garncarstwo");
//    add("murarstwo");
  //  add("zdun");
//    add("papiernictwo");
//    add("młynarstwo");
//    add("piekarstwo");
}

void Skills::copy(ListElement *el)
{
    Skill * sk = (Skill*) el;
    add(sk->type);
}


