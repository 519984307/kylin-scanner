#include "showimagewidget.h"
#include <ukui-log4qt.h>

ShowImageWidget::ShowImageWidget(QWidget *parent) : QWidget(parent)
  , m_stackImage (new QImage())
  , m_editImage (new QImage())
  , m_normalImage (new QImage())
  , m_showImageLabel (new QLabel())
  , m_cropLabel (new CropLabel())
  , m_showImageAndCropWidget (new QStackedWidget())
  , m_showImageHLayout (new QHBoxLayout())
  , m_toolbarWidget (new ToolBarWidget())
  , m_mainVLayout (new QVBoxLayout())
  , scannerImagePath(g_config_signal->scannerImagePath)
{
    setupGui();
    initConnect();
}

void ShowImageWidget::setupGui()
{
    this->setMinimumSize(QSize(644, 636));
    this->setFocusPolicy(Qt::StrongFocus);

    m_cropLabel->setMinimumSize(QSize(387, 536));
    m_normalImage->load(scannerImagePath+ QString("/scan.pnm"));
    setPixmapScaled(*m_normalImage, m_cropLabel);

    KyInfo() << "load m_normalImage size: " << m_normalImage->size()
             << "showImageWidget size: " << this->size();


    m_showImageLabel->setMinimumSize(QSize(387, 536));

    setPixmapScaled(*m_normalImage, m_showImageLabel);
    g_user_signal->toolbarPercentageChanged();

    m_showImageAndCropWidget->addWidget(m_cropLabel);
    m_showImageAndCropWidget->addWidget(m_showImageLabel);
    m_showImageAndCropWidget->setCurrentWidget(m_showImageLabel);


    m_showImageHLayout->setSpacing(0);
    m_showImageHLayout->addSpacing(128);
    m_showImageHLayout->addStretch();
    m_showImageHLayout->addWidget(m_showImageAndCropWidget);
    m_showImageHLayout->addSpacing(129);
    m_showImageHLayout->addStretch();
    m_showImageHLayout->setContentsMargins(0, 0, 0, 0);

    m_mainVLayout->setSpacing(0);
    m_mainVLayout->addSpacing(24);
    m_mainVLayout->addStretch();
    m_mainVLayout->addLayout(m_showImageHLayout);
    m_mainVLayout->addSpacing(24);
    m_mainVLayout->addWidget(m_toolbarWidget, 0, Qt::AlignVCenter | Qt::AlignCenter);
    m_mainVLayout->addSpacing(16);
    m_mainVLayout->addStretch();
    m_mainVLayout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(m_mainVLayout);
}

void ShowImageWidget::initConnect()
{
    connect(g_user_signal, &GlobalUserSignal::scanThreadFinishedImageLoadSignal, this, &ShowImageWidget::showNormalImageAfterScan);

    connect(g_user_signal, &GlobalUserSignal::showImageAfterClickedThumbnailSignal, this, &ShowImageWidget::showImageAfterClickedThumbnail);

    connect(g_user_signal, &GlobalUserSignal::saveAsButtonClickedSignal, this, &ShowImageWidget::saveImage);

    connect(g_user_signal, &GlobalUserSignal::toolbarCropOperationSignal, this, &ShowImageWidget::cropSlot);
    connect(g_user_signal, &GlobalUserSignal::toolbarRotateOperationSignal, this, &ShowImageWidget::rotateSlot);
    connect(g_user_signal, &GlobalUserSignal::toolbarMirrorOperationSignal, this, &ShowImageWidget::mirrorSlot);
    connect(g_user_signal, &GlobalUserSignal::toolbarWatermarkOperationSignal, this, &ShowImageWidget::watermarkSlot);

    connect(g_user_signal, &GlobalUserSignal::toolbarZoomoutOperationSignal, this, &ShowImageWidget::zoomoutNormalImage);
    connect(g_user_signal, &GlobalUserSignal::toolbarZoominOperationSignal, this, &ShowImageWidget::zoominNormalImage);
}

constexpr inline int U(const char *str)
{
    return str[0] + (str[1] ? U(str + 1) : 0);
}

int ShowImageWidget::toUnicode(QString str)
{
    char  *ch;
    QByteArray ba = str.toLatin1();
    ch = ba.data();
    return ch[0] + (ch[1] ? toUnicode(ch + 1) : 0);

}

