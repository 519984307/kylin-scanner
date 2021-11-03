#include "showimagewidget.h"
#include <ukui-log4qt.h>

ShowImageWidget::ShowImageWidget(QWidget *parent) : QWidget(parent)
  , m_editImage( new QImage())
  , m_normalImage (new QImage())
  , m_showImage (new QLabel())
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

    m_normalImage->load(scannerImagePath+ QString("/scan.pnm"));
    KyInfo() << "loadm_normalImage size: " << m_normalImage->size()
             << "showImageWidget size: " << this->size();

    m_showImage->setMinimumSize(QSize(387, 536));

    m_normalImage->scaled(m_showImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_showImage->setScaledContents(true);
    m_showImage->setPixmap(QPixmap::fromImage(*m_normalImage));

    m_showImageHLayout->setSpacing(0);
    m_showImageHLayout->addSpacing(128);
    m_showImageHLayout->addStretch();
    m_showImageHLayout->addWidget(m_showImage);
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
        QByteArray str = g_sane_object->outText.toUtf8();
        file.write(str);
        file.close();
    }
    return NULL;
}

void ShowImageWidget::showNormalImageAfterScan()
{
    QString loadPath = g_sane_object->loadFullScanFileName;
    m_normalImage->load(loadPath);

    KyInfo() << "loadPath = " << loadPath
             << "loadm_normalImage size: " << m_normalImage->size() << "showImageWidget size: " << m_normalImage->size();

    m_normalImage->scaled(m_showImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_showImage->setScaledContents(true);
    m_showImage->setPixmap(QPixmap::fromImage(*m_normalImage));

    QString savePath = g_sane_object->saveFullScanFileName;
    KyInfo() << "savePath = " << savePath;

    saveImage(savePath);
}

void ShowImageWidget::showImageAfterClickedThumbnail(QString loadPath)
{
    KyInfo() << "loadPath = " << loadPath;
    m_normalImage->load(loadPath);
    m_normalImage->scaled(m_showImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_showImage->setScaledContents(true);
    m_showImage->setPixmap(QPixmap::fromImage(*m_normalImage));
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

