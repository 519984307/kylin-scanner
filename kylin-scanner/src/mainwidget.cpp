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

bool isExited = false; //! exit scan thread

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


}

MainWidget::~MainWidget()
{
    if (m_detectScanDevicesThread.isRunning()) {
        KyInfo() << "Quit window, so quit detect scan devices thread ...";
        m_detectScanDevicesThread.wait();
        m_detectScanDevicesThread.quit();
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
    connect(g_user_signal, &GlobalUserSignal::minimumWindowSignal, this, [=](){
        this->showMinimized();
    });
    connect(g_user_signal, &GlobalUserSignal::maximumWindowSignal, this, &MainWidget::maximizeWindowSlot);
    connect(g_user_signal, &GlobalUserSignal::exitApplicationSignal, this, &MainWidget::closeWindowSlot);
    connect(g_user_signal, &GlobalUserSignal::showAboutDialogSignal, this, &MainWidget::showAboutWindowSlot);

    // detect scan devices start
    connect(m_displayWidget, &DisplayWidget::detectScanDevicesSignal, this, &MainWidget::detectScanDevicesSlot, Qt::QueuedConnection);

    // detect scan devices finished
    connect(&m_detectScanDevicesThread, &DetectScanDevicesThread::detectScanDevicesFinishedSignal, this, &MainWidget::detectScanDeviceThreadFinishedSlot);

    connect(g_user_signal, &GlobalUserSignal::startScanOperationSignal, this, &MainWidget::startScanOperationSlot);
    connect(g_user_signal, &GlobalUserSignal::stopScanOperationSignal, this, &MainWidget::stopScanOperationSlot);

    connect(&m_scanThread, &ScanThread::scanThreadFinishedSignal, this, &MainWidget::scanThreadFinishedSlot);

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

void MainWidget::resizeEvent(QResizeEvent *event)
{
    return;
    //resizeTitleBar();
    //resizeDisplayWidget();

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
    KyInfo() << "isDetected scan devices: " << isDetected;

    g_user_signal->detectPageWaitTimerStop();

    if (isDetected) {

        // while detect scan device thread finished, we should open the first device to get scan parameters
        g_sane_object->openSaneDeviceForPage(0);

        m_displayWidget->showSuccessPageSlot(isDetected);

    } else {
        m_displayWidget->showFailedPageSlot();

        // set some widgets isEnable(false)
    }
}

void MainWidget::startScanOperationSlot()
{

    m_scanThread.start();
    showScanDialogSlot();
}

void MainWidget::stopScanOperationSlot()
{
    KyInfo() << "pageNumber = " << m_scanDialog->pageNumber();
    if (m_scanThread.isRunning()) {
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

void MainWidget::scanThreadFinishedSlot()
{
    QString pageNumber = g_sane_object->userInfo.pageNumber;
    int retCompare = QString::compare(pageNumber, tr("Single"), Qt::CaseInsensitive);
    if ((retCompare == 0) && (! m_scanDialog->isHidden())) {
        m_scanDialog->hide();
    }

    m_displayWidget->showSuccessImageHandlePageSlot();
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

    quit();
}


void ScanThread::run()
{

    QString pageNumber = g_sane_object->userInfo.pageNumber;
    int retCompare = QString::compare(pageNumber, tr("Multiple"), Qt::CaseInsensitive);
    int sleepTime = 0;
    int ret = 0;
    if (retCompare == 0) {
        sleepTime = getSleepTime(g_sane_object->userInfo.time);
        KyInfo() << "sleepTime = " << sleepTime;
    }

    do {
        KyInfo() << "start_scanning ..............................";

        // scan format and name, we should update it at actually time
        g_sane_object->startScanning(g_sane_object->userInfo);
        KyInfo() << "start_scanning end, status = " << ret;

        emit scanThreadFinishedSignal(ret);
        sleep(sleepTime);

        g_user_signal->scanThreadFinished();
    } while ((sleepTime != 0) && (! isExited));

    quit();
}

int ScanThread::getSleepTime(QString &time)
{
    KyInfo() << "time: " << time;
    if (QString::compare(time, tr("3s"), Qt::CaseInsensitive) == 0) {
        return 3;
    } else if (QString::compare(time, tr("5s"), Qt::CaseInsensitive) == 0) {
        return 5;
    } else if (QString::compare(time, tr("7s"), Qt::CaseInsensitive) == 0) {
        return 7;
    } else if (QString::compare(time, tr("10s"), Qt::CaseInsensitive) == 0) {
        return 10;
    } else if (QString::compare(time, tr("15s"), Qt::CaseInsensitive) == 0) {
        return 15;
    }
}
