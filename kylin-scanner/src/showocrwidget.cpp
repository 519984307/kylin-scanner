#include "showocrwidget.h"
#include <ukui-log4qt.h>

showOcrWidget::showOcrWidget(QWidget *parent) : QLabel(parent)
  , m_ocrImageLabel (new QLabel())
//  , m_ocrImageHLayout (new QHBoxLayout())
  , m_ocrTextEdit (new QTextEdit())
//  , m_ocrTextEditHLayout (new QHBoxLayout())
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

//    m_ocrImageHLayout->setSpacing(0);
//    m_ocrImageHLayout->addStretch();
//    m_ocrImageHLayout->addWidget(m_ocrImageLabel, 0, Qt::AlignCenter | Qt::AlignVCenter | Qt::AlignHCenter);
//    m_ocrImageHLayout->addStretch();
//    m_ocrImageHLayout->setContentsMargins(0, 0, 0, 0);

    m_ocrTextEdit->setText("The document is in character recognition ...");
    m_ocrTextEdit->setReadOnly(true);
    m_ocrTextEdit->setMinimumSize(QSize(612, 398));

//    m_ocrTextEditHLayout->setSpacing(0);
//    m_ocrTextEditHLayout->addStretch();
//    m_ocrTextEditHLayout->addWidget(m_ocrTextEdit, 0, Qt::AlignCenter | Qt::AlignVCenter |Qt::AlignHCenter);
//    m_ocrTextEditHLayout->addStretch();
//    m_ocrTextEditHLayout->setContentsMargins(0, 0, 0, 0);


    m_ocrVLayout->setSpacing(0);
    m_ocrVLayout->addSpacing(12);
//    m_ocrVLayout->addLayout(m_ocrImageHLayout);
    m_ocrVLayout->addWidget(m_ocrImageLabel, 0, Qt::AlignCenter | Qt::AlignVCenter | Qt::AlignHCenter);
    m_ocrVLayout->addSpacing(12);
//    m_ocrVLayout->addStretch();
//    m_ocrVLayout->addLayout(m_ocrTextEditHLayout);
    m_ocrVLayout->addWidget(m_ocrTextEdit, 0, Qt::AlignCenter | Qt::AlignVCenter |Qt::AlignHCenter);
    m_ocrVLayout->addSpacing(12);
//    m_ocrVLayout->addStretch();
    m_ocrVLayout->addWidget(m_toolbarWidget, 0, Qt::AlignVCenter | Qt::AlignCenter);
    m_ocrVLayout->addSpacing(16);
//    m_ocrVLayout->addStretch();
    m_ocrVLayout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(m_ocrVLayout);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &showOcrWidget::showScanLine);
}

void showOcrWidget::initConnect()
{
    connect(g_user_signal, &GlobalUserSignal::toolbarOcrOperationStartSignal, this, &showOcrWidget::startScanSlot);
    connect(g_user_signal, &GlobalUserSignal::toolbarOcrOperationFinishedSignal, this, &showOcrWidget::updateOcrTextEdit);

    connect(g_user_signal, &GlobalUserSignal::startScanOperationSignal, this, &showOcrWidget::stopScanStop);
    connect(g_user_signal, &GlobalUserSignal::stopOcrTimerSignal, this, &showOcrWidget::stopScanStop);
}

void showOcrWidget::resizeEvent(QResizeEvent *event)
{
    // todo: auto update size of m_ocrImageLabel, m_ocrTextEdit, m_toolbarWidget

    QWidget::resizeEvent(event);
}

void showOcrWidget::showScanLine()
{
//    KyInfo() << "showScanLine";
    scanHeight += 1;

    if (scanHeight >=m_ocrImageLabel->height()) {
        scanHeight = 0;
    }


    QPixmap pix;
    QString pixFileName = QString(ScanningPicture);

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

void showOcrWidget::startScanSlot()
{
    m_ocrTextEdit->setText("The document is in character recognition ...");
    timer->start(10);
    scanHeight = 0;

}

void showOcrWidget::stopScanStop()
{
    m_ocrTextEdit->setText("The document is in character recognition ...");
    timer->stop();
    scanHeight = 0;
}

void showOcrWidget::updateOcrTextEdit()
{
    KyInfo() << "ocrTextEdit = " << g_sane_object->ocrOutputText;
    timer->stop();
    scanHeight = 0;

    m_ocrTextEdit->setText(g_sane_object->ocrOutputText);
}
