#include "scandialog.h"
#include "./utils/xatom-helper.h"
#include "globalsignal.h"

ScanDialog::ScanDialog(QWidget *parent) : QDialog(parent),
    m_titleLabel(new QLabel()),
    m_closeButton(new QPushButton()),
    m_iconLabel(new QLabel()),
    m_msgLabel(new QLabel()),
    m_cancelButton(new QPushButton()),
    m_titleHBoxLayout(new QHBoxLayout()),
    m_msgHBoxLayout(new QHBoxLayout()),
    m_cancelHBoxLayout(new QHBoxLayout()),
    m_mainVBoxLayout(new QVBoxLayout())
{
    setupGui();
    initConnect();
}

void ScanDialog::setupGui()
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowModality(Qt::ApplicationModal);
    setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    this->setWindowTitle(tr("Scanner"));
    this->setFixedSize(ScanDialogWindowSize);

    m_titleLabel->setText(tr("Scanner"));

    m_closeButton->setIcon (QIcon::fromTheme ("window-close-symbolic"));
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setIconSize (QSize(16, 16));
    m_closeButton->setProperty("isWindowButton", 0x2);
    m_closeButton->setProperty("useIconHighlightEffect", 0x8);
    m_closeButton->setFlat(true);

    m_titleHBoxLayout->addSpacing(12);
    m_titleHBoxLayout->addWidget(m_titleLabel);
    m_titleHBoxLayout->addStretch();
    m_titleHBoxLayout->addWidget(m_closeButton);
    m_titleHBoxLayout->setContentsMargins(0, 4, 4, 4);
    m_titleHBoxLayout->setAlignment(Qt::AlignCenter);

    m_iconLabel->setPixmap(QPixmap(":/scanIcon.svg"));
    m_iconLabel->setFixedSize(ScanDialogIconSize);

    m_pageNumber = 0;
    m_scanMsg = tr("Number of pages being scanned: ") + QString::number(m_pageNumber);
    m_msgLabel->setText(m_scanMsg);

    m_msgHBoxLayout->addWidget(m_iconLabel);
    m_msgHBoxLayout->addSpacing(8);
    m_msgHBoxLayout->addWidget(m_msgLabel);
    m_msgHBoxLayout->addStretch();
    m_msgHBoxLayout->setContentsMargins(24, 0, 0, 0);

    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->setFixedSize(ScanDialogButtonSize);


    m_cancelHBoxLayout->addWidget(m_cancelButton);
    m_cancelHBoxLayout->setContentsMargins(260, 0, 24, 24);

    m_mainVBoxLayout->addLayout(m_titleHBoxLayout);
    m_mainVBoxLayout->addSpacing(17);
    m_mainVBoxLayout->addLayout(m_msgHBoxLayout);
    m_mainVBoxLayout->addSpacing(23);
    m_mainVBoxLayout->addLayout(m_cancelHBoxLayout);
    m_mainVBoxLayout->setContentsMargins(0, 0, 0, 0);


    this->setLayout(m_mainVBoxLayout);

}

void ScanDialog::initConnect()
{
    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, [=](){
        this->setStyleSheet("background: #FFFFFF;");
    });
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, [=](){
        this->setStyleSheet("background: #262626;");
    });

    connect(m_closeButton, &QPushButton::clicked, this, [=](){
        this->reject();
        g_user_signal->stopScanOperation();
    });

    connect(m_cancelButton, &QPushButton::clicked, this, [=]{
        this->reject();
        g_user_signal->stopScanOperation();
    });

    connect(g_user_signal, &GlobalUserSignal::scanThreadFinishedSignal, this, &ScanDialog::updatePageNumberWhileScanning);
    connect(g_user_signal, &GlobalUserSignal::stopScanOperationSignal, this, &ScanDialog::updatePageNumberWhileStopScanning);


}

int ScanDialog::pageNumber() const
{
    return m_pageNumber;
}

void ScanDialog::setPageNumber(int pageNumber)
{
    m_pageNumber = pageNumber;
}

QString ScanDialog::scanMsg() const
{
    return m_scanMsg;
}

void ScanDialog::setScanMsg(const QString &scanMsg)
{
    m_scanMsg = scanMsg;
}

void ScanDialog::updatePageNumberWhileScanning()
{
    m_pageNumber += 1;
    m_scanMsg = tr("Number of pages being scanned: ") + QString::number(m_pageNumber);
    m_msgLabel->setText(m_scanMsg);
}

void ScanDialog::updatePageNumberWhileStopScanning()
{
    m_pageNumber = 0;
}
