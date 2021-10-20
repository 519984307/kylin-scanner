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
#include "successpagewidget.h"
#include <ukui-log4qt.h>

SuccessPageWidget::SuccessPageWidget(QWidget *parent) :
    QWidget(parent),
    m_leftStackSuccessPageLayout(new QStackedLayout),
    m_leftImageHandleSuccessPageWidget(new LeftImageHandleSuccessPageWidget()),
    m_leftSuccessPageWidget(new LeftSuccessPageWidget()),
    m_scanSettingsWidget(new ScanSettingsWidget()),
    m_successPageHLayout(new QHBoxLayout(this))
{
    setupGui();
    initConnect();
}

void SuccessPageWidget::setupGui()
{
    m_leftStackSuccessPageLayout->addWidget(m_leftSuccessPageWidget);
    m_leftStackSuccessPageLayout->addWidget(m_leftImageHandleSuccessPageWidget);
    m_leftStackSuccessPageLayout->setCurrentWidget(m_leftSuccessPageWidget);

    m_successPageHLayout->addLayout(m_leftStackSuccessPageLayout);
    m_successPageHLayout->addWidget(m_scanSettingsWidget);
    m_successPageHLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_successPageHLayout);
}

void SuccessPageWidget::initConnect()
{
}

void SuccessPageWidget::showLeftImageHandleSuccessPage()
{
    m_leftStackSuccessPageLayout->setCurrentWidget(m_leftImageHandleSuccessPageWidget);
}

void SuccessPageWidget::updateScanSettingsSlot(bool isWork)
{
    m_scanSettingsWidget->updateSettingsForDetectDevices();
}

