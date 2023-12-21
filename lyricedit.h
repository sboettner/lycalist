class Song;
struct Syllable;


class LyricEditor:public Gtk::Widget {
public:
    LyricEditor(Song&);

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

    Glib::RefPtr<Gdk::Window> m_refGdkWindow;
	GtkIMContext*	im_context;

    Song&       song;

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

