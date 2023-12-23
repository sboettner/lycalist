class Scale;

class ChordChooser:public Gtk::Popover {
public:
    ChordChooser(const Gtk::Widget& parent, const Scale&);

private:
    Gtk::Grid               rootgrid;
    Gtk::RadioButtonGroup   rootgroup;
};
