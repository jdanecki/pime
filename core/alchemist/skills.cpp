#include <cstdio>
#include <cstring>
#include "skills_names.h"
#include "skills.h"
int CONSOLE_LOG(const char * fmt, ...);

const char * gsn(Skill_type t)
{
    return skills_names[t];
}

Skill::Skill()
{
    experience = 0;
}

int Skill::use()
{
    CONSOLE_LOG("używam skill: %s\n", gsn(type));
    return 1;
}
