#include <gtkmm.h>
#include "songmodel.h"
#include "lyricedit.h"


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
