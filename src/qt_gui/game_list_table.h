#pragma once

#include <QMouseEvent>
#include <QTableWidget>

#include "game_info.h"
#include "game_list_item.h"

struct gui_game_info {
    GameInfo info{};
    QPixmap icon;
    QPixmap pxmap;
    GameListItem* item = nullptr;
};

typedef std::shared_ptr<gui_game_info> game_info;
Q_DECLARE_METATYPE(game_info)

class GameListTable : public QTableWidget {
public:
    void clear_list();

protected:
    void mousePressEvent(QMouseEvent* event) override;
};