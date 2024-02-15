#include <QtWidgets/QApplication>

#include "qt_gui/gui_settings.h"
#include "qt_gui/main_window.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    auto m_gui_settings = std::make_shared<GuiSettings>();
    MainWindow* m_main_window = new MainWindow(m_gui_settings, nullptr);
    m_main_window->Init();

    return a.exec();
}