#include "songmodel.h"


Song::Song()
{
    syllables.emplace_back(12, new Syllable);
    syllables.emplace_back(24, new Syllable);
    syllables.emplace_back(30, nullptr);
    syllables.emplace_back(36, new Syllable);
    syllables.emplace_back(60, nullptr);

    syllables[0].syllable->text="One";
    syllables[1].syllable->text="Two";
    syllables[3].syllable->text="Three";
}


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


int Song::find_index_before_time(int time) const
{
    int index=0;

    while (index<syllables.size() && syllables[index].time<=time) index++;

    return index;
}
