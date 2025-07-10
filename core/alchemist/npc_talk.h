#ifndef NPC_TALK_H
#define NPC_TALK_H

#include "el_list.h"
#include "generated/npc_talk_generated.hpp"

class Sentence : public ListElement
{
  public:
    const char * text;
    enum Npc_say id;

    Sentence( enum Npc_say i, const char * t)
    {
        text = t;
        id = i;        
    }
    void show(bool details = true)
    {
        printf("%s\n", text);
    }
    bool check(void * what)
    {
        enum Npc_say * i = (enum Npc_say *)what;
        return id == *i;
    }    
};

class SentencesList : public ElementsList
{
  public:
    SentencesList(const char * n) : ElementsList(n)
    {
    }
    Sentence * add(ListElement * el)
    {
        return (Sentence *)ElementsList::add(el);
    }
    void disable(enum Npc_say id);
    void enable(enum Npc_say id);
};

#endif // NPC_TALK_H
