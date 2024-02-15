#include "gui_settings.h"

GuiSettings::GuiSettings(QObject* parent) {
    m_settings.reset(new QSettings("shadps4.ini", QSettings::Format::IniFormat,
                                   parent)); // TODO make the path configurable
}

void GuiSettings::SetGamelistColVisibility(int col, bool val) const {
    SetValue(GetGuiSaveForColumn(col), val);
}

bool GuiSettings::GetGamelistColVisibility(int col) const {
    return GetValue(GetGuiSaveForColumn(col)).toBool();
}

GuiSave GuiSettings::GetGuiSaveForColumn(int col) {
    return GuiSave{gui::game_list,
                    "visibility_" +
                        gui::get_game_list_column_name(static_cast<gui::game_list_columns>(col)),
                    true};
}
QSize GuiSettings::SizeFromSlider(int pos) {
    return gui::game_list_icon_size_min +
           (gui::game_list_icon_size_max - gui::game_list_icon_size_min) *
               (1.f * pos / gui::game_list_max_slider_pos);
}