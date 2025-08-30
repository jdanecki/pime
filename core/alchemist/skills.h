#ifndef __SKILL_H__
#define __SKILL_H__


#include "skills_type.h"
#include "el_list.h"

class Skill :public ListElement
{
    int use_magic();
    int use_crafting();
    int use_fight();
    int use_herbalism();
    int use_masonry();
    int use_fishing();
    int use_catenation();
    int use_farming();
    int use_stoving();

public:
    int experience;
    Skill_type type;
    Skill(Skill_type t);
    int use();
    void show(bool details=true);
    bool check(void * what)
    {
        Skill_type * sk = (Skill_type *)what;
        if (type == *sk) return true;
        return false;
    }
};

const char * gsn(Skill_type t);

class Skills: public ElementsList
{
 public:
    Skills();
    Skill * find(Skill_type sk);
    void add(Skill_type sk);    
    void virtual copy(ListElement *el);
};

extern Skills * skills;
void create_skills();
#endif

