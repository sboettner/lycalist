#include <gtkmm.h>
#include "songmodel.h"
#include "lyricedit.h"
#include "melodyedit.h"


class MainWindow:public Gtk::Window {
public:
    MainWindow();

private:
    Song            song;

    Gtk::Notebook   notebook;
    LyricEditor     lyric_editor;
    MelodyEditor    melody_editor;
};


MainWindow::MainWindow():lyric_editor(song), melody_editor(song)
{
    set_default_size(960, 640);

    add(notebook);
    notebook.append_page(lyric_editor, "Lyrics");
    notebook.append_page(melody_editor, "Melody");

    show_all_children();
}


int main(int argc, char* argv[])
{
    auto app=Gtk::Application::create(argc, argv);

    auto settings=Gtk::Settings::get_default();
    settings->property_gtk_application_prefer_dark_theme()=true;

    MainWindow wnd;

    return app->run(wnd);
}
