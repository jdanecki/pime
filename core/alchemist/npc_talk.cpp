#include "npc_talk.h"
#include "el_list.h" // IWYU pragma: export
#include "generated/sentences.cpp"
#include "generated/questions.cpp"
#include "generated/answers.cpp"

void SentencesList::disable(Npc_say id)
{
    Sentence * s = static_cast<Sentence *>(find(&id));
    s->disable();
}

void SentencesList::enable(Npc_say id)
{
    Sentence * s = static_cast<Sentence *>(find(&id));
    s->enable();
}
Sentence::Sentence(enum Npc_say i, const char * t)
{
    text = t;
    id = i;
}
void Sentence::show(bool details)
{
    CONSOLE_LOG("%s\n", text);
}
bool Sentence::check(void * what)
{
    enum Npc_say * i = (enum Npc_say *)what;
    return id == *i;
}
SentencesList::SentencesList(const char * n) : ElementsList(n)
{
}
Sentence * SentencesList::add(ListElement * el)
{
    return (Sentence *)ElementsList::add(el);
}
