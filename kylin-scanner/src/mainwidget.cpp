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

#include "mainwidget.h"
#include <QMessageBox>
#include <QWidgetList>
#include <QPropertyAnimation>
#include <QTimer>

static bool isExited = false; //! exit scan thread

int const MainWidget::EXIT_CODE_REBOOT = -123456789;

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent),
      m_titleBar(new TitleBar(this)),
      m_displayWidget(new DisplayWidget(this)),
      m_mainWidgetVLayout(new QVBoxLayout(this))
{
    setupGui();
    initConnect();
    initGsettings();

    m_detectScanDevicesThread.start();
    m_usbHotplugThread.start();
}

MainWidget::~MainWidget()
{
    if (m_detectScanDevicesThread.isRunning()) {
        KyInfo() << "Quit window, so quit detect scan devices thread ...";
        m_detectScanDevicesThread.wait();
        m_detectScanDevicesThread.quit();
    }

    if (m_scanThread.isRunning()) {
        KyInfo() << "Quit window, so quit scan thread ...";
        m_scanThread.wait();
        m_scanThread.quit();
    }
}

void MainWidget::setupGui()
{
    // Frosted glass effect, must before than XAtomHelper
    this->setProperty("useSystemStyleBlur", true);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    // Add window control protocol
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), hints);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    resize(MainwidgetWidth, MainWidgetHeight);
    setMinimumSize(MainwidgetWidth, MainWidgetHeight);
    setWindowTitle(tr("kylin-scanner"));
    this->setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    setMouseTracking(true);
    setAutoFillBackground(true);
    setAcceptDrops(true);

    // Center window
    QScreen *screen = QGuiApplication::primaryScreen();
    move((screen->geometry().width() - MainwidgetWidth) / 2, (screen->geometry().height() - MainWidgetHeight) / 2);

    this->setObjectName("MainWindow");

    m_mainWidgetVLayout->setSpacing(0);
    m_mainWidgetVLayout->addWidget(m_titleBar);
    m_mainWidgetVLayout->addWidget(m_displayWidget);
    m_mainWidgetVLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_mainWidgetVLayout);

}

void MainWidget::initConnect()
{
    connect(g_user_signal, &GlobalUserSignal::minimumWindowSignal, this, &MainWidget::showMinimized);
    connect(g_user_signal, &GlobalUserSignal::maximumWindowSignal, this, &MainWidget::maximizeWindowSlot);
    connect(g_user_signal, &GlobalUserSignal::exitApplicationSignal, this, &MainWidget::closeWindowSlot);
    connect(g_user_signal, &GlobalUserSignal::showAboutDialogSignal, this, &MainWidget::showAboutWindowSlot);

    connect(m_displayWidget, &DisplayWidget::detectScanDevicesSignal, this, &MainWidget::detectScanDevicesSlot, Qt::QueuedConnection);

    connect(&m_detectScanDevicesThread, &DetectScanDevicesThread::detectScanDevicesFinishedSignal, this, &MainWidget::detectScanDeviceThreadFinishedSlot);

    connect(&m_usbHotplugThread, &UsbHotplugThread::usbAdd, this, &MainWidget::usbAddedOperationSlot);
    connect(&m_usbHotplugThread, &UsbHotplugThread::usbRemove, this, &MainWidget::usbRemovedOperationSlot);

    connect(g_user_signal, &GlobalUserSignal::startScanOperationSignal, this, &MainWidget::startScanOperationSlot);
    connect(g_user_signal, &GlobalUserSignal::stopScanOperationSignal, this, &MainWidget::stopScanOperationSlot);

    connect(g_user_signal, &GlobalUserSignal::scanThreadFinishedSignal, this, &MainWidget::scanThreadFinishedSlot);

    connect(g_user_signal, &GlobalUserSignal::toolbarWatermarkOperationSignal, this, &MainWidget::showWatermarkDialogSlot);
}

void MainWidget::initGsettings()
{
    if (QGSettings::isSchemaInstalled(ORG_UKUI_STYLE)) {
        m_styleSettings = new QGSettings(ORG_UKUI_STYLE);
        connect(m_styleSettings, &QGSettings::changed, this, [=] (const QString &key) {
            if (key == STYLE_NAME) {
                themeChange();
            }
        });
    }

    if (QGSettings::isSchemaInstalled(ORG_UKUI_STYLE)) {
        m_fontSizeSettings = new QGSettings(ORG_UKUI_STYLE);
        connect(m_fontSizeSettings, &QGSettings::changed, this, [=] (const QString &key) {
            if (key == SYSTEM_FONT_NAME) {
                fontSizeChange();
            }
        });
    }

    if (QGSettings::isSchemaInstalled(ORG_UKUI_CONTROL_CENTER_PERSONALISE)) {
        m_transparencySettings = new QGSettings(ORG_UKUI_CONTROL_CENTER_PERSONALISE);
        connect(m_transparencySettings, &QGSettings::changed, this, [=] (const QString &key){
            if (key == "transparency") {
                transparencyChange();
            }
        });
    }
    transparencyChange();

    themeChange();
    fontSizeChange();
    return;
}

