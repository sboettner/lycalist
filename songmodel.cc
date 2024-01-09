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


Glib::ustring Scale::get_note_name(int note) const
{
    static const char* notenames[]={ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    return Glib::ustring(notenames[note]);
}


Chord::Chord()
{
    root=0;
    update();
}


Glib::ustring Chord::get_name(const Scale& scale) const
{
    const static char* qualitysuffix[]={ "", "m", "6", "m6", "maj7", "min7", "7", "sus2", "sus4", "dim" };

    Glib::ustring name=scale.get_note_name(root);
    name.append(qualitysuffix[int(quality)]);
    return name;
}


void Chord::set_root(int r)
{
    root=r;
    update();
}


void Chord::set_quality(Quality q)
{
    quality=q;
    update();
}


void Chord::update()
{
    const static int8_t qualitynotes[10][4]={
        { 0, 4, 7, -1 },    // major
        { 0, 3, 7, -1 },    // minor
        { 0, 4, 7, 9 },     // major6
        { 0, 3, 7, 9 },     // minor6
        { 0, 4, 7, 11 },    // major7
        { 0, 3, 7, 10 },    // minor7
        { 0, 4, 7, 10 },    // dominant7
        { 0, 2, 7, -1 },    // sus2
        { 0, 5, 7, -1 },    // sus4
        { 0, 3, 6, -1 }     // diminished
    };

    notes.clear();
    for (int8_t i: qualitynotes[int(quality)]) {
        if (i<0) break;
        notes.append(1, (uint8_t) ((root+i)%12));
    }
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
