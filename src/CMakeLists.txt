qt_add_executable(shadps4
    qt_gui/main_window.ui
    qt_gui/main_window.cpp
	qt_gui/main_window.h
    qt_gui/gui_settings.cpp
    qt_gui/gui_settings.h
    qt_gui/settings.cpp
    qt_gui/settings.h
    qt_gui/gui_save.h
    main.cpp
)

target_link_libraries(shadps4 PRIVATE Qt6::Widgets)

set_target_properties(shadps4 PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)