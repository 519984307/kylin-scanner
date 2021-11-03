#ifndef SHOWIMAGEWIDGET_H
#define SHOWIMAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QMenu>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QFile>
#include <QPdfWriter>

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

//    int U(const char *str);
    int toUnicode(QString str);
    void setPdfSize(QPdfWriter *pdfWriter, QString size);
    void saveToPdf(QImage img, QString pathname);

    QImage *imageSave(QString filename);

public slots:
    void showNormalImageAfterScan();

    void showImageAfterClickedThumbnail(QString loadPath);

    void saveImage(QString filename);

private:

    QImage *m_editImage;
    QImage *m_normalImage;

    QLabel *m_showImage;
    QHBoxLayout *m_showImageHLayout;

    ToolBarWidget *m_toolbarWidget;
    QVBoxLayout *m_mainVLayout;
};

#endif // SHOWIMAGEWIDGET_H
