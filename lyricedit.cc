#include <gtkmm.h>
#include "songmodel.h"
#include "lyricedit.h"


LyricEditor::LyricEditor()
{
    set_has_window(true);
	set_can_focus(true);

    add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
	add_events(Gdk::KEY_PRESS_MASK);

	im_context=gtk_im_context_simple_new();
	g_signal_connect(im_context, "commit", G_CALLBACK(commit_input), this);
}


//Discover the total amount of minimum space and natural space needed by
//this widget.
//Let's make this simple example widget always need minimum 60 by 50 and
//natural 100 by 70.
void LyricEditor::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
    minimum_width = 60;
    natural_width = 100;
}

void LyricEditor::get_preferred_height_for_width_vfunc(int /* width */, int& minimum_height, int& natural_height) const
{
    minimum_height = 50;
    natural_height = 70;
}

void LyricEditor::get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
    minimum_height = 50;
    natural_height = 70;
}

void LyricEditor::get_preferred_width_for_height_vfunc(int /* height */, int& minimum_width, int& natural_width) const
{
    minimum_width = 60;
    natural_width = 100;
}


void LyricEditor::on_size_allocate(Gtk::Allocation& allocation)
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


void LyricEditor::on_realize()
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


void LyricEditor::draw_syllable_box_closed(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h)
{
    cr->move_to(x+4.5, y+0.5);
    cr->line_to(x+w-4.5, y+0.5);
    cr->curve_to(x+w-2.5, y+0.5, x+w-0.5, y+2.5, x+w-0.5, y+4.5);
    cr->line_to(x+w-0.5, y+h-4.5);
    cr->curve_to(x+w-0.5, y+h-2.5, x+w-2.5, y+h-0.5, x+w-4.5, y+h-0.5);
    cr->line_to(x+4.5, y+h-0.5);
    cr->curve_to(x+2.5, y+h-0.5, x+0.5, y+h-2.5, x+0.5, y+h-4.5);
    cr->line_to(x+0.5, y+4.5);
    cr->curve_to(x+0.5, y+2.5, x+2.5, y+0.5, x+4.5, y+0.5);
}


void LyricEditor::draw_syllable_box_open_right(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h)
{
    cr->move_to(x+w-0.5, y+h-0.5);
    cr->line_to(x+4.5, y+h-0.5);
    cr->curve_to(x+2.5, y+h-0.5, x+0.5, y+h-2.5, x+0.5, y+h-4.5);
    cr->line_to(x+0.5, y+4.5);
    cr->curve_to(x+0.5, y+2.5, x+2.5, y+0.5, x+4.5, y+0.5);
    cr->line_to(x+w-0.5, y+0.5);
}


void LyricEditor::draw_syllable_box_open_left(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h)
{
    cr->move_to(x+0.5, y+0.5);
    cr->line_to(x+w-4.5, y+0.5);
    cr->curve_to(x+w-2.5, y+0.5, x+w-0.5, y+2.5, x+w-0.5, y+4.5);
    cr->line_to(x+w-0.5, y+h-4.5);
    cr->curve_to(x+w-0.5, y+h-2.5, x+w-2.5, y+h-0.5, x+w-4.5, y+h-0.5);
    cr->line_to(x+0.5, y+h-0.5);
}


