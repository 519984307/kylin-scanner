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

#include "ukui-log4qt.h"


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

    void scanThreadFinished();




signals:
    void exitApplicationSignal(); // exit application
    void minimumWindowSignal(); // miminum window
    void maximumWindowSignal(); // maximum window
    void showAboutDialogSignal(); // show about dialog
    void showHelpDialogSignal(); // show help dialog

    void themeChangedWhiteSignal(); // white theme
    void themeChangedBlackSignal(); // black theme
    void fontSizeChangedSignal(int fontSize);   // system font size change

    void detectPageWaitTimerStartSignal(); // start detect page wait timer
    void detectPageWaitTimerStopSignal(); // stop detect page wait timer

    void openDeviceStatusSignal(bool openSucceed); // open device status

    void startScanOperationSignal();
    void stopScanOperationSignal();

    void scanThreadFinishedSignal();

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