void ShowImageWidget::setPdfSize(QPdfWriter *pdfWriter, QString size)
{
    switch (toUnicode(size)) {
    case U("A0"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A0));
        break;
    case U("A1"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A1));
        break;
    case U("A2"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A2));
        break;
    case U("A3"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A3));
        break;
    case U("A4"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A4));
        break;
    case U("A5"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A5));
        break;
    case U("A6"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A6));
        break;
    case U("A7"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A7));
        break;
    case U("A8"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A8));
        break;
    case U("A9"):
        pdfWriter->setPageSize(QPageSize(QPageSize::A9));
        break;
    default:
        pdfWriter->setPageSize(QPageSize(QPageSize::A4));
        break;
    }

}

void ShowImageWidget::saveToPdf(QImage img, QString pathname)
{
    QFile pdfFile(pathname);
    pdfFile.open(QIODevice::WriteOnly);
    QPdfWriter *pdfWriter = new QPdfWriter(&pdfFile);
    setPdfSize(pdfWriter, g_sane_object->userInfo.size);

    int resolution = g_sane_object->resolutionValue;
    pdfWriter->setResolution(resolution);

    int pageMargin = 0;
    pdfWriter->setPageMargins(QMarginsF(pageMargin, pageMargin, pageMargin, pageMargin));

    QPainter *pdfPainter = new QPainter(pdfWriter);

    int yCurrentP = 0;
    int xCurrentP = 0;

    QPixmap pixmap = QPixmap::fromImage(img);
    pdfPainter->drawPixmap(xCurrentP, yCurrentP, pixmap.width(), pixmap.height(), pixmap);

    delete pdfPainter;
    delete pdfWriter;
    pdfFile.close();
}

QImage *ShowImageWidget::imageSave(QString filename)
{
    KyInfo() << "save image: " << filename;
    if (g_sane_object->ocrFlag == 0) {
        *m_editImage = m_normalImage->copy();
        if (filename.endsWith(".pdf"))
            return m_editImage;
        if (filename.endsWith(".png") || filename.endsWith(".jpg") || filename.endsWith(".bmp"))
            m_editImage->save(filename);
    } else {
        if (!filename.endsWith(".txt"))
            filename += ".txt";
        QFile file(filename);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QByteArray str = g_sane_object->ocrOutputText.toUtf8();
        file.write(str);
        file.close();
    }
    return NULL;
}

