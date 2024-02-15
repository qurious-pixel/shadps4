#pragma once

#include "game_list_table.h"

class GameListGridDelegate;

class GameListGrid : public GameListTable {
    Q_OBJECT

    QSize m_icon_size;
    QColor m_icon_color;
    qreal m_margin_factor;
    qreal m_text_factor;
    bool m_text_enabled = true;

public:
    explicit GameListGrid(const QSize& icon_size, QColor icon_color, const qreal& margin_factor,
                            const qreal& text_factor, const bool& showText);

    void enableText(const bool& enabled);
    void setIconSize(const QSize& size) const;
    GameListItem* addItem(const game_info& app, const QString& name, const int& row,
                            const int& col);

    [[nodiscard]] qreal getMarginFactor() const;

private:
    GameListGridDelegate* grid_item_delegate;
};
