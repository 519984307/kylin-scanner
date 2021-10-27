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
#include "displaywidget.h"
#include <ukui-log4qt.h>

DisplayWidget::DisplayWidget(QWidget *parent) :
    QWidget(parent),
    m_defaultConnectDetectPageWidget(new DetectPageWidget()),
    m_defaultConnectFailedPageWidget(new FailedPageWidget()),
    m_defaultConnectSuccessPageWidget(new SuccessPageWidget()),
    m_displayStackedLayout(new QStackedLayout(this))
{
    setupGui();
    initConnect();
}

void DisplayWidget::setupGui()
{
    m_displayStackedLayout->addWidget(m_defaultConnectDetectPageWidget);
    m_displayStackedLayout->addWidget(m_defaultConnectFailedPageWidget);
    m_displayStackedLayout->addWidget(m_defaultConnectSuccessPageWidget);
    m_displayStackedLayout->setContentsMargins(0, 0, 0, 0);
    m_displayStackedLayout->setCurrentWidget(m_defaultConnectDetectPageWidget);
}

void DisplayWidget::initConnect()
{
    connect(m_defaultConnectFailedPageWidget, &FailedPageWidget::scanButtonClicked, this, &DisplayWidget::showDetectPageSlot);
}

void DisplayWidget::showDetectPageSlot()
{
    m_displayStackedLayout->setCurrentWidget(m_defaultConnectDetectPageWidget);

    emit detectScanDevicesSignal();
}

void DisplayWidget::showSuccessPageSlot(bool isSucceed)
{
    m_displayStackedLayout->setCurrentWidget(m_defaultConnectSuccessPageWidget);

    m_defaultConnectSuccessPageWidget->updateScanSettingsSlot(isSucceed);
}

void DisplayWidget::showFailedPageSlot()
{
    m_displayStackedLayout->setCurrentWidget(m_defaultConnectFailedPageWidget);
}

void DisplayWidget::showSuccessImageHandlePageSlot()
{
    m_displayStackedLayout->setCurrentWidget(m_defaultConnectSuccessPageWidget);

    m_defaultConnectSuccessPageWidget->showLeftImageHandleSuccessPage();
}

