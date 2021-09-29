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
#ifndef LEFTIMAGEHANDLESUCCESSPAGEWIDGET_H
#define LEFTIMAGEHANDLESUCCESSPAGEWIDGET_H

#include "thumbnailwidget.h"
#include "showimagewidget.h"
#include "toolbarwidget.h"

#include <QWidget>
#include <QStackedLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListView>
#include <QListWidget>
#include <QListWidgetItem>

#define LeftImageHandleSuccessPageWidth  712
#define LeftImageHandleSuccessPageHeight 636

class LeftImageHandleSuccessPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LeftImageHandleSuccessPageWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();
    void initSettings();


private:
    ThumbnailWidget *m_thumbnailWidget;
    ShowImageWidget *m_showImageWidget;
    ToolBarWidget *m_toolBarWidget;

    QVBoxLayout *m_showImageAndToolBarVLayout;
    QHBoxLayout *m_leftImageHandleSuccessPageHLayout;

signals:

public slots:

};

#endif // LEFTIMAGEHANDLESUCCESSPAGEWIDGET_H
