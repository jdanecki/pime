#ifndef __SKILL_H__
#define __SKILL_H__


#include "skills_type.h"

class Skill
{
public:
    int experience;
    Skill();
    void use();
};

const char * gsn(Skill_type t);

#endif

