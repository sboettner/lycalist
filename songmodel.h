#include <vector>
#include <glibmm.h>


class Scale {
public:
    int get_display_offset_for_note(int note) const;
    int get_note_for_display_offset(int offset) const;

    int has_note(int note) const
    {
        return !(get_display_offset_for_note(note)&1);
    }

    Glib::ustring get_note_name(int note) const;
};


class Chord {
public:
    enum class Quality {
        Major,
        Minor,
        Major6,
        Minor6,
        Major7,
        Minor7,
        Dominant7,
        Sus2,
        Sus4,
        Diminished
    };

    Chord();

    auto begin() const
    {
        return notes.begin();
    }

    auto end() const
    {
        return notes.end();
    }

    bool has_note(int note) const
    {
        return notes.find(uint8_t(note%12))!=notes.npos;
    }

    int get_root() const
    {
        return root;
    }

    void set_root(int);

    Quality get_quality() const
    {
        return quality;
    }

    void set_quality(Quality);

    Glib::ustring get_name(const Scale&) const;

private:
    int                         root=0;
    Quality                     quality=Quality::Major;

    std::basic_string<uint8_t>  notes;

    void update();
};


struct Syllable {
	Glib::ustring	text;
    Chord           chord;
    int             note=12;
};


struct TimePoint {
    int         time;
    Syllable*   syllable;

    TimePoint(int time, Syllable* syl):time(time), syllable(syl) {}

    ~TimePoint()
    {
        delete syllable;
    }

    TimePoint(const TimePoint&) = delete;

    TimePoint(TimePoint&& other)
    {
        time=other.time;
        syllable=other.syllable;
        other.syllable=nullptr;
    }

    TimePoint& operator=(const TimePoint&) = delete;

    TimePoint& operator=(TimePoint&& other)
    {
        time=other.time;
        syllable=other.syllable;
        other.syllable=nullptr;
        return *this;
    }
};


class Song {
public:
    Song();

    const Scale& get_scale() const
    {
        return scale;
    }

    TimePoint& operator[](int i)
    {
        return syllables[i];
    }

    int length() const
    {
        return syllables.size();
    }

    void insert(int pos, TimePoint&& tp);
    void remove(int pos);

    int find(const Syllable*) const;

    int find_index_before_time(int time) const;

private:
    Scale                   scale;
    std::vector<TimePoint>  syllables;
};
