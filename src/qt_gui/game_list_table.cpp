#include "game_list_table.h"

void GameListTable::clear_list() {
    clearSelection();
    clearContents();
}

void GameListTable::mousePressEvent(QMouseEvent* event) {
    if (QTableWidgetItem* item = itemAt(event->pos());
        !item || !item->data(Qt::UserRole).isValid()) {
        clearSelection();
        setCurrentItem(nullptr); // Needed for currentItemChanged
    }
    QTableWidget::mousePressEvent(event);
}