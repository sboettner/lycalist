class Scale;
class Chord;

class ChordChooser:public Gtk::Popover {
public:
    ChordChooser(const Gtk::Widget& parent, const Scale&, Chord&);

private:
    const Scale&            scale;
    Chord&                  chord;

    Gtk::VBox               vbox;
    
    Gtk::Grid               rootgrid;
    Gtk::RadioButtonGroup   rootgroup;

    Gtk::HSeparator         hsep;

    Gtk::FlowBox            qualflow;
    Gtk::RadioButtonGroup   qualgroup;
};
