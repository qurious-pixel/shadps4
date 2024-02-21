
#pragma once
#include <QApplication>
#include <QWidget>
#include <QLineEdit>

enum class Theme : int {
    Light,
    Dark,
    Green,
    Blue,
    Violet,
};

class WindowThemes : public QObject {
    Q_OBJECT
public Q_SLOTS:
    void SetWindowTheme(Theme theme, QLineEdit* mw_searchbar);
};
