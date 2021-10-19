#include "thumbnailwidget.h"

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

    setDragEnabled(false); // Not drag
    setSpacing(8); // Icon spacing
    setEditTriggers(QAbstractItemView::NoEditTriggers); // No edit
    setIconSize(ThumbnailIconSize); // Icon size
    setViewMode(QListView::IconMode);
    setMovement(QListWidget::Static); // Cannot move

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

}

void ThumbnailWidget::initSettings()
{
    showThumbnailIcon();
}

#if 0
void ThumbnailWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect(), 0, 0);
    QStyleOption opt;
    opt.init(this);

    QColor mainColor;
    mainColor = opt.palette.color(QPalette::Window);

    p.fillPath(rectPath,QBrush(mainColor));

}
#endif


void ThumbnailWidget::showThumbnailIcon()
{
    m_thumbnailDelegate = new ThumbnailDelegate();
    this->setItemDelegate(m_thumbnailDelegate);

    QStandardItemModel *m_thumbnailItemModel = new QStandardItemModel();
#if 1

    for (int i=0; i< 20; i++){
        QStandardItem *item = new QStandardItem();
        item->setIcon(QIcon("/home/yushuoqi/scanner01.jpg"));
        //item->setSizeHint(QSize(38, 38));
        m_thumbnailItemModel->insertRow(i, item);
    }
        QStandardItem *item = new QStandardItem();
        item->setIcon(QIcon("/home/yushuoqi/out1.pnm"));
        m_thumbnailItemModel->insertRow(20, item);
#endif

    this->setModel(m_thumbnailItemModel);
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
