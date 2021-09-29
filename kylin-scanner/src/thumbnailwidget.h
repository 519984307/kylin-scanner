#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include "thumbnaildelegate.h"
#include "globalsignal.h"

#include <QWidget>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QListData>
#include <QListIterator>
#include <QSize>
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

class ThumbnailWidget : public QListView
{
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();
    void initSettings();

protected:
//    void paintEvent(QPaintEvent *event) override;


private:
    ThumbnailDelegate *m_thumbnailDelegate;

signals:

public slots:
    void showThumbnailIcon();
    void themeChangedWhite();
    void themeChangedBlack();

};

#endif // THUMBNAILWIDGET_H
