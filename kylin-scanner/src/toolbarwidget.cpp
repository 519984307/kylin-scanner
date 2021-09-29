#include "toolbarwidget.h"

ToolBarWidget::ToolBarWidget(QWidget *parent) : QWidget(parent),
    m_beautyButton(new QPushButton),
    m_rectifyButton(new QPushButton),
    m_ocrButton(new QPushButton),
    m_leftFrame(new QFrame),
    m_cropButton(new QPushButton),
    m_rotateButton(new QPushButton),
    m_mirrorHButton(new QPushButton),
    m_watermarkButton(new QPushButton),
    m_rightFrame(new QFrame),
    m_zoomOutButton(new QPushButton),
    m_percentageLabel(new QLabel),
    m_zoomInButton(new QPushButton),
    m_mainHLayout(new QHBoxLayout(this))
{
    setupGui();
    initConnect();
}

void ToolBarWidget::setupGui()
{
    this->setAttribute(Qt::WA_TranslucentBackground); // set background translucent，combine with overriding paintEvent to let setStyleSheet work
    this->setWindowFlags(Qt::FramelessWindowHint);   // set frameless window
    this->setAutoFillBackground(true);

    this->setMinimumSize(ToolBarWidgetFixedSize);

    m_beautyButton->setFixedSize(ToolBarWidgetButtonSize);
    m_beautyButton->setIconSize(ToolBarWidgetButtonSize);

    m_rectifyButton->setFixedSize(ToolBarWidgetButtonSize);
    m_rectifyButton->setIconSize(ToolBarWidgetButtonSize);

    m_ocrButton->setFixedSize(ToolBarWidgetButtonSize);
    m_ocrButton->setIconSize(ToolBarWidgetButtonSize);

    m_leftFrame->setFixedSize(ToolBarWidgetFrameSize);

    m_cropButton->setFixedSize(ToolBarWidgetButtonSize);
    m_cropButton->setIconSize(ToolBarWidgetButtonSize);

    m_rotateButton->setFixedSize(ToolBarWidgetButtonSize);
    m_rotateButton->setIconSize(ToolBarWidgetButtonSize);

    m_mirrorHButton->setFixedSize(ToolBarWidgetButtonSize);
    m_mirrorHButton->setIconSize(ToolBarWidgetButtonSize);

    m_watermarkButton->setFixedSize(ToolBarWidgetButtonSize);
    m_watermarkButton->setIconSize(ToolBarWidgetButtonSize);

    m_rightFrame->setFixedSize(ToolBarWidgetFrameSize);

    QFont m_percentageFontSize;
    m_percentageFontSize.setPixelSize(12);
    m_percentageLabel->setText("100%");
    m_percentageLabel->setFont(m_percentageFontSize);

    m_zoomOutButton->setFixedSize(ToolBarWidgetZoomButtonSize);
    m_zoomOutButton->setIconSize(ToolBarWidgetZoomButtonSize);

    m_zoomInButton->setFixedSize(ToolBarWidgetZoomButtonSize);
    m_zoomInButton->setIconSize(ToolBarWidgetZoomButtonSize);


    m_mainHLayout->setSpacing(0);
    m_mainHLayout->addWidget(m_beautyButton);
    m_mainHLayout->addSpacing(4);
    m_mainHLayout->addWidget(m_rectifyButton);
    m_mainHLayout->addSpacing(4);
    m_mainHLayout->addWidget(m_ocrButton);
    m_mainHLayout->addSpacing(8);
    m_mainHLayout->addWidget(m_leftFrame);
    m_mainHLayout->addSpacing(4);
    m_mainHLayout->addWidget(m_cropButton);
    m_mainHLayout->addSpacing(4);
    m_mainHLayout->addWidget(m_rotateButton);
    m_mainHLayout->addSpacing(4);
    m_mainHLayout->addWidget(m_mirrorHButton);
    m_mainHLayout->addSpacing(3);
    m_mainHLayout->addWidget(m_watermarkButton);
    m_mainHLayout->addSpacing(8);
    m_mainHLayout->addWidget(m_rightFrame);
    m_mainHLayout->addSpacing(16);
    m_mainHLayout->addWidget(m_zoomOutButton, 0, Qt::AlignCenter);
    m_mainHLayout->addSpacing(6);
    m_mainHLayout->addWidget(m_percentageLabel, 0, Qt::AlignCenter);
    m_mainHLayout->addSpacing(4);
    m_mainHLayout->addWidget(m_zoomInButton, 0, Qt::AlignCenter);
    m_mainHLayout->setContentsMargins(8, 0, 16, 0);

    this->setLayout(m_mainHLayout);

    KyInfo() << "toolbarwidget size: " << this->size();

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(0, 0);          // set direction of shadow effect, especially (0,0) is on behave of spread around
    effect->setColor(QColor(220, 220, 220)); // set shadow color
    effect->setBlurRadius(20);        // set blur of shadow, which means the large the radius, the more blurred
    this->setGraphicsEffect(effect);

}

