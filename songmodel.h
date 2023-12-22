#include <vector>
#include <glibmm.h>

struct Syllable {
	Glib::ustring	text;
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
    std::vector<TimePoint>  syllables;
};
