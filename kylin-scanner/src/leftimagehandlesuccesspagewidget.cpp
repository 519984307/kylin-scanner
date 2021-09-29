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
#include "leftimagehandlesuccesspagewidget.h"

LeftImageHandleSuccessPageWidget::LeftImageHandleSuccessPageWidget(QWidget *parent) : QWidget(parent),
    m_thumbnailWidget(new ThumbnailWidget()),
    m_showImageWidget(new ShowImageWidget()),
    m_toolBarWidget(new ToolBarWidget()),
    m_showImageAndToolBarVLayout(new QVBoxLayout()),
    m_leftImageHandleSuccessPageHLayout(new QHBoxLayout)
{
    setupGui();
    initConnect();
    initSettings();
}

void LeftImageHandleSuccessPageWidget::setupGui()
{
    setMinimumSize(LeftImageHandleSuccessPageWidth, LeftImageHandleSuccessPageHeight);


    m_showImageAndToolBarVLayout->setSpacing(0);
    m_showImageAndToolBarVLayout->addSpacing(24);
    m_showImageAndToolBarVLayout->addStretch();
    m_showImageAndToolBarVLayout->addWidget(m_showImageWidget, 0, Qt::AlignCenter);
    m_showImageAndToolBarVLayout->addSpacing(24);
    m_showImageAndToolBarVLayout->addWidget(m_toolBarWidget, 0, Qt::AlignCenter);
    m_showImageAndToolBarVLayout->addSpacing(16);
    m_showImageAndToolBarVLayout->addStretch();
    m_showImageAndToolBarVLayout->setContentsMargins(0, 0, 0, 0);

    m_leftImageHandleSuccessPageHLayout->setSpacing(0);
    m_leftImageHandleSuccessPageHLayout->addWidget(m_thumbnailWidget);
    m_leftImageHandleSuccessPageHLayout->addStretch();
    m_leftImageHandleSuccessPageHLayout->addSpacing(111);
    m_leftImageHandleSuccessPageHLayout->addLayout(m_showImageAndToolBarVLayout);
    m_leftImageHandleSuccessPageHLayout->addSpacing(112);
    m_leftImageHandleSuccessPageHLayout->addStretch();
    m_leftImageHandleSuccessPageHLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_leftImageHandleSuccessPageHLayout);

}

void LeftImageHandleSuccessPageWidget::initConnect()
{

}

void LeftImageHandleSuccessPageWidget::initSettings()
{

}
