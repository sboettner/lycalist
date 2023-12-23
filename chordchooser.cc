#include <gtkmm.h>
#include "songmodel.h"
#include "chordchooser.h"


ChordChooser::ChordChooser(const Gtk::Widget& parent, const Scale& scale):Gtk::Popover(parent)
{
    rootgrid.set_column_homogeneous(true);
    rootgrid.set_row_homogeneous(true);
    add(rootgrid);

    const char* romannumeral[]={ "I", "I#", "II", "II#", "III", "III#", "IV", "IV#", "V", "V#", "VI", "VI#", "VII", "VII#" };

    for (int i=0;i<12;i++) {
        int offs=scale.get_display_offset_for_note(i);

        Gtk::RadioButton* b=new Gtk::RadioButton(romannumeral[offs]);
        b->set_group(rootgroup);
        b->set_mode(false);
        rootgrid.attach(*b, offs, (offs&1)^1, 2, 1);
    }

    show_all_children();
}
