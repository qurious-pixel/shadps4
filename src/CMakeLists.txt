qt_add_executable(shadps4
    qt_gui/main_window.ui
    qt_gui/main_window.cpp
	qt_gui/main_window.h
    qt_gui/gui_settings.cpp
    qt_gui/gui_settings.h
    qt_gui/settings.cpp
    qt_gui/settings.h
    qt_gui/gui_save.h
    qt_gui/custom_dock_widget.h
    qt_gui/custom_table_widget_item.cpp
    qt_gui/custom_table_widget_item.h
    qt_gui/game_list_item.h
    qt_gui/game_list_table.cpp
    qt_gui/game_list_table.h
    qt_gui/game_info.h
    qt_gui/game_list_grid.cpp
    qt_gui/game_list_grid.h
    qt_gui/game_list_grid_delegate.cpp
    qt_gui/game_list_grid_delegate.h
    qt_gui/qt_utils.h
    common/io_file.cpp
    common/io_file.h
    common/types.h
    main.cpp
)

target_link_libraries(shadps4 PRIVATE Qt6::Widgets)

set_target_properties(shadps4 PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)