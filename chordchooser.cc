#include <gtkmm.h>
#include "songmodel.h"
#include "chordchooser.h"


ChordChooser::ChordChooser(const Gtk::Widget& parent, const Scale& scale, Chord& chord):Gtk::Popover(parent), scale(scale), chord(chord)
{
    add(vbox);

    rootgrid.set_column_homogeneous(true);
    rootgrid.set_row_homogeneous(true);
    vbox.pack_start(rootgrid);
    vbox.pack_start(hsep);
    vbox.pack_start(qualflow);

    const static char* romannumeral[]={ "I", "I#", "II", "II#", "III", "III#", "IV", "IV#", "V", "V#", "VI", "VI#", "VII", "VII#" };

    for (int i=0;i<12;i++) {
        int offs=scale.get_display_offset_for_note(i);

        Gtk::RadioButton* b=new Gtk::RadioButton(romannumeral[offs]);
        b->set_group(rootgroup);
        b->set_mode(false);
        b->signal_toggled().connect([this, b, i]() {
            if (b->get_active()) {
                this->chord.set_root(i);
            }
        });
        rootgrid.attach(*b, offs, (offs&1)^1, 2, 1);
    }

    const static char* qualitynames[]={ "Maj", "Min", "Maj6", "Min6", "Maj7", "Min7", "Dom7", "Sus2", "Sus4", "Dim" };
    
    for (int i=0;i<10;i++) {
        Gtk::RadioButton* b=new Gtk::RadioButton(qualitynames[i]);
        b->set_group(qualgroup);
        b->set_mode(false);
        qualflow.insert(*b, i);
    }

    show_all_children();
}