/**
 * @brief MainWidget::resizeTitleBar
 * resize Titlebar while MainWidget size have been resized
 */
void MainWidget::resizeTitleBar()
{
    m_titleBar->move(0, 0);
    m_titleBar->resize(this->width(), TitlebarHeight);
}

void MainWidget::resizeDisplayWidget()
{
    m_displayWidget->resize(this->width(), this->height() - TitlebarHeight);
}

void MainWidget::themeChange()
{
    QString themeStyle = m_styleSettings->get(STYLE_NAME).toString();
    QPalette pal(palette());

    if ("ukui-dark" == themeStyle || "ukui-black" == themeStyle) {
        pal.setColor(QPalette::Background, QColor(38, 38, 38));

        g_user_signal->themeChangedBlack();
    } else {
        pal.setColor(QPalette::Background, QColor(255, 255, 255));

        g_user_signal->themeChangedWhite();
    }
    setAutoFillBackground(true);
    setPalette(pal);
}

void MainWidget::fontSizeChange()
{
    int fontSize = m_fontSizeSettings->get(SYSTEM_FONT_NAME).toInt();
    KyInfo() << "fontSize = " << fontSize;

    emit g_user_signal->fontSizeChanged(fontSize);
}

void MainWidget::transparencyChange()
{
  m_transparency = m_transparencySettings->get(TRANSPARENCY_NAME).toDouble() * 255;
  this->update();
}

void MainWidget::warnMsg(QString msg)
{
    QMessageBox *msgBox = new QMessageBox();

    msgBox->setText(msg);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    msgBox->setWindowTitle(tr("Scanner"));
    msgBox->setStandardButtons(QMessageBox::Yes);
    msgBox->setContextMenuPolicy(Qt::NoContextMenu);
    msgBox->button(QMessageBox::Yes)->setText(tr("Yes"));

    QWidget *widget = nullptr;
    QWidgetList widgetList = QApplication::allWidgets();
    for (int i=0; i<widgetList.length(); ++i) {
        if (widgetList.at(i)->objectName() == "MainWindow") {
            widget = widgetList.at(i);
        }
    }
    if (widget) {
        msgBox->setParent(widget);
        QRect rect = widget->geometry();
        int x = rect.x() + rect.width()/2 - msgBox->width()/2;
        int y = rect.y() + rect.height()/2 - msgBox->height()/2;
        msgBox->move(x,y);
    }

    msgBox->exec();

    QTimer* timer = new QTimer();
    timer->start(1500);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, msgBox, &QMessageBox::hide);


}

void MainWidget::reboot()
{
    qApp->exit(EXIT_CODE_REBOOT);
}

void MainWidget::resizeEvent(QResizeEvent *event)
{
    return;

    QWidget::resizeEvent(event);
}

void MainWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect(), 0, 0);
    QStyleOption opt;
    opt.init(this);

    QColor mainColor;

    if (QColor(255,255,255) == opt.palette.color(QPalette::Base) || QColor(248,248,248) == opt.palette.color(QPalette::Base)) {
        mainColor = QColor(242, 242, 242, m_transparency);
    } else {
        mainColor = QColor(20, 20, 20, m_transparency);
    }

    p.fillPath(rectPath,QBrush(mainColor));
}

void MainWidget::keyReleaseEvent(QKeyEvent *event)
{
    return QWidget::keyReleaseEvent(event);

    switch (event->key()) {
    case Qt::Key_F1:
        KyInfo() << "F1 ==========";
        g_user_signal->showHelpDialog();
        break;
    default:
        QWidget::keyReleaseEvent(event);
        break;
    }
}

void MainWidget::maximizeWindowSlot()
{
    if (this->isFullScreen()) {
        m_titleBar->updateMaxButtonStatus(false);
        this->showNormal();
    } else {
        if (this->isMaximized()) {
            m_titleBar->updateMaxButtonStatus(false);
            this->showNormal();
        } else {
            m_titleBar->updateMaxButtonStatus(true);
            this->showMaximized();
        }
    }
}

void MainWidget::closeWindowSlot()
{
    this->close();
    stopScanOperationSlot();
}

void MainWidget::showAboutWindowSlot()
{
    m_aboutDialog = new KYCAboutDialog(this);

    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    m_aboutDialog->move(globalPos.x() + (this->width() - m_aboutDialog->width())/2,
                        globalPos.y() + (this->height() - m_aboutDialog->height())/2);

    m_aboutDialog->exec();
}

