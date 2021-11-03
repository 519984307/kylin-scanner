#include "showimagewidget.h"
#include <ukui-log4qt.h>

ShowImageWidget::ShowImageWidget(QWidget *parent) : QWidget(parent)
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
}

void ShowImageWidget::showImageAfterClickedThumbnail(QString loadPath)
{
    KyInfo() << "loadPath = " << loadPath;
    m_normalImage->load(loadPath);
    m_normalImage->scaled(m_showImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    m_showImage->setScaledContents(true);
    m_showImage->setPixmap(QPixmap::fromImage(*m_normalImage));
}