void ToolBarWidget::initConnect()
{
    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, [=](){
        themeChangedBlackSlot();
    });
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, [=](){
        themeChangedWhiteSlot();
    });

}

void ToolBarWidget::themeChangedIconBlackSettings()
{
    m_beautyButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/beauty.svg", "white")));
    m_rectifyButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/rectify.svg", "white")));
    m_ocrButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/ocr.svg", "white")));
    m_cropButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/crop.svg", "white")));
    m_rotateButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/rotate.svg", "white")));
    m_mirrorHButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/mirror.svg", "white")));
    m_watermarkButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/watermark.svg", "white")));
    m_zoomOutButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/zoomout.svg", "white")));
    m_zoomInButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/zoomin.svg", "white")));
}

void ToolBarWidget::themeChangedIconWhiteSettings()
{
    m_beautyButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/beauty.svg", "black")));
    m_rectifyButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/rectify.svg", "black")));
    m_ocrButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/ocr.svg", "black")));
    m_cropButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/crop.svg", "black")));
    m_rotateButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/rotate.svg", "black")));
    m_mirrorHButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/mirror.svg", "black")));
    m_watermarkButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/watermark.svg", "black")));
    m_zoomOutButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/zoomout.svg", "black")));
    m_zoomInButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/zoomin.svg", "black")));

}

/**
 * @brief ToolBarWidget::paintEvent
 * @param event
 * Basically, it only support these stylesheet attributes for QWidget : background, background-clip and background-origin
 */
void ToolBarWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QWidget::paintEvent(event);

#if 0
    // green background
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::green));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width());
    rect.setHeight(rect.height());
    painter.drawRoundedRect(rect, 6, 6);
#endif

#if 0
    // follow system window color
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
#endif
}

void ToolBarWidget::themeChangedWhiteSlot()
{
    KyInfo() << "white theme";

    themeChangedIconWhiteSettings();

    this->setStyleSheet("background: rgba(249, 249, 249, 0.8); box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 6px; filter: blur(18px);");

    m_leftFrame->setStyleSheet("background: #B3B3B3; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15);");
    m_rightFrame->setStyleSheet("background: #B3B3B3; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15);");

    m_beautyButton->setStyleSheet("QPushButton{background: rgba(249, 249, 249, 0.8); border-radius: 6px;}"
                                  "QPushButton::pressed{background:#0079ed; border-radius: 6px;}"
                                  "QPushButton::hover{background:#0079ed; border-radius: 6px;}"
                                  );

    m_rectifyButton->setStyleSheet("background:transparent; border-radius: 6px;");
    m_ocrButton->setStyleSheet("background:transparent; border-radius: 6px;");
    m_cropButton->setStyleSheet("background:transparent;  border-radius: 6px;");
    m_rotateButton->setStyleSheet("background:transparent; border-radius: 6px;");
    m_mirrorHButton->setStyleSheet("background:transparent; border-radius: 6px;");
    m_watermarkButton->setStyleSheet("background:transparent; border-radius: 6px;");


    m_zoomOutButton->setStyleSheet("background: #FFFFFF; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 10px;");
    m_percentageLabel->setStyleSheet("width: 32px; height: 18px; color: #262626;");
    m_zoomOutButton->setStyleSheet("background: #FFFFFF; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 10px;");
}

void ToolBarWidget::themeChangedBlackSlot()
{
    themeChangedIconBlackSettings();

    this->setStyleSheet("background: rgba(38, 38, 38, 1); box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 6px; filter: blur(18px);");
    m_leftFrame->setStyleSheet("background: #B3B3B3; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15);");
    m_rightFrame->setStyleSheet("background: #B3B3B3; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15);");

    m_beautyButton->setStyleSheet("QPushButton{ border-radius: 6px;}"
                                  "QPushButton::pressed{background:#0079ed; border-radius: 6px;}"
                                  "QPushButton::hover{background:#0079ed; border-radius: 6px;}"
                                  );

    m_zoomOutButton->setStyleSheet("background: #373738; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 10px;");
    m_percentageLabel->setStyleSheet("width: 32px; height: 18px; color: #D9D9D9;");
    m_zoomOutButton->setStyleSheet("background: #373738; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 10px;");

}