void MainWidget::styleSettingsChanged(QString)
{
    QPalette pal(palette());
    if (m_styleList.contains(m_styleSettings->get(STYLE_NAME).toString())) {
        pal.setColor(QPalette::Background, QColor(38, 38, 38));
    } else {
        pal.setColor(QPalette::Background, QColor(255, 255, 255));
    }
    setAutoFillBackground(true);
    setPalette(pal);
}

void MainWidget::detectScanDevicesSlot()
{
    if (! m_detectScanDevicesThread.isRunning()) {
        KyInfo() << "Not running detect scan devices thread. Let's detect ...";
        m_detectScanDevicesThread.start();

        g_user_signal->detectPageWaitTimerStart();
    }
}

void MainWidget::detectScanDeviceThreadFinishedSlot(bool isDetected)
{
    g_user_signal->detectPageWaitTimerStop();

    if (isDetected) {
        // while detect scan device thread finished, we should open the first device to get scan parameters
        g_sane_object->openSaneDeviceForPage(0);

        m_displayWidget->showSuccessPageSlot(isDetected);
    } else {
        m_displayWidget->showFailedPageSlot();
    }

}

void MainWidget::usbAddedOperationSlot(QString recvData)
{
    KyInfo() << "USB Add: " << recvData;

    /// Think below:
    /// whether we need judge this usb device is a scanner usb device,
    /// or this method is perfect that we do not need to change ?
    g_sane_object->saneExit();
    m_detectScanDevicesThread.start();

    KyInfo() << "device add";
    QString msg = tr("There is a new scanner connect, redetect all scanners, please wait a moment. ");
    warnMsg(msg);
}

void MainWidget::usbRemovedOperationSlot(QString recvData)
{
    KyInfo() << "USB Remove: " << recvData;

    QProcess *scanList = new QProcess(this);
    QStringList  argvList;
    argvList.append("-L");
    scanList->start("scanimage", argvList);

    connect(scanList, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [ = ](int exitCode, QProcess::ExitStatus exitStatus) {

        KyInfo() << "USB Remove exitCode = " << exitCode
                 << "exitStatus = " << exitStatus;

        if (0 == exitCode) {
            QString result = scanList->readAll();
            KyInfo() << "result = " << result;

            QStringList strListDevice;
            strListDevice =g_sane_object->getSaneNames();
            KyInfo() << "current sane names: " << strListDevice
                     << "userInfo.name= " << g_sane_object->userInfo.name
                     << "size = " << strListDevice.size();

            for (int i = 0; i < strListDevice.size(); ++i) {
                QString str = strListDevice.at(i).toLocal8Bit().constData();
                KyInfo() << "i=" << i << "str = " << str;

                if (str == tr("No available device")) {
                    break;
                }

                /// There are two cases that we cannot find scanners throught `scanimage -L`:
                /// case 1. no this scanner in system, which means this scanner has been disconnect
                /// case 2. this scanner has been connect in kylin-scanner by sane_init
                /// case 3. this scanner has been disconnet, but net or usb can be found CaseInsensitive, such as hp:Color LaserJet Pro MFP M281fdw
                if (! result.contains(str, Qt::CaseSensitive)) {
                    QString msg;
                    bool retStatus;
                    msg = tr("device ") + str + tr(" has been disconnect.");
                    KyInfo() << "usbRemoved msg: " << msg
                             << "userInfo.name = " << g_sane_object->userInfo.name
                             << "str = " << str;

                    if (g_sane_object->userInfo.name == str) {
                        KyInfo() << "The user choose device: " << str << "has been disconnect!";
                        // get argument again, because it cannot search using scanner while has opened it

                        g_sane_object->openSaneDeviceForPage(i);
                        QString currentOpenDeviceName = g_sane_object->openSaneName;
                        msg = tr("device ") + currentOpenDeviceName + tr(" has been disconnect.");
                        retStatus = g_sane_object->getSaneStatus();

                        KyInfo() << "test scanning end, status = " << retStatus;

                        if (!retStatus) {
                            warnMsg(msg);

                            // Need this before detect devices again, because meet `Error during device I/O`,
                            // which could crash this application.
                            g_sane_object->saneExit();
                            m_detectScanDevicesThread.start();

                            if (result.contains("No scanners were identified", Qt::CaseInsensitive)) {
                                KyInfo() << "No scanners were identified.";
                                // If not find any scan device
//                                m_displayWidget->showFailedPageSlot();
                            }
                        }

                    }
                }
            }
        }
    });

}

