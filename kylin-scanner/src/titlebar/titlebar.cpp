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
#include "titlebar.h"
#include "ui_titlebar.h"

TitleBar::TitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TitleBar),
    m_menu(new QMenu(this))
{
    ui->setupUi(this);

    // appname
    QFont appnameFont;
    appnameFont.setPixelSize(TitlebarFontSize);
    ui->m_appnameLabel->setFont(appnameFont);

    // option menu
    m_menu->addAction(tr("Help"), this, [ = ]() {
        showHelpDialog();
    }, QKeySequence(Qt::Key_F1));

    m_menu->addAction(tr("About"), this, [ = ]() {
        g_user_signal->showAboutDialog();
    });
    m_menu->addAction(tr("Exit"), [ = ]() {
        g_user_signal->exitApplication();
    } );
    ui->m_OptionBtn->setMenu(m_menu);

}

TitleBar::~TitleBar()
{
    delete ui;
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (window()->isMaximized()) {
            updateMaxButtonStatus(false);
            window()->showNormal();
        } else if (! window()->isFullScreen()) {
            updateMaxButtonStatus(true);
            window()->showMaximized();
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::paintEvent(QPaintEvent *event)
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
        mainColor = QColor(255, 255, 255);
    } else {
        mainColor = QColor(38, 38, 38);
    }
    p.fillPath(rectPath,QBrush(mainColor));
}

void TitleBar::updateMaxButtonStatus(bool isMax)
{
    if (isMax) {
        ui->m_maxBtn->setIcon(QIcon::fromTheme("window-restore-symbolic"));
        ui->m_maxBtn->setToolTip(tr("Restore"));
    } else {
        ui->m_maxBtn->setIcon(QIcon::fromTheme("window-maximize-symbolic"));
        ui->m_maxBtn->setToolTip(tr("Maximize"));
    }
}

void TitleBar::on_m_minBtn_clicked()
{
    g_user_signal->minimumWindowSignal();
}

void TitleBar::on_m_maxBtn_clicked(bool checked)
{
    KyInfo() << "checked: " << checked;

    g_user_signal->maximumWindowSignal();
}

void TitleBar::on_m_closeBtn_clicked()
{
    g_user_signal->exitApplication();
}

void TitleBar::showHelpDialog()
{
    QString appName = "tools/kylin-scanner";
    KYCDaemonDbusObject *ipcDbus = new KYCDaemonDbusObject();

    if (!ipcDbus->daemonIsNotRunning()) {
        ipcDbus->showGuide(appName);
    }
}
