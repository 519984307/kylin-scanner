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
#include "globalsignal.h"

GlobalUserSignal* GlobalUserSignal::instance = new GlobalUserSignal;

GlobalUserSignal *GlobalUserSignal::getInstance()
{
    return instance;

}

void GlobalUserSignal::exitApplication()
{
    emit exitApplicationSignal();
}

void GlobalUserSignal::minimumWindow()
{
    emit minimumWindowSignal();
}

void GlobalUserSignal::maximumWindow()
{
    emit maximumWindowSignal();
}

void GlobalUserSignal::showAboutDialog()
{
    emit showAboutDialogSignal();
}

void GlobalUserSignal::showHelpDialog()
{
    emit showHelpDialogSignal();
}

void GlobalUserSignal::themeChangedWhite()
{
    emit themeChangedWhiteSignal();
}

void GlobalUserSignal::themeChangedBlack()
{
    emit themeChangedBlackSignal();
}

void GlobalUserSignal::fontSizeChanged(int fontSize)
{
    emit fontSizeChangedSignal(fontSize);
}


void GlobalUserSignal::detectPageWaitTimerStart()
{
    emit detectPageWaitTimerStartSignal();
}

void GlobalUserSignal::detectPageWaitTimerStop()
{
    emit detectPageWaitTimerStopSignal();
}

void GlobalUserSignal::openDeviceStatus(bool openSucceed)
{
    emit openDeviceStatusSignal(openSucceed);
}

void GlobalUserSignal::startScanOperation()
{
    emit startScanOperationSignal();
}

void GlobalUserSignal::stopScanOperation()
{
    emit stopScanOperationSignal();
}

void GlobalUserSignal::scanThreadFinished()
{
    emit scanThreadFinishedSignal();
}

void GlobalUserSignal::scanThreadFinishedImageLoad()
{
    emit scanThreadFinishedImageLoadSignal();
}




GlobalUserSignal::GlobalUserSignal(QObject *parent) : QObject(parent)
{

}

GlobalUserSignal::~GlobalUserSignal()
{

}


GlobalCoreSignal* GlobalCoreSignal::instance = new GlobalCoreSignal;

GlobalCoreSignal *GlobalCoreSignal::getInstance()
{
    return instance;
}


GlobalCoreSignal::GlobalCoreSignal(QObject *parent) : QObject(parent)
{

}

GlobalCoreSignal::~GlobalCoreSignal()
{

}


GlobalConfigSignal* GlobalConfigSignal::instance = new GlobalConfigSignal;
GlobalConfigSignal *GlobalConfigSignal::getInstance()
{
    return instance;
}

GlobalConfigSignal::GlobalConfigSignal(QObject *parent)
{
    configPath = QDir::homePath() + "/.config";
    scannerPath = configPath + "/kylin-scanner";
    scannerImagePath = scannerPath + "/images";
    scannerTempPath = QDir::tempPath() + "/kylin-scanner";
}

GlobalConfigSignal::~GlobalConfigSignal()
{

}
