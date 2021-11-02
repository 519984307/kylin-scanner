#include "toolbarwidget.h"

ToolBarWidget::ToolBarWidget(QWidget *parent) : QWidget(parent),
    m_beautyButton(new QPushButton),
    m_rectifyButton(new QPushButton),
    m_ocrButton(new QPushButton),
    m_leftFrame(new QFrame),
    m_cropButton(new QPushButton),
    m_rotateButton(new QPushButton),
    m_mirrorButton(new QPushButton),
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
    m_beautyButton->setToolTip(tr("Beauty"));

    m_rectifyButton->setFixedSize(ToolBarWidgetButtonSize);
    m_rectifyButton->setIconSize(ToolBarWidgetButtonSize);
    m_rectifyButton->setToolTip(tr("Rectify"));

    m_ocrButton->setFixedSize(ToolBarWidgetButtonSize);
    m_ocrButton->setIconSize(ToolBarWidgetButtonSize);
    m_ocrButton->setToolTip(tr("OCR"));

    m_leftFrame->setFixedSize(ToolBarWidgetFrameSize);

    m_cropButton->setFixedSize(ToolBarWidgetButtonSize);
    m_cropButton->setIconSize(ToolBarWidgetButtonSize);
    m_cropButton->setToolTip(tr("Crop"));

    m_rotateButton->setFixedSize(ToolBarWidgetButtonSize);
    m_rotateButton->setIconSize(ToolBarWidgetButtonSize);
    m_rotateButton->setToolTip(tr("Rotate"));

    m_mirrorButton->setFixedSize(ToolBarWidgetButtonSize);
    m_mirrorButton->setIconSize(ToolBarWidgetButtonSize);
    m_mirrorButton->setToolTip(tr("Mirror"));

    m_watermarkButton->setFixedSize(ToolBarWidgetButtonSize);
    m_watermarkButton->setIconSize(ToolBarWidgetButtonSize);
    m_watermarkButton->setToolTip(tr("Watermark"));

    m_rightFrame->setFixedSize(ToolBarWidgetFrameSize);

    QFont m_percentageFontSize;
    m_percentageFontSize.setPixelSize(12);
    m_percentageLabel->setText("100%");
    m_percentageLabel->setFont(m_percentageFontSize);

    m_zoomOutButton->setFixedSize(ToolBarWidgetZoomButtonSize);
    m_zoomOutButton->setIconSize(ToolBarWidgetZoomButtonSize);
    m_zoomOutButton->setToolTip(tr("ZoomOut"));

    m_zoomInButton->setFixedSize(ToolBarWidgetZoomButtonSize);
    m_zoomInButton->setIconSize(ToolBarWidgetZoomButtonSize);
    m_zoomInButton->setToolTip(tr("ZoomIn"));


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
    m_mainHLayout->addWidget(m_mirrorButton);
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
    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, &ToolBarWidget::themeChangedBlackSlot);
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, &ToolBarWidget::themeChangedWhiteSlot);

    connect(m_beautyButton, &QPushButton::clicked, this, &ToolBarWidget::beautyButtonClickedSlot);
    connect(m_rectifyButton, &QPushButton::clicked, this, &ToolBarWidget::rectifyButtonClickedSlot);
    connect(m_ocrButton, &QPushButton::clicked, this, &ToolBarWidget::ocrButtonClickedSlot);
    connect(m_cropButton, &QPushButton::clicked, this, &ToolBarWidget::cropButtonClickedSlot);
    connect(m_rotateButton, &QPushButton::clicked, this, &ToolBarWidget::rotateButtonClickedSlot);
    connect(m_mirrorButton, &QPushButton::clicked, this, &ToolBarWidget::mirrorButtonClickedSlot);
    connect(m_watermarkButton, &QPushButton::clicked, this, &ToolBarWidget::watermarkButtonClickedSlot);
}

void ToolBarWidget::themeChangedIconBlackSettings()
{
    m_zoomOutButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/zoomout.svg", "white")));
    m_zoomInButton->setIcon(QIcon(SVGHandler::loadSvgColor(":/toolbar/zoomin.svg", "white")));
}

