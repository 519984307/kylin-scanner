#include "showocrwidget.h".h"
#include <ukui-log4qt.h>

showOcrWidget::showOcrWidget(QWidget *parent) : QLabel(parent)
  , m_ocrImageLabel (new QLabel())
  , m_ocrTextEdit (new QTextEdit())
  , m_toolbarWidget (new ToolBarWidget())
  , m_ocrVLayout (new QVBoxLayout())
{
    setupGui();
    initConnect();
}

void showOcrWidget::setupGui()
{
    this->setMinimumSize(QSize(644, 636));

    m_ocrImageLabel->setMinimumSize(QSize(108, 150));

    m_ocrTextEdit->setMinimumSize(QSize(612, 398));

    m_ocrVLayout->setSpacing(0);
    m_ocrVLayout->addSpacing(12);
    m_ocrVLayout->addWidget(m_ocrImageLabel, 0, Qt::AlignVCenter | Qt::AlignCenter);
    m_ocrVLayout->addSpacing(12);
    m_ocrVLayout->addStretch();
    m_ocrVLayout->addWidget(m_ocrTextEdit, 0, Qt::AlignVCenter | Qt::AlignCenter);
    m_ocrVLayout->addSpacing(12);
    m_ocrVLayout->addStretch();
    m_ocrVLayout->addWidget(m_toolbarWidget, 0, Qt::AlignVCenter | Qt::AlignCenter);
    m_ocrVLayout->addSpacing(16);
    m_ocrVLayout->addStretch();
    m_ocrVLayout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(m_ocrVLayout);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &showOcrWidget::showScanLine);
//    timer->start(10);
}

void showOcrWidget::initConnect()
{
}

void showOcrWidget::showScanLine()
{
    KyInfo() << "showScanLine";
    scanHeight += 1;

    if (scanHeight >=m_ocrImageLabel->height()) {
        scanHeight = 0;
    }


    QPixmap pix;
    QString pixFileName = QString("/home/yushuoqi/scanner01.jpg");

    QPixmap pixScan(":/scan-line.png");

    pix.load(pixFileName );
    pix = pix.scaled(m_ocrImageLabel->width(), m_ocrImageLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_ocrImageLabel->setPixmap(pix);

    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::transparent);
    painter.drawPixmap(0, scanHeight, m_ocrImageLabel->width(), 30, pixScan);

    m_ocrImageLabel->setPixmap(pix);
}

void showOcrWidget::startScan()
{
    timer->start(10);
    scanHeight = 0;

}

void showOcrWidget::stopScan()
{
    QPixmap pix;
    QString pixFileName = QString("/home/yushuoqi/scanner01.jpg");
    pix.load(pixFileName );
    pix = pix.scaled(m_ocrImageLabel->width(), m_ocrImageLabel->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_ocrImageLabel->setPixmap(pix);

    timer->stop();
}
