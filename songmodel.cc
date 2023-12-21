#include "songmodel.h"


void Song::insert(int pos, TimePoint&& tp)
{
    syllables.insert(syllables.begin()+pos, std::move(tp));
}


void Song::remove(int pos)
{
    syllables.erase(syllables.begin()+pos);
}


int Song::find(const Syllable* syl) const
{
    if (!syl) return -1;

    int index=0;
    while (index<syllables.size() && syllables[index].syllable!=syl)
        index++;
    
    return index==syllables.size() ? -1 : index;
}
