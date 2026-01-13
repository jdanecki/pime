#ifndef __SKILL_H__
#define __SKILL_H__

#include "skills_type.h"

class Skill
{
  public:
    Skill_type type;
    int experience;
    Skill();
    int use();
};

const char * gsn(Skill_type t);

#endif
