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
#ifndef GLOBALSIGNAL_H
#define GLOBALSIGNAL_H

#include <QObject>

#include <QDir>
#include <QFileInfo>



/**
 * @brief The GlobalUserSignal class
 * Global user signal
 *      -
 */
class GlobalUserSignal : public QObject
{
    Q_OBJECT

public:
    static GlobalUserSignal* getInstance();

    void exitApplication();
    void minimumWindow();
    void maximumWindow();
    void showAboutDialog();
    void showHelpDialog();

    void themeChangedWhite();
    void themeChangedBlack();
    void fontSizeChanged(int fontSize);

    void detectPageWaitTimerStart();
    void detectPageWaitTimerStop();

    void openDeviceStatus(bool openSucceed);

    void startScanOperation();
    void stopScanOperation();

    void scanThreadFinished(int saneStatus);
    void scanThreadFinishedImageLoad();

    void showImageAfterClickedThumbnail(QString loadPath);
    void updateSaveNameTextAfterScanSuccess();

    void saveAsButtonClicked(QString filepath);
    void sendMailButtonClicked();

    void toolbarBeautyOperationStart();
    void doBeautyOperation();
    void doBeautyOperationFinished();
    void toolbarBeautyOperationStop();

    void toolbarRectifyOperationStart();
    void doRectifyOperation();
    void doRectifyOperationFinished();
    void toolbarRectifyOperationStop();

    void toolbarOcrOperationStart();
    void doOcrOperation();
    void toolbarOcrOperationFinished();
    void toolbarOcrOperationStop();
    void stopOcrTimer();

    void toolbarCropOperation();
    void toolbarRotateOperation();
    void toolbarMirrorOperation();
    void toolbarWatermarkOperation();
    void toolbarZoomoutOperation();
    void toolbarZoominOperation();
    void toolbarPercentageChanged();



signals:


    void toolbarPercentageChangedSignel();
    void toolbarZoominOperationSignal();
    void toolbarZoomoutOperationSignal();
    void toolbarWatermarkOperationSignal();
    void toolbarMirrorOperationSignal();
    void toolbarRotateOperationSignal();
    void toolbarCropOperationSignal();

    void stopOcrTimerSignal();
    void toolbarOcrOperationStartSignal();
    void doOcrOperationSignal();
    void toolbarOcrOperationFinishedSignal();
    void toolbarOcrOperationStopSignal();

    void toolbarBeautyOperationStartSignal();
    void doBeautyOperationSignal();
    void doBeautyOperationFinishedSignal();
    void toolbarBeautyOperationStopSignal();

    void toolbarRectifyOperationStartSignal();
    void doRectifyOperationSignal();
    void doRectifyOperationFinishedSignal();
    void toolbarRectifyOperationStopSignal();

    void sendMailButtonClickedSignal();
    void saveAsButtonClickedSignal(QString filepath);

    void updateSaveNameTextAfterScanSuccessSignal();
    void showImageAfterClickedThumbnailSignal(QString loadPath);

    void scanThreadFinishedImageLoadSignal();
    void scanThreadFinishedSignal(int saneStatus);

    void stopScanOperationSignal();
    void startScanOperationSignal();

    void openDeviceStatusSignal(bool openSucceed);
    void detectPageWaitTimerStopSignal();
    void detectPageWaitTimerStartSignal();

    void fontSizeChangedSignal(int fontSize);
    void themeChangedBlackSignal();
    void themeChangedWhiteSignal();

    void showHelpDialogSignal();
    void showAboutDialogSignal();
    void maximumWindowSignal();
    void minimumWindowSignal();
    void exitApplicationSignal();

private:
    static GlobalUserSignal* instance;

    explicit GlobalUserSignal(QObject *parent = nullptr);
    ~GlobalUserSignal();
    GlobalUserSignal(const GlobalUserSignal &) = delete;
    GlobalUserSignal &operator = (const GlobalUserSignal &) = delete;

};


class GlobalCoreSignal : public QObject
{
    Q_OBJECT

public:
    static GlobalCoreSignal* getInstance();


private:
    static GlobalCoreSignal *instance;

    explicit GlobalCoreSignal(QObject *parent = nullptr);
    ~GlobalCoreSignal();
    GlobalCoreSignal(const GlobalCoreSignal &) = delete;
    GlobalCoreSignal &operator = (const GlobalCoreSignal &) = delete;

signals:

};

class GlobalConfigSignal : public QObject
{
    Q_OBJECT

public:
    static GlobalConfigSignal* getInstance();

    QString configPath;
    QString scannerPath;
    QString scannerImagePath;
    QString scannerTempPath;


private:
    static GlobalConfigSignal *instance;

    explicit GlobalConfigSignal(QObject *parent = nullptr);
    ~GlobalConfigSignal();
    GlobalConfigSignal(const GlobalConfigSignal &) = delete;
    GlobalConfigSignal &operator = (const GlobalConfigSignal &) = delete;

signals:

};

#define g_user_signal GlobalUserSignal::getInstance()
#define g_core_signal GlobalCoreSignal::getInstance()
#define g_config_signal GlobalConfigSignal::getInstance()

#endif // GLOBALSIGNAL_H

