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
#ifndef DETECTPAGEWIDGET_H
#define DETECTPAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QIcon>
#include <QPalette>
#include <QSize>
#include <QTimer>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QMovie>

#include <ukui-log4qt.h>
#include "globalsignal.h"
#include "svghandler.h"

#define DetectPageIconSize  QSize(70, 70)

class DetectPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DetectPageWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();

    void getFileListNum();
    QFileInfoList GetFileList(QString path);

public slots:
    void showWaitImages();
    void setWaitTimerStart();
    void setWaitTimerStop();

signals:

private:
    int num = 0;
    int count = 0;

    QFileInfo fileinfo;
    QString path;
    QTimer *waitingTimer;

    QString themeColor = "white";


    QLabel *m_detectPageIcon;
    QLabel *m_detectPageText;
    QVBoxLayout *m_detectPageVLayout;

};

#endif // DETECTPAGEWIDGET_H
