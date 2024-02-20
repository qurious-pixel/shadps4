
#pragma once
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

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
    void SetWindowTheme(Theme theme);
};
