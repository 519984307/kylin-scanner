#include "showimagewidget.h"
#include <ukui-log4qt.h>

ShowImageWidget::ShowImageWidget(QWidget *parent) : QLabel(parent)
{
    setupGui();
    initConnect();
}

void ShowImageWidget::setupGui()
{
    setMinimumSize(ShowImageWidgetMinimumSize);


    QPixmap *pixmap = new QPixmap("/home/yushuoqi/scanner01.jpg");
    KyInfo() << "loadPixmap size: " << pixmap->size()
             << "showImageWidget size: " << this->size();

    pixmap->scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->setScaledContents(true);
    this->setPixmap(*pixmap);

    this->setContextMenuPolicy(Qt::ActionsContextMenu);
    this->setContextMenuPolicy(Qt::CustomContextMenu);


//    m_imageSaveAction = new QAction(QIcon("/home/yushuoqi/scanner01.jpg"), "save", this);
//    m_imageMenu = new QMenu("ImageMenu", this);
//    m_imageMenu->addAction(m_imageSaveAction);

}

void ShowImageWidget::initConnect()
{
//    connect(this, &ShowImageWidget::customContextMenuRequested, [=](const QPoint &pos){
//        Q_UNUSED(pos);
//        m_imageMenu->exec(QCursor::pos());
//    });

}
