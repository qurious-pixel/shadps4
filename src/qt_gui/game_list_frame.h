#pragma once

#include <deque>
#include <QFutureWatcher>
#include <QGraphicsBlurEffect>
#include <QHeaderView>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtConcurrent>

#include "custom_dock_widget.h"
#include "game_list_grid.h"
#include "game_list_item.h"
#include "game_list_table.h"
#include "gui_settings.h"

class GameListFrame : public CustomDockWidget {
    Q_OBJECT
public:
    explicit GameListFrame(std::shared_ptr<GuiSettings> gui_settings, QWidget* parent = nullptr);
    ~GameListFrame();
    /** Fix columns with width smaller than the minimal section size */
    void FixNarrowColumns() const;

    /** Loads from settings. Public so that main frame can easily reset these settings if needed. */
    void LoadSettings();

    /** Saves settings. Public so that main frame can save this when a caching of column widths is
     * needed for settings backup */
    void SaveSettings();

    /** Resizes the columns to their contents and adds a small spacing */
    void ResizeColumnsToContents(int spacing = 20) const;

    /** Refresh the gamelist with/without loading game data from files. Public so that main frame
     * can refresh after vfs or install */
    void Refresh(const bool from_drive = false, const bool scroll_after = true);

    /** Repaint Gamelist Icons with new background color */
    void RepaintIcons(const bool& from_settings = false);

    /** Resize Gamelist Icons to size given by slider position */
    void ResizeIcons(const int& slider_pos);

public Q_SLOTS:
    void SetSearchText(const QString& text);
    void SetListMode(const bool& is_list);
private Q_SLOTS:
    void OnHeaderColumnClicked(int col);
    void OnRepaintFinished();
    void OnRefreshFinished();
    void RequestGameMenu(const QPoint& pos);
    void SetBackgroundImage(QTableWidgetItem* item);

Q_SIGNALS:
    void GameListFrameClosed();
    void RequestIconSizeChange(const int& val);
    void ResizedWindow(QTableWidgetItem* item);

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPixmap PaintedPixmap(const QPixmap& icon) const;
    void SortGameList() const;
    std::string CurrentSelectionPath();
    void PopulateGameList();
    void PopulateGameGrid(int maxCols, const QSize& image_size, const QColor& image_color);
    bool SearchMatchesApp(const QString& name, const QString& serial) const;
    bool IsEntryVisible(const game_info& game);
    static game_info GetGameInfoFromItem(const QTableWidgetItem* item);

    // Which widget we are displaying depends on if we are in grid or list mode.
    QMainWindow* m_game_dock = nullptr;
    QStackedWidget* m_central_widget = nullptr;

    // Game Grid
    GameListGrid* m_game_grid = nullptr;

    // Game List
    GameListTable* m_game_list = nullptr;
    QList<QAction*> m_columnActs;
    Qt::SortOrder m_col_sort_order;
    int m_sort_column;
    QMap<QString, QString> m_titles;

    // List Mode
    bool m_is_list_layout = true;
    bool m_old_layout_is_list = true;

    // data
    std::shared_ptr<GuiSettings> m_gui_settings;
    QList<game_info> m_game_data;
    std::vector<std::string> m_path_list;
    std::deque<game_info> m_games;
    QFutureWatcher<GameListItem*> m_repaint_watcher;
    QFutureWatcher<void> m_refresh_watcher;

    // Search
    QString m_search_text;

    // Icon Size
    int m_icon_size_index = 0;

    // Icons
    QSize m_icon_size;
    QColor m_icon_color;
    qreal m_margin_factor;
    qreal m_text_factor;

    QString formatSize(qint64 size) {
        static const QStringList suffixes = {"B", "KB", "MB", "GB", "TB"};
        int suffixIndex = 0;

        while (size >= 1024 && suffixIndex < suffixes.size() - 1) {
            size /= 1024;
            ++suffixIndex;
        }

        return QString("%1 %2").arg(size).arg(suffixes[suffixIndex]);
    }

    QString getFolderSize(const QDir& dir) {

        QDirIterator it(dir.absolutePath(), QDirIterator::Subdirectories);
        qint64 total = 0;

        while (it.hasNext()) {
            // check if entry is file
            if (it.fileInfo().isFile()) {
                total += it.fileInfo().size();
            }
            it.next();
        }

        // if there is a file left "at the end" get it's size
        if (it.fileInfo().isFile()) {
            total += it.fileInfo().size();
        }

        return formatSize(total);
    }

    void SetTableItem(GameListTable* game_list, int row, int column, QString itemStr) {
        QWidget* widget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        QLabel* label = new QLabel(itemStr);
        QTableWidgetItem* item = new QTableWidgetItem();

        label->setStyleSheet("color: white; font-size: 15px; font-weight: bold;");

        // Create shadow effect
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
        shadowEffect->setBlurRadius(5);               // Set the blur radius of the shadow
        shadowEffect->setColor(QColor(0, 0, 0, 160)); // Set the color and opacity of the shadow
        shadowEffect->setOffset(2, 2);                // Set the offset of the shadow

        label->setGraphicsEffect(shadowEffect); // Apply shadow effect to the QLabel

        layout->addWidget(label);
        widget->setLayout(layout);

        game_list->setItem(row, column, item);
        game_list->setCellWidget(row, column, widget);
    }

    QImage BlurImage(const QImage& image, const QRect& rect, int radius) {
        int tab[] = {14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2};
        int alpha = (radius < 1) ? 16 : (radius > 17) ? 1 : tab[radius - 1];

        QImage result = image.convertToFormat(QImage::Format_ARGB32);
        int r1 = rect.top();
        int r2 = rect.bottom();
        int c1 = rect.left();
        int c2 = rect.right();

        int bpl = result.bytesPerLine();
        int rgba[4];
        unsigned char* p;

        int i1 = 0;
        int i2 = 3;

        const int i2Minusi1 = i2 - i1;
        const int i1Shifted = i1 << 4;
        const int alphaOver16 = alpha / 16;

        for (int col = c1; col <= c2; col++) {
            p = result.scanLine(r1) + col * 4;
            for (int i = i1; i <= i2; i++)
                rgba[i] = p[i] << 4;

            p += bpl;
            for (int j = r1; j < r2; j++, p += bpl)
                for (int i = i1; i <= i2; i++)
                    p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
        }

        for (int row = r1; row <= r2; row++) {
            p = result.scanLine(row) + c1 * 4;
            for (int i = i1; i <= i2; i++)
                rgba[i] = p[i] << 4;

            p += 4;
            for (int j = c1; j < c2; j++, p += 4)
                for (int i = i1; i <= i2; i++)
                    p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
        }

        for (int col = c1; col <= c2; col++) {
            p = result.scanLine(r2) + col * 4;
            for (int i = i1; i <= i2; i++)
                rgba[i] = p[i] << 4;

            p -= bpl;
            for (int j = r1; j < r2; j++, p -= bpl)
                for (int i = i1; i <= i2; i++)
                    p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
        }

        for (int row = r1; row <= r2; row++) {
            p = result.scanLine(row) + c2 * 4;
            for (int i = i1; i <= i2; i++)
                rgba[i] = p[i] << 4;

            p -= 4;
            for (int j = c1; j < c2; j++, p -= 4)
                for (int i = i1; i <= i2; i++)
                    p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
        }

        return result;
    }
};
