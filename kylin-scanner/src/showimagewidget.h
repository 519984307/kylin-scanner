#ifndef SHOWIMAGEWIDGET_H
#define SHOWIMAGEWIDGET_H

#include <QWidget>
#include <QWidgetList>
#include <QStackedWidget>
#include <QLabel>
#include <QPainter>
#include <QImage>
#include <QMenu>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QFile>
#include <QPdfWriter>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QStack>
#include <QMatrix>
#include <QThread>

#include "globalsignal.h"
#include "saneobject.h"
#include "toolbarwidget.h"
#include "watermarkdialog.h"
#include "crop.h"
#include "beauty.h"
#include "rectify.h"

#define ShowImageWidgetMinimumSize QSize(387, 536)



class ShowImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowImageWidget(QWidget *parent = nullptr);


    void setupGui();
    void initConnect();

    int toUnicode(QString str);
    void setPdfSize(QPdfWriter *pdfWriter, QString size);
    void saveToPdf(QImage img, QString pathname);

    QImage *imageSave(QString filename);

    QString setPixmapScaled(QImage img, QLabel *lab);

    void setPixmapScaledByProportion(double scaledNumber);

    const QString scannerImagePath;
    const int defaultImageLableWidth = 387;
    const int defaultImageLableHeight = 536;

    QSize defaultScanImageSize;


protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

public slots:
    void showNormalImageAfterScan();

    void showImageAfterClickedThumbnail(QString loadPath);

    void saveImage(QString filename);

    void cropSlot();
    void rotateSlot();
    void mirrorSlot();
    void watermarkSlot();
    void zoomoutNormalImage();
    void zoominNormalImage();

    void beautyStartSlot();
    void beautyStopSlot();
    void rectifyStartSlot();
    void rectifyStopSlot();
    void ocrStartSlot();
    void ocrStopSlot();

    void saveCurrentPicture();
    void loadAfterBROPicture();

private:
    QStack<QImage> m_imageStack;
    double proportion = 1;
    double proportionForPercentage = 1;

    QImage *m_stackImage;
    QImage *m_editImage;
    QImage *m_normalImage;

    QLabel *m_showImageLabel;
    CropLabel *m_cropLabel;
    QStackedWidget *m_showImageAndCropWidget;
    QHBoxLayout *m_showImageHLayout;

    ToolBarWidget *m_toolbarWidget;
    QVBoxLayout *m_mainVLayout;

};

#endif // SHOWIMAGEWIDGET_H
