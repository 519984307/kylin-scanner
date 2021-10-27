#include "failedpagewidget.h"
#include <ukui-log4qt.h>

FailedPageWidget::FailedPageWidget(QWidget *parent) :
    QWidget(parent),
    m_failedPageIcon(new QLabel()),
    m_failedPageText(new QLabel()),
    m_failedPageButton(new QPushButton()),
    m_failedPageVLayout(new QVBoxLayout())
{
    setupGui();
    initConnect();
}

void FailedPageWidget::setupGui()
{
    m_failedPageIcon->setPixmap(QPixmap(":/default-connect-page/detect-failed.svg"));
    m_failedPageIcon->setFixedSize(FailedPageIconSize);

    m_failedPageText->setEnabled(false);
    m_failedPageText->setText(tr("Not detect scanners, please connect scanners firstly!"));

    m_failedPageButton->setText(tr("Connect"));
    m_failedPageButton->setFixedSize(FailedPageButtonSize);
//    m_failedPageButton->setCursor(Qt::PointingHandCursor);

    m_failedPageVLayout->setSpacing(0);
    m_failedPageVLayout->addStretch();
    m_failedPageVLayout->addWidget(m_failedPageIcon, 0,  Qt::AlignCenter);
    m_failedPageVLayout->addSpacing(13);
    m_failedPageVLayout->addWidget(m_failedPageText, 0,  Qt::AlignCenter);
    m_failedPageVLayout->addSpacing(29);
    m_failedPageVLayout->addWidget(m_failedPageButton, 0,  Qt::AlignCenter);
    m_failedPageVLayout->addStretch();
    m_failedPageVLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_failedPageVLayout);

}

void FailedPageWidget::initConnect()
{
    connect(m_failedPageButton, &QPushButton::clicked, this, [=](){
        emit scanButtonClicked();
    });
}

