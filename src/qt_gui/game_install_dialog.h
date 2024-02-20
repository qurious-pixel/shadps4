#pragma once

#include <QDialog>
#include "gui_settings.h"

class QLineEdit;

class GameInstallDialog final : public QDialog {
public:
    GameInstallDialog(std::shared_ptr<GuiSettings> gui_settings);
    ~GameInstallDialog();

private slots:
    void Browse();

private:
    QWidget* SetupGamesDirectory();
    QWidget* SetupDialogActions();
    void Save();

private:
    QLineEdit* m_gamesDirectory;
    std::shared_ptr<GuiSettings> m_gui_settings;
};