#include <cstdio>
#include <cstring>
#include "skills_names.h"
#include "skills.h"

const char * gsn(Skill_type t)
{
    return skills_names[t];
}

Skill::Skill()
{
    experience = 0;
}

void Skill::use()
{
    experience++;
} 

// void create_skills()
// {
//     skills = new Skills();
//     skills->add(SK_RZEMIOSLO);
//     skills->add(SK_WALKA);
//     skills->add(SK_MAGIA);
//     skills->add(SK_ZIELARSTWO);
//     skills->add(SK_WIAZANIE);
//     skills->add(SK_KAMIENIARSTWO);
//     skills->add(SK_WEDKARSTWO);
//     skills->add(SK_HODOWLA);

// //    add("przetrwanie");
//    // add("górnictwo");
//     //add("pszczelarstwo");
//    // add("rolnictwo");
//     //add("kucharstwo");
//     //add("kowalstwo");
// //    add("kartografia");
// //    add("drwal");
//   //  add("tkactwo");
//     //add("garncarstwo");
// //    add("murarstwo");
//   //  add("zdun");
// //    add("papiernictwo");
// //    add("młynarstwo");
// //    add("piekarstwo");
// }


