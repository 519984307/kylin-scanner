#include "showimagewidget.h"
#include <ukui-log4qt.h>

ShowImageWidget::ShowImageWidget(QWidget *parent) :
    QLabel(parent),
    scannerImagePath(g_config_signal->scannerImagePath)
{
    setupGui();
    initConnect();
}

void ShowImageWidget::setupGui()
{
    setMinimumSize(ShowImageWidgetMinimumSize);

    m_normalImage = new QImage();
    m_normalImage->load(scannerImagePath+ QString("/scan.pnm"));
    KyInfo() << "loadm_normalImage size: " << m_normalImage->size()
             << "showImageWidget size: " << this->size();

    m_normalImage->scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->setScaledContents(true);
    this->setPixmap(QPixmap::fromImage(*m_normalImage));

    this->setContextMenuPolicy(Qt::ActionsContextMenu);
    this->setContextMenuPolicy(Qt::CustomContextMenu);


#if 0
    m_imageSaveAction = new QAction(QIcon("/home/yushuoqi/scanner01.jpg"), "save", this);
    m_imageMenu = new QMenu("ImageMenu", this);
    m_imageMenu->addAction(m_imageSaveAction);
#endif

}

void ShowImageWidget::initConnect()
{
    connect(g_user_signal, &GlobalUserSignal::scanThreadFinishedImageLoadSignal, this, &ShowImageWidget::showNormalImageAfterScan);

#if 0
    connect(this, &ShowImageWidget::customContextMenuRequested, [=](const QPoint &pos){
        Q_UNUSED(pos);
        m_imageMenu->exec(QCursor::pos());
    });
#endif

    connect(g_user_signal, &GlobalUserSignal::showImageAfterClickedThumbnailSignal, this, &ShowImageWidget::showImageAfterClickedThumbnail);

}

void ShowImageWidget::showNormalImageAfterScan()
{
    QString loadPath = g_sane_object->loadFullScanFileName;
    m_normalImage->load(loadPath);

    KyInfo() << "loadPath = " << loadPath
             << "loadm_normalImage size: " << m_normalImage->size() << "showImageWidget size: " << this->size();

    m_normalImage->scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->setScaledContents(true);
    this->setPixmap(QPixmap::fromImage(*m_normalImage));

}

void ShowImageWidget::showImageAfterClickedThumbnail(QString loadPath)
{
    KyInfo() << "loadPath = " << loadPath;
    m_normalImage->load(loadPath);

    m_normalImage->scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->setScaledContents(true);
    this->setPixmap(QPixmap::fromImage(*m_normalImage));
}