bool LyricEditor::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	const Gtk::Allocation allocation = get_allocation();
	const double scale_x = 1.0;// (double)allocation.get_width();
	const double scale_y = 1.0;//(double)allocation.get_height();
	auto refStyleContext = get_style_context();

	// paint the background
	refStyleContext->render_background(cr,
	allocation.get_x(), allocation.get_y(),
	allocation.get_width(), allocation.get_height());

	// draw the foreground
	const auto state = refStyleContext->get_state();
	Gdk::Cairo::set_source_rgba(cr, refStyleContext->get_color(Gtk::STATE_FLAG_INSENSITIVE));

	cr->set_line_width(1.0);

    for (int i=0;i<=beatsperline;i++) {
        cr->move_to(borderx + i*beatsubdivisions*scalex, 32);
        cr->line_to(bordery + i*beatsubdivisions*scalex, 288);
    }

    cr->stroke();


    const int ncolumns=beatsperline*beatsubdivisions;

    for (int i=0;i+1<song.length();i++) {
        Syllable* syl=song[i].syllable;
        if (!syl) continue;

        int column=song[i].time % ncolumns;
        int row   =song[i].time / ncolumns;

        int columnend=song[i+1].time % ncolumns;
        int rowend   =song[i+1].time / ncolumns;

        bool highlight=selected_syllable ? syl==selected_syllable : highlighted_syllable ? syl==highlighted_syllable : false;

        if (row<rowend) {
            while (row<rowend) {
                // FIXME: after the first row, we need a box open on both ends
                draw_syllable_box_open_right(cr, borderx+column*scalex+1, bordery+row*scaley+1, (ncolumns-column)*scalex-2, scaley-2);

                Gdk::Cairo::set_source_rgba(cr, highlight ? Gdk::RGBA("#00aa00") : Gdk::RGBA("#005500"));
                cr->fill_preserve();

                Gdk::Cairo::set_source_rgba(cr, highlight ? Gdk::RGBA("#aaffaa") : Gdk::RGBA("#00ff00"));
                cr->stroke();

                row++;
                column=0;
            }

            draw_syllable_box_open_left(cr, borderx+column*scalex+1, bordery+row*scaley+1, (columnend-column)*scalex-2, scaley-2);

            Gdk::Cairo::set_source_rgba(cr, highlight ? Gdk::RGBA("#00aa00") : Gdk::RGBA("#005500"));
            cr->fill_preserve();

            Gdk::Cairo::set_source_rgba(cr, highlight ? Gdk::RGBA("#aaffaa") : Gdk::RGBA("#00ff00"));
            cr->stroke();
        }
        else {
            draw_syllable_box_closed(cr, borderx+column*scalex+1, bordery+row*scaley+1, (columnend-column)*scalex-2, scaley-2);

            Gdk::Cairo::set_source_rgba(cr, highlight ? Gdk::RGBA("#00aa00") : Gdk::RGBA("#005500"));
            cr->fill_preserve();

            Gdk::Cairo::set_source_rgba(cr, highlight ? Gdk::RGBA("#aaffaa") : Gdk::RGBA("#00ff00"));
            cr->stroke();
        }

		auto layout=create_pango_layout(syl->text);
		
		int text_width, text_height;
		layout->get_pixel_size(text_width, text_height);

		cr->move_to(borderx+column*scalex + 2.5, bordery+row*scaley + (scaley-text_height)*0.5);
		layout->show_in_cairo_context(cr);

        if (syl==selected_syllable) {
            // FIXME: can we get the byte index more easily?
            const gchar* text=syl->text.data();
            const gchar* cursor=g_utf8_offset_to_pointer(text, cursorpos);

            Pango::Rectangle rect=layout->get_cursor_strong_pos(cursor-text);
            cr->move_to(borderx+column*scalex + 2.5 + (double)rect.get_x()/Pango::SCALE, bordery+row*scaley + (scaley-text_height)*0.5 + (double)rect.get_y()/Pango::SCALE);
            cr->rel_line_to(0, (double)rect.get_height()/Pango::SCALE);
            cr->stroke();
        }
    }

    return true;
}


bool LyricEditor::on_motion_notify_event(GdkEventMotion* event)
{
    int col=(int(event->x) - borderx) / scalex;
    int row=(int(event->y) - bordery) / scaley;

    int time=col + row*beatsperline*beatsubdivisions;

	if (event->state & Gdk::BUTTON1_MASK) {
		if (curitem>0 && song[curitem-1].time>time)
			time=song[curitem-1].time;

		if (curitem+1<song.length() && song[curitem+1].time<time)
			time=song[curitem+1].time;

		if (curitem>=0 && song[curitem].time!=time) {
			song[curitem].time=time;
			queue_draw();
		}

		return true;
	}

    Syllable* new_highlighted_syllable=nullptr;

    for (int i=0;i<song.length();i++) {
        if (i+1<song.length() && time>=song[i].time && time<song[i+1].time)
            new_highlighted_syllable=song[i].syllable;

        if (song[i].time!=time) continue;

        curitem=i;

        auto cursor=Gdk::Cursor::create(get_toplevel()->get_display(), "ew-resize");
        get_toplevel()->get_window()->set_cursor(cursor);

        return true;
    }

    if (highlighted_syllable!=new_highlighted_syllable) {
        highlighted_syllable=new_highlighted_syllable;
        queue_draw();
    }

	curitem=-1;

    auto cursor=Gdk::Cursor::create(get_toplevel()->get_display(), "default");
    get_toplevel()->get_window()->set_cursor(cursor);

    return true;
}


bool LyricEditor::on_button_press_event(GdkEventButton* event)
{
    selected_syllable=highlighted_syllable;
    cursorpos=highlighted_syllable ? highlighted_syllable->text.length() : 0;

    if (event->type==GDK_DOUBLE_BUTTON_PRESS && !selected_syllable) {
        int col=(int(event->x) - borderx) / scalex;
        int row=(int(event->y) - bordery) / scaley;

        int time=col + row*beatsperline*beatsubdivisions;
        int index=song.find_index_before_time(time);

        if (index==song.length()) {
            song.insert(index, TimePoint(time, new Syllable));
            song.insert(index+1, TimePoint(time+default_syllable_duration, nullptr));
        }
        else {
            song.insert(index++, TimePoint(time, new Syllable));
            
            if (index==song.length() || song[index].time>time+default_syllable_duration)
                song.insert(index, TimePoint(time+default_syllable_duration, nullptr));
        }
    }

    queue_draw();

    return true;
}


