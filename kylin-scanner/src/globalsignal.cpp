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

#include <ukui-log4qt.h>

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

void GlobalUserSignal::scanThreadFinished(int saneStatus)
{
    emit scanThreadFinishedSignal(saneStatus);
}

void GlobalUserSignal::scanThreadFinishedImageLoad()
{
    emit scanThreadFinishedImageLoadSignal();
}

void GlobalUserSignal::showImageAfterClickedThumbnail(QString loadPath)
{
    emit showImageAfterClickedThumbnailSignal(loadPath);
}

void GlobalUserSignal::updateSaveNameTextAfterScanSuccess()
{
    emit updateSaveNameTextAfterScanSuccessSignal();
}

void GlobalUserSignal::saveAsButtonClicked(QString filepath)
{
    emit saveAsButtonClickedSignal(filepath);
}

void GlobalUserSignal::sendMailButtonClicked()
{
    emit sendMailButtonClickedSignal();
}

void GlobalUserSignal::toolbarBeautyOperationStart()
{
    emit toolbarBeautyOperationStartSignal();
}

void GlobalUserSignal::doBeautyOperation()
{
    emit doBeautyOperationSignal();
}

void GlobalUserSignal::doBeautyOperationFinished()
{
    emit doBeautyOperationFinishedSignal();
}

void GlobalUserSignal::toolbarBeautyOperationStop()
{
    emit toolbarBeautyOperationStopSignal();
}

void GlobalUserSignal::toolbarRectifyOperationStart()
{
    emit toolbarRectifyOperationStartSignal();
}

void GlobalUserSignal::doRectifyOperation()
{
    emit doRectifyOperationSignal();
}

void GlobalUserSignal::doRectifyOperationFinished()
{
    emit doRectifyOperationFinishedSignal();
}

void GlobalUserSignal::toolbarRectifyOperationStop()
{
    emit toolbarRectifyOperationStopSignal();
}

void GlobalUserSignal::toolbarOcrOperationStart()
{
    emit toolbarOcrOperationStartSignal();
}

void GlobalUserSignal::doOcrOperation()
{
    emit doOcrOperationSignal();
}

void GlobalUserSignal::toolbarOcrOperationFinished()
{
    emit toolbarOcrOperationFinishedSignal();
}

void GlobalUserSignal::toolbarOcrOperationStop()
{
    emit toolbarOcrOperationStopSignal();
}

void GlobalUserSignal::toolbarCropOperation()
{
    emit toolbarCropOperationSignal();
}

void GlobalUserSignal::toolbarRotateOperation()
{
    emit toolbarRotateOperationSignal();
}

void GlobalUserSignal::toolbarMirrorOperation()
{
    emit toolbarMirrorOperationSignal();
}

void GlobalUserSignal::toolbarWatermarkOperation()
{
    emit toolbarWatermarkOperationSignal();
}

void GlobalUserSignal::toolbarZoomoutOperation()
{
    emit toolbarZoomoutOperationSignal();
}
void GlobalUserSignal::toolbarZoominOperation()
{
    emit toolbarZoominOperationSignal();
}

void GlobalUserSignal::toolbarPercentageChanged()
{
    emit toolbarPercentageChangedSignel();
}

void GlobalUserSignal::stopOcrTimer()
{
    emit stopOcrTimerSignal();
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
