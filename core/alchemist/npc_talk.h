#ifndef NPC_TALK_H
#define NPC_TALK_H

#include "el_list.h"
#include "generated/npc_talk_generated.hpp" // IWYU pragma: keep

class Sentence : public ListElement
{
  public:
    const char * text;
    enum Npc_say id;

    Sentence(enum Npc_say i, const char * t);
    void show(bool details = true);
    bool check(void * what);
};

class SentencesList : public ElementsList
{
  public:
    SentencesList(const char * n);
    Sentence * add(ListElement * el);
    void disable(enum Npc_say id);
    void enable(enum Npc_say id);
};

#endif // NPC_TALK_H
