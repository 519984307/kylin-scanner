#ifndef SHOWIMAGEWIDGET_H
#define SHOWIMAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>

#include "globalsignal.h"
#include "saneobject.h"
#include "toolbarwidget.h"

#define ShowImageWidgetMinimumSize QSize(387, 536)

class ShowImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowImageWidget(QWidget *parent = nullptr);

    const QString scannerImagePath;

    void setupGui();
    void initConnect();

public slots:
    void showNormalImageAfterScan();

    void showImageAfterClickedThumbnail(QString loadPath);

private:
#if 0
    QMenu *m_imageMenu;
    QAction *m_imageSaveAction;
#endif

    QImage *m_normalImage;

    QLabel *m_showImage;
    ToolBarWidget *m_toolbarWidget;
    QVBoxLayout *m_mainVLayout;
};

#endif // SHOWIMAGEWIDGET_H
