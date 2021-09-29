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
#include "detectpagewidget.h"

DetectPageWidget::DetectPageWidget(QWidget *parent) : QWidget(parent),
    m_detectPageIcon(new QLabel()),
    m_detectPageText(new QLabel()),
    m_detectPageVLayout(new QVBoxLayout())
{
    setupGui();
    initConnect();
}

void DetectPageWidget::setupGui()
{
    getFileListNum();

    waitingTimer = new QTimer();
    setWaitTimerStart();


    m_detectPageIcon->setFixedSize(DetectPageIconSize);
    m_detectPageIcon->setPixmap(QIcon::fromTheme("kylin-scanner").pixmap(m_detectPageIcon->size()));

    m_detectPageText->setEnabled(false);
    m_detectPageText->setText(tr("Detect scanners, please waiting ..."));

    m_detectPageVLayout->setSpacing(0);
    m_detectPageVLayout->addStretch();
    m_detectPageVLayout->addWidget(m_detectPageIcon, 0,  Qt::AlignCenter);
    m_detectPageVLayout->addSpacing(13);
    m_detectPageVLayout->addWidget(m_detectPageText, 0,  Qt::AlignCenter);
    m_detectPageVLayout->addStretch();
    m_detectPageVLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_detectPageVLayout);


}

void DetectPageWidget::initConnect()
{
    connect(waitingTimer, SIGNAL(timeout()), this, SLOT(showWaitImages()));

    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, [=](){
        themeColor = "black";
    });
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, [=](){
        themeColor = "white";
    });

    connect(g_user_signal, &GlobalUserSignal::detectPageWaitTimerStartSignal, this, &DetectPageWidget::setWaitTimerStart);
    connect(g_user_signal, &GlobalUserSignal::detectPageWaitTimerStopSignal, this, &DetectPageWidget::setWaitTimerStop);

}

void DetectPageWidget::getFileListNum()
{
    path = ":/waiting/";

    num = GetFileList(path).size();

    count = 0;
}

QFileInfoList DetectPageWidget::GetFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (int i = 0; i != folder_list.size(); i++) {
        QString name = folder_list.at(i).absoluteFilePath();
        QFileInfoList child_file_list = GetFileList(name);
        file_list.append(child_file_list);
    }

    return file_list;
}

void DetectPageWidget::showWaitImages()
{
    fileinfo = GetFileList(path).at(count);
    m_detectPageIcon->setScaledContents(true);

    if (! themeColor.compare("white", Qt::CaseInsensitive)) {
        m_detectPageIcon->setPixmap(SVGHandler::loadSvgColor(fileinfo.filePath(), "black", 70));
    } else {
        m_detectPageIcon->setPixmap(SVGHandler::loadSvgColor(fileinfo.filePath(), "white", 70));
    }

    ++count;

    if (count == num) {
        count = 0;
    }
}

void DetectPageWidget::setWaitTimerStart()
{
    waitingTimer->start(100);
}

void DetectPageWidget::setWaitTimerStop()
{
    waitingTimer->stop();
}