void MainWidget::startScanOperationSlot()
{
    g_sane_object->ocrFlag = 0;
    m_scanThread.start();

    QString pageNumber = g_sane_object->userInfo.pageNumber;
    int retCompare = QString::compare(pageNumber, tr("Single"), Qt::CaseInsensitive);
    if (retCompare == 0 ) {
        return;
    }

    showScanDialogSlot();
}

void MainWidget::stopScanOperationSlot()
{
    KyInfo() << "pageNumber = " << m_scanDialog->pageNumber();
    if (m_scanThread.isRunning()) {
        KyInfo() << "stop scan operation.";
        isExited = true;
        m_scanThread.quit();
        m_scanThread.wait();
    }

}

void MainWidget::showScanDialogSlot()
{
    m_scanDialog = new ScanDialog(this);

    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    m_scanDialog->move(globalPos.x() + (this->width() - m_scanDialog->width())/2,
                        globalPos.y() + (this->height() - m_scanDialog->height())/2);

    m_scanDialog->exec();
}

void MainWidget::scanThreadFinishedSlot(int saneStatus)
{
    if (saneStatus != SANE_STATUS_GOOD) {
        if (saneStatus == SANE_STATUS_INVAL) {
            warnMsg(tr("Invalid argument, please change arguments or switch other scanners."));
        } else if (saneStatus == SANE_STATUS_DEVICE_BUSY) {
            warnMsg(tr("Device busy, please wait or switch other scanners."));
        } else if(saneStatus == SANE_STATUS_NO_DOCS) {
            warnMsg(tr("Document feeder out of documents, please place papers and scan again."));
        } else {
            warnMsg(tr("Scan failed, please check your scanner or switch other scanners."));
        }
    } else {
        g_sane_object->haveScanSuccessImage = true;

        m_displayWidget->showSuccessImageHandlePageSlot();
        g_user_signal->scanThreadFinishedImageLoad();

    }
}

void MainWidget::showWatermarkDialogSlot()
{
    m_watermarkDialog = new WatermarkDialog(this);

    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    m_watermarkDialog->move(globalPos.x() + (this->width() - m_watermarkDialog->width())/2,
                        globalPos.y() + (this->height() - m_watermarkDialog->height())/2);

    m_watermarkDialog->exec();

}


void DetectScanDevicesThread::run()
{
    KyInfo() << "FindScanDevicesThread begin";

    g_sane_object->detectSaneDeviceForPage();

    if (! g_sane_object->getSaneStatus()) {
        emit detectScanDevicesFinishedSignal(false);
        KyInfo() << "scan finished!";
    } else {
        emit detectScanDevicesFinishedSignal(true);
    }
}


void ScanThread::run()
{
    int sleepTime = 0;
    int ret = 0;

    g_sane_object->scanPageNumber = 0;

    QString pageNumber = g_sane_object->userInfo.pageNumber;
    int retCompare = QString::compare(pageNumber, tr("Multiple"), Qt::CaseInsensitive);

    if (retCompare == 0) {
        sleepTime = getSleepTime(g_sane_object->userInfo.time);
        KyInfo() << "sleepTime = " << sleepTime;
    }

    do {
        KyInfo() << "start_scanning start";
        ret = g_sane_object->startScanning(g_sane_object->userInfo);
        KyInfo() << "start_scanning end, status = " << ret;

        int saneStatus = ret;

        g_user_signal->scanThreadFinished(ret);

        if (saneStatus != SANE_STATUS_GOOD) {
            // Avoid duplicate scan in multiple scanning while meet error
            break;
        }

        KyInfo() << "start sleep time: " << sleepTime;
        sleep(sleepTime);

        g_sane_object->scanPageNumber += 1;

        KyInfo() << "sleep end."
                 << "scanPageNumber = " << g_sane_object->scanPageNumber;

    } while ((sleepTime != 0) && (! isExited));
}

int ScanThread::getSleepTime(QString &time)
{
    if (QString::compare(time, "3s", Qt::CaseInsensitive) == 0
            || QString::compare(time, tr("3s"), Qt::CaseInsensitive) == 0) {
        return 3;
    } else if (QString::compare(time, "5s", Qt::CaseInsensitive) == 0
               || QString::compare(time, tr("5s"), Qt::CaseInsensitive) == 0) {
        return 5;
    } else if (QString::compare(time, "7s", Qt::CaseInsensitive) == 0
               || QString::compare(time, tr("7s"), Qt::CaseInsensitive) == 0) {
        return 7;
    } else if (QString::compare(time, "10s", Qt::CaseInsensitive) == 0
               || QString::compare(time, tr("10s"), Qt::CaseInsensitive) == 0) {
        return 10;
    } else if (QString::compare(time, "15s", Qt::CaseInsensitive) == 0
               || QString::compare(time, tr("15s"), Qt::CaseInsensitive) == 0) {
        return 15;
    }
}