QString ShowImageWidget::setPixmapScaled(QImage img, QLabel *lab)
{
    double labWidth = lab->width();
    double labHeight = lab->height();

    double imgWidth = img.width();
    double imgHeight = img.height();

    proportion = 1.0;

    if ((labWidth / imgWidth) <= (labHeight / imgHeight)) {
        proportion = labWidth / imgWidth;
    } else {
        proportion = labHeight / imgHeight;
    }

#if 0
    m_normalImage->scaled(lab->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    lab->setScaledContents(true);
    lab->setPixmap(QPixmap::fromImage(*m_normalImage));
#endif

    QPixmap pixmap = QPixmap::fromImage(img);
    QPixmap fitpixmap = pixmap.scaled(lab->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    lab->setPixmap(fitpixmap);

    KyInfo() << "proportion: " << proportion;
    int proportionInt = static_cast<int>(proportion * 100);

    KyInfo() << "proportionInt: " << proportionInt;
    QString proportionString = QString("%1").arg(proportionInt) + "%";

    KyInfo() << "proportionString: " << proportionString;
    g_sane_object->percentage = proportionString;

    return proportionString;
}

void ShowImageWidget::setPixmapScaledByProportion(double scaledNumber)
{
    int scaledWidth = static_cast<int>(m_showImageLabel->width() * scaledNumber);
    int scaledHeight = static_cast<int>(m_showImageLabel->height() * scaledNumber);

    KyInfo() << "scaledNumber = " << scaledNumber
             << "scaleWidth = " << scaledWidth
             << "scaleHeight = " << scaledHeight;

    *m_editImage = m_normalImage->copy();

    m_editImage->scaled(scaledWidth, scaledHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    *m_normalImage = m_editImage->copy();
    m_showImageLabel->setPixmap(QPixmap::fromImage(*m_normalImage));
    m_showImageLabel->setScaledContents(true);
}

//void ShowImageWidget::resizeEvent(QResizeEvent *event)
//{
//    Q_UNUSED(event);
//}

void ShowImageWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:

        if (m_showImageAndCropWidget->currentWidget() == m_cropLabel) {
            KyInfo() << "pressed key(Enter): " << event->key();

            int x1, y1, x2, y2;
            if (m_cropLabel->getStartX() <= m_cropLabel->getEndX()) {
                x1 = m_cropLabel->getStartX() - ((m_cropLabel->width() - m_editImage->width())/2);
                x2 = m_cropLabel->getEndX() - ((m_cropLabel->width() - m_editImage->width())/2);
            } else {
                x1 = m_cropLabel->getEndX()- ((m_cropLabel->width() - m_editImage->width())/2);
                x2 = m_cropLabel->getStartX()- ((m_cropLabel->width() - m_editImage->width())/2);
            }

            if (m_cropLabel->getStartY() <= m_cropLabel->getEndY()) {
                y1 = m_cropLabel->getStartY()- ((m_cropLabel->height() - m_editImage->height())/2);
                y2 = m_cropLabel->getEndY()- ((m_cropLabel->height() - m_editImage->height())/2);
            } else {
                y1 = m_cropLabel->getEndY()- ((m_cropLabel->height() - m_editImage->height())/2);
                y2 = m_cropLabel->getStartY()- ((m_cropLabel->height() - m_editImage->height())/2);
            }

            QImage cropImage = m_normalImage->copy(x1, y1, (x2-x1), (y2-y1));

            *m_editImage = cropImage;
            setPixmapScaled(*m_editImage, m_showImageLabel);

            *m_normalImage = m_editImage->copy();
            setPixmapScaled(*m_normalImage, m_showImageLabel);

            m_cropLabel->initStartAndEndXY();


            m_showImageAndCropWidget->setCurrentWidget(m_showImageLabel);

            g_sane_object->cropFlag = 0;
        }
        break;

    case Qt::Key_Z:
        if (event->modifiers() == Qt::ControlModifier) {
            KyInfo() << "pressed key(ctrl+z): " << event->key();

            if (! m_imageStack.isEmpty()) {
                *m_editImage = m_imageStack.pop();
                setPixmapScaled(*m_editImage, m_showImageLabel);

                *m_normalImage = m_editImage->copy();
                setPixmapScaled(*m_normalImage, m_showImageLabel);
            }

        }
        break;
    case Qt::Key_Escape:
        KyInfo() << "pressed key(Esc): " << event->key();

        break;

    default:
        KyInfo() << "pressed key: " << event->key();
        break;
    }
}

void ShowImageWidget::showNormalImageAfterScan()
{
    m_showImageAndCropWidget->setCurrentWidget(m_showImageLabel);

    QString loadPath = g_sane_object->loadFullScanFileName;
    m_normalImage->load(loadPath);

    KyInfo() << "loadPath = " << loadPath
             << "loadm_normalImage size: " << m_normalImage->size() << "showImageWidget size: " << m_normalImage->size();

    QString percentagedouble = setPixmapScaled(*m_normalImage, m_showImageLabel);
    g_user_signal->toolbarPercentageChanged();
    KyInfo() << "percentagedouble = " << percentagedouble;


    QString savePath = g_sane_object->saveFullScanFileName;
    KyInfo() << "savePath = " << savePath;

    g_sane_object->normalImagePath = loadPath;

    saveImage(savePath);
}

void ShowImageWidget::showImageAfterClickedThumbnail(QString loadPath)
{
    KyInfo() << "loadPath = " << loadPath;
    m_normalImage->load(loadPath);

    QString percentagedouble = setPixmapScaled(*m_normalImage, m_showImageLabel);
    KyInfo() << "percentagedouble = " << percentagedouble;
    g_user_signal->toolbarPercentageChanged();

    g_sane_object->normalImagePath = loadPath;
}

void ShowImageWidget::saveImage(QString filename)
{
    KyInfo() << "Save filename: " << filename;
    QImage *img = NULL;
    img = imageSave(filename);

    if (img) {
        saveToPdf(*img, filename);
    }
}

void ShowImageWidget::cropSlot()
{
    KyInfo() << "Crop begin";

    if (g_sane_object->cropFlag == 0) {
        m_cropLabel->setFixedSize(m_showImageLabel->size());
        m_showImageAndCropWidget->setCurrentWidget(m_cropLabel);

        *m_editImage = m_normalImage->copy();

        *m_stackImage = m_normalImage->copy();
        m_imageStack.push(*m_stackImage);

        setPixmapScaled(*m_editImage, m_cropLabel);
    }

}

void ShowImageWidget::rotateSlot()
{
    KyInfo() << "Rotate begin";

    QMatrix matrix;
    *m_stackImage = m_normalImage->copy();
    m_imageStack.push(*m_stackImage);

    matrix.rotate(270);
    *m_editImage = m_normalImage->transformed(matrix);
    setPixmapScaled(*m_editImage, m_showImageLabel);
    *m_normalImage = m_editImage->copy();
    setPixmapScaled(*m_normalImage, m_showImageLabel);

}

void ShowImageWidget::mirrorSlot()
{
    KyInfo() << "Mirror begin";
    *m_stackImage = m_normalImage->copy();
    m_imageStack.push(*m_stackImage);

    *m_editImage = m_normalImage->mirrored(true, false);

    setPixmapScaled(*m_editImage, m_showImageLabel);

    *m_normalImage = m_editImage->copy();
    setPixmapScaled(*m_normalImage, m_showImageLabel);

}

void ShowImageWidget::watermarkSlot()
{
    KyInfo() << "Watermark begin";

    WatermarkDialog *dialog = new WatermarkDialog();

    QWidget *widget = nullptr;
    QWidgetList widgetList = QApplication::allWidgets();
    for (int i=0; i<widgetList.length(); ++i) {
        if (widgetList.at(i)->objectName() == "MainWindow") {
            widget = widgetList.at(i);
        }
    }
    if (widget) {
        QRect rect = widget->geometry();
        int x = rect.x() + rect.width()/2 - dialog->width()/2;
        int y = rect.y() + rect.height()/2 - dialog->height()/2;
        dialog->move(x,y);
    }

    int ret = dialog->exec();
    KyInfo() << "ret = " << ret;

    if (ret == QDialog::Accepted) {
        QString text = dialog->getLineEditText();
        KyInfo() << text;

        *m_stackImage= m_normalImage->copy();
        m_imageStack.push(*m_stackImage);

        *m_editImage = m_normalImage->copy();

        QPainter painter(m_editImage);
        int fontSize = 60;
        int spacing = 20;
        QFont font("华文黑体", fontSize, QFont::Thin);

        font.setLetterSpacing(QFont::AbsoluteSpacing, spacing);
        painter.setFont(font);
        painter.setPen(QColor(47, 44, 43));
        painter.rotate(15);

        int squareEdgeSize = m_editImage->width() * sin(45) + m_editImage->height() * sin( 45);
        int hCount = squareEdgeSize / ((fontSize + spacing) * (text.size() + 1)) + 1;
        int x = squareEdgeSize / hCount + (fontSize + spacing) * 3;
        int y = x / 2;

        for (int i = 0; i < hCount; ++i) {
            for (int j = 0; j < hCount * 2; ++j) {
                painter.drawText(x * i, y * j, text);
                KyInfo() << "drawtext: " << text;
            }
        }
        setPixmapScaled(*m_editImage, m_showImageLabel);

        *m_normalImage = m_editImage->copy();
        setPixmapScaled(*m_normalImage, m_showImageLabel);
    }

    delete dialog;
}

void ShowImageWidget::zoomoutNormalImage()
{
    QString proportionNoPercentage = g_sane_object->percentage.replace("%", "");

    double percentageDouble = proportionNoPercentage.toDouble() / 100 - 0.1;
    KyInfo() << "percentageDouble = " << percentageDouble
             << "proportionNoPercentage = " << proportionNoPercentage;

    KyInfo() << "percentageDouble = " << percentageDouble;

    if (percentageDouble < 0.2) {
        percentageDouble = 0.2;
    }

    setPixmapScaledByProportion(percentageDouble);

    int percentageInt = static_cast<int>(percentageDouble * 100);

    QString percentageString = QString::number(percentageInt) + "%";
    KyInfo() << "percentage: " << percentageString;

    g_sane_object->percentage = percentageString;

    g_user_signal->toolbarPercentageChanged();
}

void ShowImageWidget::zoominNormalImage()
{
    QString proportionNoPercentage = g_sane_object->percentage.replace("%", "");

    double percentageDouble = proportionNoPercentage.toDouble() / 100 + 0.1;
    KyInfo() << "percentageDouble = " << percentageDouble
             << "proportionNoPercentage = " << proportionNoPercentage;


    if (percentageDouble > 2) {
        percentageDouble = 2;
    }

    setPixmapScaledByProportion(percentageDouble);

    int percentageInt = static_cast<int>(percentageDouble * 100);

    QString percentageString = QString::number(percentageInt) + "%";
    KyInfo() << "percentage: " << percentageString;

    g_sane_object->percentage = percentageString;
    g_user_signal->toolbarPercentageChanged();
}

