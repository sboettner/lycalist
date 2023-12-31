#include <gtkmm.h>
#include "songmodel.h"
#include "melodyedit.h"
#include "chordchooser.h"


MelodyEditor::MelodyEditor(Song& song):song(song)
{
    set_has_window(true);
	set_can_focus(true);

    add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
	add_events(Gdk::KEY_PRESS_MASK);
}


//Discover the total amount of minimum space and natural space needed by
//this widget.
//Let's make this simple example widget always need minimum 60 by 50 and
//natural 100 by 70.
void MelodyEditor::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
    minimum_width = 60;
    natural_width = 100;
}

void MelodyEditor::get_preferred_height_for_width_vfunc(int /* width */, int& minimum_height, int& natural_height) const
{
    minimum_height = 50;
    natural_height = 70;
}

void MelodyEditor::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
    minimum_height = 50;
    natural_height = 70;
}

void MelodyEditor::get_preferred_width_for_height_vfunc(int /* height */, int& minimum_width, int& natural_width) const
{
    minimum_width = 60;
    natural_width = 100;
}


void MelodyEditor::on_size_allocate(Gtk::Allocation& allocation)
{
    //Do something with the space that we have actually been given:
    //(We will not be given heights or widths less than we have requested, though
    //we might get more)

    //Use the offered allocation for this container:
    set_allocation(allocation);

    if(m_refGdkWindow)
    {
        m_refGdkWindow->move_resize( allocation.get_x(), allocation.get_y(),
            allocation.get_width(), allocation.get_height() );
    }
}


void MelodyEditor::on_realize()
{
    //Do not call base class Gtk::Widget::on_realize().
    //It's intended only for widgets that set_has_window(false).

    set_realized();

    if(!m_refGdkWindow) {
        //Create the GdkWindow:

        GdkWindowAttr attributes;
        memset(&attributes, 0, sizeof(attributes));

        Gtk::Allocation allocation = get_allocation();

        //Set initial position and size of the Gdk::Window:
        attributes.x = allocation.get_x();
        attributes.y = allocation.get_y();
        attributes.width = allocation.get_width();
        attributes.height = allocation.get_height();

        attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;

        m_refGdkWindow = Gdk::Window::create(get_parent_window(), &attributes, GDK_WA_X | GDK_WA_Y);
        set_window(m_refGdkWindow);

        //make the widget receive expose events
        m_refGdkWindow->set_user_data(gobj());
    }
}


bool MelodyEditor::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	const Gtk::Allocation allocation = get_allocation();
	auto refStyleContext = get_style_context();
    
    const Scale& scale=song.get_scale();

	// paint the background
	refStyleContext->render_background(cr,
	allocation.get_x(), allocation.get_y(),
	allocation.get_width(), allocation.get_height());

	cr->set_line_width(1.0);

    for (int i=0;i+1<song.length();i++) {
        if (!song[i].syllable) continue;

        Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("#282828"));

        cr->rectangle(song[i].time*scalex+1, 0, (song[i+1].time-song[i].time)*scalex-2, allocation.get_height());
        cr->fill();
    }

    for (int i=0;i<=32;i++) {
        Gdk::Cairo::set_source_rgba(cr, !(i%7) ? Gdk::RGBA("#c0c0c0") : Gdk::RGBA("#606060"));

        cr->move_to(0, 575.5-i*scaley);
        cr->line_to(allocation.get_width(), 575.5-i*scaley);
        cr->stroke();
    }


    for (int i=0;i<song.length();i++) {
        int x=song[i].time * scalex;

        Syllable* syl=song[i].syllable;
        if (!syl) continue;

        Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("#808080"));

        auto chord_layout=create_pango_layout(syl->chord.get_name(scale));
		cr->move_to(x + 4, 4);
		chord_layout->show_in_cairo_context(cr);

		auto text_layout=create_pango_layout(syl->text);
		cr->move_to(x + 4, 28);
		text_layout->show_in_cairo_context(cr);
       

        Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("#181818"));
        for (uint8_t note: syl->chord) {
            for (int noteoffs=scale.get_display_offset_for_note(note);noteoffs<64;noteoffs+=14) {
                cr->rectangle(x, 575.5-(noteoffs+2)*scaley/2, (song[i+1].time-song[i].time)*scalex, scaley);
                cr->fill();
            }
        }

        if (syl->note>=0) {
            const int noteoffs=scale.get_display_offset_for_note(syl->note);

            if (syl->chord.has_note(syl->note))
                Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("#20ff20"));
            else if (scale.has_note(syl->note))
                Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("#ffff20"));
            else
                Gdk::Cairo::set_source_rgba(cr, Gdk::RGBA("#ff2020"));

            cr->rectangle(x, 575.5-(noteoffs+2)*scaley/2, (song[i+1].time-song[i].time)*scalex, scaley);
            cr->fill();
        }
    }

    return true;
}


bool MelodyEditor::on_motion_notify_event(GdkEventMotion* event)
{
    int time=event->x/scalex;
    int index=song.find_index_before_time(time);

    int noteoffs;
    if (event->state & GDK_SHIFT_MASK)
        noteoffs=(576-int(event->y)-scaley/2)/scaley*2 + 1;
    else
        noteoffs=(576-int(event->y))/scaley*2;

    highlighted_syllable=index<1 ? nullptr : song[index-1].syllable;
    highlighted_note=song.get_scale().get_note_for_display_offset(noteoffs);

    return true;
}


bool MelodyEditor::on_button_press_event(GdkEventButton* event)
{
    if (!highlighted_syllable) return false;

    if (event->y<28) {
        int index=song.find(highlighted_syllable);

        auto* chordchooser=new ChordChooser(*this, song.get_scale(), highlighted_syllable->chord);
        chordchooser->set_pointing_to(
            Gdk::Rectangle(
                song[index].time*scalex,
                0,
                (song[index+1].time-song[index].time)*scalex,
                28
        ));
        chordchooser->show();

        return true;
    }

    highlighted_syllable->note=highlighted_note;
    queue_draw();

    return true;
}


bool MelodyEditor::on_key_press_event(GdkEventKey* event)
{
    return true;
}

