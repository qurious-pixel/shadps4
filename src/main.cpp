#include <QtWidgets/QApplication>

#include "qt_gui/main_window.h"
#include "qt_gui/gui_settings.h"


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	std::shared_ptr<gui_settings> m_gui_settings;
	m_gui_settings.reset(new gui_settings());
	main_window* m_main_window  = new main_window(m_gui_settings, nullptr);
	m_main_window->Init();

	return a.exec();
}