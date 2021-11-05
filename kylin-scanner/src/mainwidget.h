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
#ifndef MainWidget_H
#define MainWidget_H

#include <QScreen>
#include <QStringList>
#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QEvent>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QPalette>
#include <QStackedLayout>
#include <QPainter>
#include <QKeyEvent>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QWidget>
#include <QX11Info>
#include <QGSettings>
#include <QThread>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QInputDialog>
#include <qmath.h>

#include  <ukui-log4qt.h>

#include "runningdialog.h"
#include "scandialog.h"
#include "about/about.h"
#include "displaywidget.h"
#include "include/common.h"
#include "include/theme.h"
#include "utils/xatom-helper.h"
#include "titlebar/titlebar.h"
#include "globalsignal.h"
#include "usb.h"
#include "beauty.h"
#include "rectify.h"


class ScanThread : public QThread
{
    Q_OBJECT

protected:
    void run() Q_DECL_OVERRIDE;

    int getSleepTime(QString &time);

Q_SIGNALS:
    void scanThreadFinishedSignal(int);
};

class DetectScanDevicesThread : public QThread
{
    Q_OBJECT
public:
    void run() Q_DECL_OVERRIDE;


Q_SIGNALS:
    void detectScanDevicesFinishedSignal(bool);
};


class UsbHotplugThread : public QThread
{
    Q_OBJECT
public:
    void run() Q_DECL_OVERRIDE;

    bool exitWindowFlag = false;
    int hotplug_sock;

Q_SIGNALS:
    void usbRemove(QString);
    void usbAdd(QString);
};

class BeautyThread : public QThread
{
    Q_OBJECT
public:
    explicit BeautyThread(QObject *parent = 0);
    ~BeautyThread();

    void run() Q_DECL_OVERRIDE;

    void  beautyThreadStop();

};

class RectifyThread : public QThread
{
    Q_OBJECT
public:
    explicit RectifyThread(QObject *parent = 0);
    ~RectifyThread();

    void run() Q_DECL_OVERRIDE;

    void rectifyThreadStop();
    void waitDialog();

};

class OcrThread : public QThread
{
    Q_OBJECT
public:
    explicit OcrThread(QObject *parent = 0);
    ~OcrThread();

    void run() Q_DECL_OVERRIDE;

    void ocrThreadStop();

};

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void setupGui();
    void initConnect();
    void initGsettings();

    void resizeTitleBar();
    void resizeDisplayWidget();
    void themeChange();
    void fontSizeChange();
    void transparencyChange();
    void warnMsg(QString msg);

    void reboot();

    static int const EXIT_CODE_REBOOT;

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    QGSettings *m_styleSettings;
    QGSettings *m_fontSizeSettings;
    QGSettings *m_transparencySettings;
    double m_transparency = 0.60;
    QStringList m_styleList;

    TitleBar *m_titleBar;
    DisplayWidget *m_displayWidget;
    QVBoxLayout *m_mainWidgetVLayout;

    DetectScanDevicesThread m_detectScanDevicesThread;
    UsbHotplugThread m_usbHotplugThread;

    ScanThread m_scanThread;
    AboutDialog *m_aboutDialog;
    ScanDialog *m_scanDialog;
    WatermarkDialog *m_watermarkDialog;

    RunningDialog *m_beautyRunningDialog;
    BeautyThread m_beautyThread;

    RunningDialog *m_rectifyRunningDialog;
    RectifyThread m_rectifyThread;

    OcrThread m_ocrThread;


public slots:
    void maximizeWindowSlot();
    void closeWindowSlot();
    void showAboutWindowSlot();
    void styleSettingsChanged(QString);

    void detectScanDevicesSlot();
    void detectScanDeviceThreadFinishedSlot(bool isDetected);

    void usbAddedOperationSlot(QString recvData);
    void usbRemovedOperationSlot(QString recvData);

    void startScanOperationSlot();
    void stopScanOperationSlot();
    void showScanDialogSlot();
    void scanThreadFinishedSlot(int saneStatus);

    void startOcrOperationSlot();

    void showBeautyRunningDialog();
    void startBeautyOperationSlot();
    void hideBeautyRunningDialog();

    void showRectifyRunningDialog();
    void startRectifyOperationSlot();
    void hideRectifyRunningDialog();

};
#endif // MainWidget_H
