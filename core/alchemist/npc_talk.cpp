#include "npc_talk.h"

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
