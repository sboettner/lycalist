#include "songmodel.h"


int Scale::get_display_offset_for_note(int note) const
{
    static const int offs[]={ 0,1,2,3,4, 6,7,8,9,10,11,12 };
    return (note/12)*14 + offs[note%12];
}


int Scale::get_note_for_display_offset(int offset) const
{
    int octave=offset/14;
    offset%=14;

    static const int offs[]={ 0,1,2,3,4, 6,7,8,9,10,11,12 };
    int note=0;
    while (note<11 && offs[note+1]<=offset) note++;

    return octave*12 + note;
}


Chord::Chord()
{
    root=0;

    // major chord, tentatively
    notes.append(1, (uint8_t) 0);
    notes.append(1, (uint8_t) 4);
    notes.append(1, (uint8_t) 7);
}


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
