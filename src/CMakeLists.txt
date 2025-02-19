qt_add_executable(shadps4
    qt_gui/main_window_ui.h
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
    qt_gui/game_list_utils.h
    qt_gui/game_info.h
    qt_gui/game_list_grid.cpp
    qt_gui/game_list_grid.h
    qt_gui/game_list_grid_delegate.cpp
    qt_gui/game_list_grid_delegate.h
    qt_gui/game_list_frame.cpp
    qt_gui/game_list_frame.h
    qt_gui/qt_utils.h
    qt_gui/game_install_dialog.cpp
    qt_gui/game_install_dialog.h
    qt_gui/main_window_themes.cpp
    qt_gui/main_window_themes.h
    common/io_file.cpp
    common/io_file.h
    common/types.h
    common/endian.h
    common/crypto.cpp
    common/crypto.h
    common/keys.h
    emulator/file_format/psf.cpp
    emulator/file_format/psf.h
    emulator/file_format/pkg_type.cpp
    emulator/file_format/pkg_type.h
    emulator/file_format/pkg.cpp
    emulator/file_format/pkg.h
    emulator/file_format/pfs.h
    emulator/loader.cpp
    emulator/loader.h
    main.cpp
)
create_target_directory_groups(shadps4)

if(WIN32)
    target_sources(shadps4 PRIVATE shadps4.rc)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MSVC) #ugly probably make it better :D
    target_link_libraries(shadps4 PRIVATE Qt6::Widgets Qt6::Concurrent cryptoppwin zlib)
else()
    target_link_libraries(shadps4 PRIVATE Qt6::Widgets Qt6::Concurrent cryptopp::cryptopp zlib)
endif()

target_include_directories(shadps4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(shadps4 PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
add_custom_command(TARGET shadps4 POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
     "${PROJECT_SOURCE_DIR}/externals/zlib-ng-win/bin/zlib-ngd2.dll" $<TARGET_FILE_DIR:shadps4>)
else()
add_custom_command(TARGET shadps4 POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
     "${PROJECT_SOURCE_DIR}/externals/zlib-ng-win/bin/zlib-ng2.dll" $<TARGET_FILE_DIR:shadps4>)
endif()