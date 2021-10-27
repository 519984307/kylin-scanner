#include "thumbnailwidget.h"

int ThumbnailWidget::scanPictureCount = 0;

ThumbnailWidget::ThumbnailWidget(QWidget *parent) : QListView(parent)
{
    setupGui();
    initConnect();
    initSettings();
}


void ThumbnailWidget::setupGui()
{
    setFixedWidth(ThumbnailWidgetMinimumWidth);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->verticalScrollBar()->setProperty("drawScrollBarGroove",false);

    setDragEnabled(false);
    setSpacing(8);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setIconSize(ThumbnailIconSize);
    setViewMode(QListView::IconMode);
    setMovement(QListWidget::Static);

    setAcceptDrops(false);
    setFocusPolicy(Qt::NoFocus);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);


    // Draw shadow
//    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
//    effect->setOffset(0, 0);          //设置向哪个方向产生阴影效果(dx,dy)，特别地，(0,0)代表向四周发散
//    effect->setColor(ThumbnailShadowColor); //设置阴影颜色，也可以setColor(QColor(220,220,220))
//    effect->setBlurRadius(ThumbnailShadowRadiu);        //设定阴影的模糊半径，数值越大越模糊
//    this->setGraphicsEffect(effect);

}

void ThumbnailWidget::initConnect()
{
    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, [=](){
        themeChangedBlack();
    });
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, [=](){
        themeChangedWhite();
    });

    connect(g_user_signal, &GlobalUserSignal::scanThreadFinishedImageLoadSignal, [this](){
        showNormalImageAfterScan();
    });

    connect(this, &ThumbnailWidget::clicked, this, &ThumbnailWidget::clickedItemSlot);

}

void ThumbnailWidget::initSettings()
{
    showThumbnailIcon();
}


void ThumbnailWidget::showThumbnailIcon()
{
    m_thumbnailDelegate = new ThumbnailDelegate();
    this->setItemDelegate(m_thumbnailDelegate);

    m_thumbnailItemModel = new QStandardItemModel();

}

void ThumbnailWidget::themeChangedWhite()
{
    this->setStyleSheet("QListView{outline:none;background:rgba(255,255, 255, 1);border-radius: 6px;}"
                  "QListView::item{margin:0 1px 0 1px;background:rgba(255, 255, 255, 0.8);border-radius: 6px; }"
                  "QListView::item:selected{border:1px solid #3790FA;background:rgba(255, 255, 255, 0.8);}"
                  "QListView::item:hover{background:rgba(55, 144, 250, 0.5);}");
}

void ThumbnailWidget::themeChangedBlack()
{
    this->setStyleSheet("QListView{outline:none;background:rgba(38,38, 38, 1);border-radius: 6px;}"
                  "QListView::item{margin:0 1px 0 1px;background:rgba(38, 38, 38, 0.8);border-radius: 6px; }"
                  "QListView::item:selected{border:1px solid #3790FA;background:rgba(38, 38, 38, 0.8);}"
                        "QListView::item:hover{background:rgba(55, 144, 250, 0.5);}");
}

void ThumbnailWidget::showNormalImageAfterScan()
{
    QString scanFullName = g_sane_object->loadFullScanFileName;

    QString tooltipName = g_sane_object->userInfo.saveName + QString(".pnm");

    int rowcount = m_thumbnailItemModel->rowCount();
    KyInfo() << "rowcount = " << rowcount;


    ScanStandardItem *item = new ScanStandardItem();
    item->setIcon(QIcon(scanFullName));
    item->setToolTip(tooltipName);
    item->setPath(scanFullName);
    item->setRowCountLocation(rowcount);
    m_thumbnailItemModel->insertRow(rowcount, item);

    scanPictureCount = rowcount;

    this->setModel(m_thumbnailItemModel);

}

void ThumbnailWidget::clickedItemSlot(const QModelIndex &index)
{
     ScanStandardItem *item = dynamic_cast<ScanStandardItem *>(m_thumbnailItemModel->itemFromIndex(index));
     if (item) {
         int scanIconNumber = item->getRowCountLocation();
         QString loadPath = item->getPath();

         KyInfo() << "scanIconPath = " << scanIconNumber << loadPath;

         g_user_signal->showImageAfterClickedThumbnail(loadPath);

     }
}


void ScanStandardItem::setPath(const QString &path)
{
    m_path = path;
}

QString ScanStandardItem::getPath()
{
    return m_path;
}

int ScanStandardItem::getRowCountLocation() const
{
    return rowCountLocation;
}

void ScanStandardItem::setRowCountLocation(int value)
{
    rowCountLocation = value;
}
