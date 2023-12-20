#include <gtkmm.h>


struct Syllable {
	Glib::ustring	text;
};


class LyricEditor:public Gtk::Widget {
public:
    LyricEditor();

protected:
    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const  override;
    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
  
    void on_size_allocate(Gtk::Allocation& allocation) override;
    void on_realize() override;
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;
    bool on_button_press_event(GdkEventButton* event) override;
	bool on_key_press_event(GdkEventKey* event) override; 	

private:
    void on_input(const gchar*);
	static void commit_input(GtkIMContext*, const gchar*, gpointer);

    static void draw_syllable_box_closed(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h);
    static void draw_syllable_box_open_right(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h);
    static void draw_syllable_box_open_left(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, int w, int h);

    int find_syllable_index(Syllable*);

    Glib::RefPtr<Gdk::Window> m_refGdkWindow;
	GtkIMContext*	im_context;

    struct TimePoint {
        int         time;
        Syllable*   syllable;

        TimePoint(int time, Syllable* syl):time(time), syllable(syl) {}
    };

    std::vector<TimePoint> syllables;

    Syllable*   selected_syllable=nullptr;
    Syllable*   highlighted_syllable=nullptr;
    size_t      cursorpos=0;

    int beatsperline=8;
    int beatsubdivisions=12;

    int default_syllable_duration=12;

    int borderx=32;
    int bordery=32;
    int scalex=8;
    int scaley=32;

	int curitem=-1;
};


LyricEditor::LyricEditor()
{
    set_has_window(true);
	set_can_focus(true);

    add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
	add_events(Gdk::KEY_PRESS_MASK);

	im_context=gtk_im_context_simple_new();
	g_signal_connect(im_context, "commit", G_CALLBACK(commit_input), this);

    syllables.emplace_back(6, new Syllable);
    syllables.emplace_back(30, nullptr);
    syllables.emplace_back(80, new Syllable);
    syllables.emplace_back(120, nullptr);
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

    for (int i=0;i+1<syllables.size();i++) {
        Syllable* syl=syllables[i].syllable;
        if (!syl) continue;

        int column=syllables[i].time % ncolumns;
        int row   =syllables[i].time / ncolumns;

        int columnend=syllables[i+1].time % ncolumns;
        int rowend   =syllables[i+1].time / ncolumns;

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
		if (curitem>0 && syllables[curitem-1].time>time)
			time=syllables[curitem-1].time;

		if (curitem+1<syllables.size() && syllables[curitem+1].time<time)
			time=syllables[curitem+1].time;

		if (curitem>=0 && syllables[curitem].time!=time) {
			syllables[curitem].time=time;
			queue_draw();
		}

		return true;
	}

    Syllable* new_highlighted_syllable=nullptr;

    for (int i=0;i<syllables.size();i++) {
        if (i+1<syllables.size() && time>=syllables[i].time && time<syllables[i+1].time)
            new_highlighted_syllable=syllables[i].syllable;

        if (syllables[i].time!=time) continue;

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
                int index=find_syllable_index(selected_syllable);
                int duration=syllables[index+1].time - syllables[index].time;

                duration/=2;

                if (syllables[index+1].syllable)
                    syllables.insert(syllables.begin()+index+1, TimePoint(syllables[index].time + duration, nullptr));
                else
                    syllables[index+1].time=syllables[index].time + duration;
            }
            else if (cursorpos>0)
                selected_syllable->text.erase(--cursorpos, 1);
            else if (int index=find_syllable_index(selected_syllable); index>0 && syllables[index-1].syllable) {
                cursorpos=syllables[index-1].syllable->text.length();
                syllables[index-1].syllable->text.append(selected_syllable->text);

                delete selected_syllable;
                syllables[index].syllable=nullptr;

                if (!syllables[index+1].syllable)
                    syllables.erase(syllables.begin()+index+1);

                selected_syllable=highlighted_syllable=syllables[index-1].syllable;
            }

            queue_draw();
        }
        return true;
    case GDK_KEY_Delete:
        if (event->state & GDK_CONTROL_MASK) {
            // delete entire syllable
            int index=find_syllable_index(selected_syllable);
            if (index<0) return true;

            delete selected_syllable;
            syllables[index].syllable=nullptr;

            if (!syllables[index+1].syllable)
                syllables.erase(syllables.begin()+index+1);

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
            for (int i=0;i<syllables.size();i++) {
                if (syllables[i].syllable!=selected_syllable) continue;

                if (event->state & GDK_CONTROL_MASK) {
                    if (i-1>=0 && syllables[i-1].syllable)
                        selected_syllable=syllables[i-1].syllable;
                    else if (i-2>=0 && syllables[i-2].syllable)
                        selected_syllable=syllables[i-2].syllable;
                }
                else {
                    if (i+1<syllables.size() && syllables[i+1].syllable)
                        selected_syllable=syllables[i+1].syllable;
                    else if (i+2<syllables.size() && syllables[i+2].syllable)
                        selected_syllable=syllables[i+2].syllable;
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
        int index=find_syllable_index(selected_syllable);

        if (index==syllables.size()) return;    // shouldn't happen

        if (!cursorpos && !syllables[index+1].syllable) {
            if (index+2==syllables.size() || syllables[index+1].time+default_syllable_duration<syllables[index+2].time) {
                syllables[index].time+=default_syllable_duration;
                syllables[index+1].time+=default_syllable_duration;
            }
        }
        else {
            if (*chr=='-')
                selected_syllable->text.append(chr);

            if (syllables[index+1].syllable) {
            }
            else {
                int endtime=syllables[index+1].time + default_syllable_duration;
                if (index+2<syllables.size() && endtime>syllables[index+2].time)
                    endtime=syllables[index+2].time;

                syllables[index+1].syllable=selected_syllable=new Syllable;
                cursorpos=0;

                syllables.insert(syllables.begin()+index+2, TimePoint(endtime, nullptr));
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


int LyricEditor::find_syllable_index(Syllable* syl)
{
    if (!syl) return -1;

    int index=0;
    while (index<syllables.size() && syllables[index].syllable!=syl)
        index++;
    
    return index==syllables.size() ? -1 : index;
}


int main(int argc, char* argv[])
{
    auto app=Gtk::Application::create(argc, argv);

    auto settings=Gtk::Settings::get_default();
    settings->property_gtk_application_prefer_dark_theme()=true;

    Gtk::Window wnd;
	wnd.add_events(Gdk::KEY_PRESS_MASK);

    LyricEditor editor;
    wnd.add(editor);
    editor.show();

    wnd.set_default_size(960, 640);
    wnd.show_all_children();

    return app->run(wnd);
}