void ToolBarWidget::themeChangedIconWhiteSettings()
{
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

}

void ToolBarWidget::themeChangedWhiteSlot()
{
    KyInfo() << "white theme";

    themeChangedIconWhiteSettings();

    this->setStyleSheet("background: rgba(249, 249, 249, 0.8); box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 6px; filter: blur(18px);");

    m_leftFrame->setStyleSheet("background: #B3B3B3; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15);");
    m_rightFrame->setStyleSheet("background: #B3B3B3; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15);");


    m_beautyButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/beauty.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/beauty-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/beauty-blue.svg);}");

    m_rectifyButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rectify.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rectify-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rectify-blue.svg);}");

    m_ocrButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/ocr.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/ocr-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/ocr-blue.svg);}");

    m_cropButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/crop.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/crop-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/crop-blue.svg);}");

    m_rotateButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rotate.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rotate-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rotate-blue.svg);}");

    m_mirrorButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/mirror.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/mirror-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/mirror-blue.svg);}");

    m_watermarkButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/watermark.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/watermark-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/watermark-blue.svg);}");


    m_zoomOutButton->setStyleSheet("background: #FFFFFF; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 10px;");
    m_percentageLabel->setStyleSheet("width: 32px; height: 18px; color: #262626;");
    m_zoomInButton->setStyleSheet("background: #FFFFFF; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 10px;");
}

void ToolBarWidget::themeChangedBlackSlot()
{
    themeChangedIconBlackSettings();

    this->setStyleSheet("background: rgba(38, 38, 38, 1); box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 6px; filter: blur(18px);");
    m_leftFrame->setStyleSheet("background: #B3B3B3; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15);");
    m_rightFrame->setStyleSheet("background: #B3B3B3; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15);");

    m_beautyButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/beauty-white.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/beauty-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/beauty-blue.svg);}");

    m_rectifyButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rectify-white.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rectify-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rectify-blue.svg);}");

    m_ocrButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/ocr-white.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/ocr-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/ocr-blue.svg);}");

    m_cropButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/crop-white.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/crop-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/crop-blue.svg);}");

    m_rotateButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rotate-white.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rotate-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/rotate-blue.svg);}");

    m_mirrorButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/mirror-white.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/mirror-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/mirror-blue.svg);}");

    m_watermarkButton->setStyleSheet("QPushButton{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/watermark-white.svg);}"
                                   "QPushButton::hover{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/watermark-blue.svg);}"
                                   "QPushButton::pressed{border:0px; border-radius: 6px; background:transparent; background-image:url(:/toolbar/watermark-blue.svg);}");



    m_zoomOutButton->setStyleSheet("background: #373738; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 10px;");
    m_percentageLabel->setStyleSheet("width: 32px; height: 18px; color: #D9D9D9;");
    m_zoomInButton->setStyleSheet("background: #373738; box-shadow: 0px 0px 5px 0px rgba(0, 0, 0, 0.15); border-radius: 10px;");

}

void ToolBarWidget::beautyButtonClickedSlot()
{
    g_user_signal->toolbarBeautyOperation();
}

void ToolBarWidget::rectifyButtonClickedSlot()
{
    g_user_signal->toolbarRectifyOperation();
}

void ToolBarWidget::ocrButtonClickedSlot()
{
    g_user_signal->toolbarOcrOperation();
}

void ToolBarWidget::cropButtonClickedSlot()
{
    g_user_signal->toolbarCropOperation();
}

void ToolBarWidget::rotateButtonClickedSlot()
{
    g_user_signal->toolbarRotateOperation();
}

void ToolBarWidget::mirrorButtonClickedSlot()
{
    g_user_signal->toolbarMirrorOperation();
}

void ToolBarWidget::watermarkButtonClickedSlot()
{
    g_user_signal->toolbarWatermarkOperation();
}
