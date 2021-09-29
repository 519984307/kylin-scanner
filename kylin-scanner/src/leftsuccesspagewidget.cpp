/*
* Copyright (C) 2021, KylinSoft Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/
#include "leftsuccesspagewidget.h"

LeftSuccessPageWidget::LeftSuccessPageWidget(QWidget *parent) :
    QWidget(parent),
    m_connectSuccessIcon(new QLabel(this)),
    m_connectSuccessText(new QLabel(this)),
    m_connectSuccessVLayout(new QVBoxLayout(this))
{
    setupGui();
}

void LeftSuccessPageWidget::setupGui()
{
    setMinimumSize(LeftSuccessPageWidth, LeftSuccessPageHeight);
    m_connectSuccessIcon->setPixmap(QPixmap(":/default-connect-page/detect-success.svg"));
    m_connectSuccessIcon->setFixedSize(LeftSuccessPageIconSize);

    m_connectSuccessText->setEnabled(false);
    m_connectSuccessText->setText(tr("Connect scanners, please click scan button to start scanning."));

    m_connectSuccessVLayout->setSpacing(0);
    m_connectSuccessVLayout->addStretch();
    m_connectSuccessVLayout->addWidget(m_connectSuccessIcon, 0,  Qt::AlignCenter | Qt::AlignHCenter | Qt::AlignVCenter);
    m_connectSuccessVLayout->addSpacing(13);
    m_connectSuccessVLayout->addWidget(m_connectSuccessText, 0,  Qt::AlignCenter | Qt::AlignHCenter | Qt::AlignVCenter);
    m_connectSuccessVLayout->addStretch();
    m_connectSuccessVLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_connectSuccessVLayout);

}
