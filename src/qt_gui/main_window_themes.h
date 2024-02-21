
#pragma once
#include <QApplication>
#include <QLineEdit>
#include <QWidget>

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
