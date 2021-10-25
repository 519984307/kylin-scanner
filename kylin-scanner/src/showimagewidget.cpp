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


    m_imageSaveAction = new QAction(QIcon("/home/yushuoqi/scanner01.jpg"), "save", this);
    m_imageMenu = new QMenu("ImageMenu", this);
    m_imageMenu->addAction(m_imageSaveAction);

}

void ShowImageWidget::initConnect()
{
    connect(g_user_signal, &GlobalUserSignal::scanThreadFinishedImageLoadSignal, [this](){
        showNormalImageAfterScan();
    });
    connect(this, &ShowImageWidget::customContextMenuRequested, [=](const QPoint &pos){
        Q_UNUSED(pos);
        m_imageMenu->exec(QCursor::pos());
    });

}

void ShowImageWidget::showNormalImageAfterScan()
{
    m_normalImage->load(scannerImagePath + QString("/scan.pnm"));
    KyInfo() << "loadm_normalImage size: " << m_normalImage->size()
             << "showImageWidget size: " << this->size();

    m_normalImage->scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->setScaledContents(true);
    this->setPixmap(QPixmap::fromImage(*m_normalImage));

}
