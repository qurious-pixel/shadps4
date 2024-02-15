#pragma once

#include <QActionGroup>
#include <QMainWindow>

class GuiSettings;
class GameListFrame;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    std::unique_ptr<Ui::MainWindow> ui;

    bool m_is_list_mode = true;
    bool m_save_slider_pos = false;
    int m_other_slider_pos = 0;

public:
    explicit MainWindow(std::shared_ptr<GuiSettings> gui_settings, QWidget* parent = nullptr);
    ~MainWindow();
    bool Init();
    void InstallPkg();

private Q_SLOTS:
    void ConfigureGuiFromSettings();
    void SetIconSizeActions(int idx) const;
    void ResizeIcons(int index);
    void SaveWindowState() const;

private:
    void CreateActions();
    void CreateDockWindows();
    void CreateConnects();

    QActionGroup* m_icon_size_act_group = nullptr;
    QActionGroup* m_list_mode_act_group = nullptr;

    // Dockable widget frames
    QMainWindow* m_main_window = nullptr;
    GameListFrame* m_game_list_frame = nullptr;

    std::shared_ptr<GuiSettings> m_gui_settings;
};
