#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include "thumbnaildelegate.h"
#include "globalsignal.h"
#include "saneobject.h"

#include <QWidget>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QListData>
#include <QListIterator>
#include <QSize>
#include <QIcon>
#include <QGraphicsDropShadowEffect>
#include <QScrollBar>
#include <QFile>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QStyle>

#define ThumbnailWidgetMinimumWidth 68
#define ThumbnailIconSize QSize(40, 40)
#define ThumbnailShadowColor QColor(5,15,25,133)
#define ThumbnailShadowRadiu 20

class ScanStandardItem: public QStandardItem
{
public:
    void setPath(const QString &path);
    QString getPath();

    int getRowCountLocation() const;
    void setRowCountLocation(int value);

private:
    QString m_path;
    int rowCountLocation;
};

class ThumbnailWidget : public QListView
{
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget *parent = nullptr);

    static int scanPictureCount;

    void setupGui();
    void initConnect();
    void initSettings();


protected:
    //    void paintEvent(QPaintEvent *event) override;


private:
    ThumbnailDelegate *m_thumbnailDelegate;
    QStandardItemModel *m_thumbnailItemModel;

signals:

public slots:
    void showThumbnailIcon();
    void themeChangedWhite();
    void themeChangedBlack();

    void showNormalImageAfterScan();

    void clickedItemSlot(const QModelIndex &index);

};

#endif // THUMBNAILWIDGET_H
