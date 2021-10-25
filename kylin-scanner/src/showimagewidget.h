#ifndef SHOWIMAGEWIDGET_H
#define SHOWIMAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QMenu>
#include <QAction>

#include "globalsignal.h"

#define ShowImageWidgetMinimumSize QSize(387, 536)

class ShowImageWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ShowImageWidget(QWidget *parent = nullptr);

    const QString scannerImagePath;

    void setupGui();
    void initConnect();

public slots:
    void showNormalImageAfterScan();

signals:

private:
    QMenu *m_imageMenu;
    QAction *m_imageSaveAction;
    QImage *m_normalImage;


};

#endif // SHOWIMAGEWIDGET_H
