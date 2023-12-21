#include <gtkmm.h>
#include "songmodel.h"
#include "melodyedit.h"


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

	// paint the background
	refStyleContext->render_background(cr,
	allocation.get_x(), allocation.get_y(),
	allocation.get_width(), allocation.get_height());

    return true;
}


bool MelodyEditor::on_motion_notify_event(GdkEventMotion* event)
{
    return true;
}


bool MelodyEditor::on_button_press_event(GdkEventButton* event)
{
    return true;
}


bool MelodyEditor::on_key_press_event(GdkEventKey* event)
{
    return true;
}