bool LyricEditor::on_key_press_event(GdkEventKey* event)
{
    switch (event->keyval) {
    case GDK_KEY_BackSpace:
        if (selected_syllable) {
            if (event->state & GDK_CONTROL_MASK) {
                // reduce syllable duration to half
                int index=song.find(selected_syllable);
                int duration=song[index+1].time - song[index].time;

                duration/=2;

                if (song[index+1].syllable)
                    song.insert(index+1, TimePoint(song[index].time + duration, nullptr));
                else
                    song[index+1].time=song[index].time + duration;
            }
            else if (cursorpos>0)
                selected_syllable->text.erase(--cursorpos, 1);
            else if (int index=song.find(selected_syllable); index>0 && song[index-1].syllable) {
                cursorpos=song[index-1].syllable->text.length();
                song[index-1].syllable->text.append(selected_syllable->text);

                delete selected_syllable;
                song[index].syllable=nullptr;

                if (!song[index+1].syllable)
                    song.remove(index+1);

                selected_syllable=highlighted_syllable=song[index-1].syllable;
            }

            queue_draw();
        }
        return true;
    case GDK_KEY_Delete:
        if (event->state & GDK_CONTROL_MASK) {
            // delete entire syllable
            int index=song.find(selected_syllable);
            if (index<0) return true;

            delete selected_syllable;
            song[index].syllable=nullptr;

            if (!song[index+1].syllable)
                song.remove(index+1);

            selected_syllable=highlighted_syllable=nullptr;
            cursorpos=0;

            queue_draw();
        }
        else {
            // delete a single character
            if (selected_syllable && cursorpos<selected_syllable->text.length()) {
                selected_syllable->text.erase(cursorpos, 1);
                queue_draw();
            }
        }
        return true;
    case GDK_KEY_Left:
        if (selected_syllable && cursorpos>0) {
            cursorpos--;
            queue_draw();
        }
        return true;
    case GDK_KEY_Right:
        if (selected_syllable && cursorpos<selected_syllable->text.length()) {
            cursorpos++;
            queue_draw();
        }
        return true;
    case GDK_KEY_Tab:
        if (selected_syllable) {
            for (int i=0;i<song.length();i++) {
                if (song[i].syllable!=selected_syllable) continue;

                if (event->state & GDK_CONTROL_MASK) {
                    if (i-1>=0 && song[i-1].syllable)
                        selected_syllable=song[i-1].syllable;
                    else if (i-2>=0 && song[i-2].syllable)
                        selected_syllable=song[i-2].syllable;
                }
                else {
                    if (i+1<song.length() && song[i+1].syllable)
                        selected_syllable=song[i+1].syllable;
                    else if (i+2<song.length() && song[i+2].syllable)
                        selected_syllable=song[i+2].syllable;
                }

                cursorpos=selected_syllable->text.length();
                queue_draw();

                break;
            }
        }
        return true;
    default:
    	return gtk_im_context_filter_keypress(im_context, event);
    }
}


void LyricEditor::on_input(const gchar* chr)
{
    if (!selected_syllable) return;

    if (cursorpos==selected_syllable->text.length() && (!strcmp(chr, " ") || !strcmp(chr, "-"))) {
        int index=song.find(selected_syllable);

        if (index==song.length()) return;    // shouldn't happen

        if (!cursorpos && !song[index+1].syllable) {
            if (index+2==song.length() || song[index+1].time+default_syllable_duration<song[index+2].time) {
                song[index].time+=default_syllable_duration;
                song[index+1].time+=default_syllable_duration;
            }
        }
        else {
            if (*chr=='-')
                selected_syllable->text.append(chr);

            if (song[index+1].syllable) {
            }
            else {
                int endtime=song[index+1].time + default_syllable_duration;
                if (index+2<song.length() && endtime>song[index+2].time)
                    endtime=song[index+2].time;

                song[index+1].syllable=selected_syllable=new Syllable;
                cursorpos=0;

                song.insert(index+2, TimePoint(endtime, nullptr));
            }
        }
    }
    else {
    	selected_syllable->text.insert(cursorpos, chr);
        cursorpos++;
    }
    
	queue_draw();
}


void LyricEditor::commit_input(GtkIMContext* imctx, const gchar* chr, gpointer editorptr)
{
	reinterpret_cast<LyricEditor*>(editorptr)->on_input(chr);
}

