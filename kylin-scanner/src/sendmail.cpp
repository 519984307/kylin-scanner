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

#include "sendmail.h"

NoMailDialog::NoMailDialog(QWidget *parent) :
    QDialog(parent)
    , m_closeButton (new QPushButton())
    , m_titleLabel (new QLabel())
    , m_textEdit (new QTextEdit())
    , m_lineFrame (new QFrame())
    , m_confirmButton (new QPushButton())
    , m_cancelButton (new QPushButton())
    , m_buttonsHBoxLayout (new QHBoxLayout())
    , m_closeHBoxLayout (new QHBoxLayout())
    , m_mainVBoxLayout (new QVBoxLayout())
{
    initWindow();

    initLayout();

    initConnect();
}

NoMailDialog::~NoMailDialog()
{

}

void NoMailDialog::initWindow()
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowTitle (tr("No email client"));
    setFixedSize(MailWindowWidth, MainWindowHeight);
}

void NoMailDialog::initLayout()
{
    m_closeButton->setIcon (QIcon::fromTheme (ICON_THEME_CLOSE));
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setIconSize (QSize(16, 16));
    m_closeButton->setProperty("isWindowButton", 0x2);
    m_closeButton->setProperty("useIconHighlightEffect", 0x8);
    m_closeButton->setFlat(true);

    m_closeHBoxLayout->setSpacing(0);
    m_closeHBoxLayout->addStretch();
    m_closeHBoxLayout->addWidget(m_closeButton);
    m_closeHBoxLayout->setAlignment(Qt::AlignCenter);
    m_closeHBoxLayout->setContentsMargins(0, 4, 4, 4);

    m_titleLabel->setText(tr("No email client"));
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_titleLabel->setFixedSize(260, 32);
    QFont ft;
    ft.setPixelSize(24);
    m_titleLabel->setFont(ft);

    QFont ft1;
    ft1.setPixelSize(14);
    m_textEdit->setFont(ft1);
    m_textEdit->setText(
        tr("Not find email client in the system, please download and install email client firstly."));

    int textEditSizeX = MailWindowWidth - 32 - 32;
    m_textEdit->setFixedSize(textEditSizeX, 50);
    m_textEdit->setReadOnly(true);

    m_lineFrame->setObjectName(QString::fromUtf8("line"));
    m_lineFrame->setMaximumHeight(1);
    m_lineFrame->setMaximumWidth(260);
    m_lineFrame->setMinimumWidth(260);
    m_lineFrame->setFrameShape(QFrame::HLine);

    m_confirmButton->setText(tr("Go to install"));
    m_confirmButton->setFixedSize(120, 36);
    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->setFixedSize(120, 36);

    m_buttonsHBoxLayout->setSpacing(0);
    m_buttonsHBoxLayout->addWidget(m_confirmButton);
    m_buttonsHBoxLayout->addSpacing(16);
    m_buttonsHBoxLayout->addWidget(m_cancelButton);
    m_buttonsHBoxLayout->setContentsMargins(0, 0, 32, 0);

    m_mainVBoxLayout->setSpacing(0);
    m_mainVBoxLayout->addLayout(m_closeHBoxLayout);
    m_mainVBoxLayout->addSpacing(18);
    m_mainVBoxLayout->addWidget(m_titleLabel);
    m_mainVBoxLayout->addSpacing(24);
    m_mainVBoxLayout->addWidget(m_textEdit);
    m_mainVBoxLayout->addSpacing(40);
    m_mainVBoxLayout->addLayout(m_buttonsHBoxLayout);
    m_mainVBoxLayout->setContentsMargins(32, 0, 0, 48);
    setLayout(m_mainVBoxLayout);
}

void NoMailDialog::initConnect()
{
    connect(m_confirmButton, &QPushButton::clicked, this, &NoMailDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &NoMailDialog::closeNoMailWindow);
    connect(m_closeButton, &QPushButton::clicked, this, &NoMailDialog::closeNoMailWindow);

    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, &NoMailDialog::themeChangedBlack);
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, &NoMailDialog::themeChangedWhite);
}


void NoMailDialog::themeChangedWhite()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(47, 44, 43));
    setAutoFillBackground(true);
    setPalette(pal);

    m_titleLabel->setStyleSheet("color:#D9FFFFFF"); // 85% => D9, 255,255,255 => FFFFFF
    m_textEdit->setStyleSheet("QTextEdit{background-color:rgb(47,44,43);color:#D9FFFFFF;border:0px}");
    m_lineFrame->setStyleSheet("QFrame{color:rgb(32,30,29)}");
}

void NoMailDialog::themeChangedBlack()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(255, 255, 255));
    setAutoFillBackground(true);
    setPalette(pal);

    m_titleLabel->setStyleSheet("color:#D9000000"); // 85% => D9, 255,255,255 => FFFFFF
    m_textEdit->setStyleSheet("QTextEdit{background-color:rgb(255,255,255);color:#D9000000;border:0px}");
    m_lineFrame->setStyleSheet("QFrame{color:rgb(32,30,29}");
}

void NoMailDialog::closeNoMailWindow()
{
    reject();
//    emit noMailWindowClose();
}

SendMailDialog::SendMailDialog(QWidget *parent) :
    QDialog(parent)
    , m_titleLabel (new QLabel())
    , m_closeButton (new QPushButton())
    , m_closeHBoxLayout (new QHBoxLayout())
    , m_titleHBoxLayout (new QHBoxLayout())
    , m_mainVBoxLayout (new QVBoxLayout())
    , m_addMailVBoxLayout (new QVBoxLayout())
    , m_scrollArea (new QScrollArea())
    , widget (new QWidget())
{
    initWindow();

    initLayout();

    initConnect();
}

SendMailDialog::~SendMailDialog()
{

}

void SendMailDialog::initWindow()
{
    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(winId(), hints);

    setWindowTitle (tr("Select email client"));
    setFixedSize(MailWindowWidth, MainWindowHeight);

    widget->setFixedSize(MailWindowWidth, MainWindowHeight);
    widget->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
}

void SendMailDialog::initLayout()
{
    m_closeButton->setIcon (QIcon::fromTheme (ICON_THEME_CLOSE));
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setIconSize (QSize(16, 16));
    m_closeButton->setProperty("isWindowButton", 0x2);
    m_closeButton->setProperty("useIconHighlightEffect", 0x8);
    m_closeButton->setFlat(true);

    m_closeHBoxLayout->setSpacing(0);
    m_closeHBoxLayout->addStretch();
    m_closeHBoxLayout->addWidget(m_closeButton);
    m_closeHBoxLayout->setAlignment(Qt::AlignCenter);
    m_closeHBoxLayout->setContentsMargins(0, 4, 4, 4);

    m_titleLabel->setText(tr("Select email client"));
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_titleLabel->setFixedSize(260, 32);
    QFont ft;
    ft.setPixelSize(24);
    m_titleLabel->setFont(ft);

    m_titleHBoxLayout->addStretch();
    m_titleHBoxLayout->addWidget(m_titleLabel);
    m_titleHBoxLayout->addStretch();
    m_titleHBoxLayout->setContentsMargins(0, 0, 0, 0);

    m_mainVBoxLayout->setSpacing(0);
    m_mainVBoxLayout->addLayout(m_closeHBoxLayout);
    m_mainVBoxLayout->addLayout(m_titleHBoxLayout);
    m_mainVBoxLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_mainVBoxLayout);
}

void SendMailDialog::initConnect()
{
    connect(m_closeButton, &QPushButton::clicked, this, &SendMailDialog::closeSendMailWindowSlot);

    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, &SendMailDialog::themeChangedBlack);
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, &SendMailDialog::themeChangedWhite);
}

Appinfo *_getAppList(const char *contentType)
{
    GList *applist;
    applist = g_app_info_get_all_for_type(contentType);
    GAppInfo *item;

    if (applist != NULL) {
        int len = g_list_length(applist);
        Appinfo *appinfo = (Appinfo *)malloc(sizeof(Appinfo) * (len + 1));

        for (int index = 0; index < len; ++index) {
            item = (GAppInfo *) g_list_nth_data(applist, index);
            appinfo[index].item = item;
        }
        appinfo[len].item = NULL;
        return appinfo;

    } else {
        return NULL;
    }
}

AppList *getAppIdList(const char *contentType)
{
    Appinfo *appinfo = _getAppList(contentType);
    if (appinfo != NULL) {
        int i = 0;

        while (appinfo[i].item != NULL)
            ++i;

        int count = i;
        int index = 0;
        AppList *list = (AppList *)malloc(sizeof(AppList) * (i + 1));

        int j = 0;
        for (j = 0; appinfo[j].item != NULL; ++j) {
            const char *id = g_app_info_get_id(appinfo[j].item);
            if (id != NULL) {
                int len = strlen(id);
                list[index].appid = (char *)malloc(sizeof(char) * (len + 1));
                strcpy(list[index].appid, id);
                ++index;
            } else {
                free(list + count);
                --count;
            }
        }
        list[count].appid = NULL;
        free(appinfo);
        return list;
    } else {
        return NULL;
    }
}
void SendMailDialog::setMailButtonList()
{
    int i = 0;

    AppList *maillist = getAppIdList(MailType);
    if (maillist) {
        qDebug() << maillist;
        m_addMailVBoxLayout->setSpacing(0);
        m_addMailVBoxLayout->setContentsMargins(0, 0, 0, 0);

        for (i = 0; maillist[i].appid != NULL; ++i) {
            QString single(maillist[i].appid);
            m_desktopName.append(single);
            QByteArray ba = QString(DesktopFilePath + single).toUtf8();
            GDesktopAppInfo *mailinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(mailinfo));
            const char *iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(mailinfo)));
            QIcon appicon;
            if (QIcon::hasThemeIcon(QString(iconname)))
                appicon = QIcon::fromTheme(QString(iconname));
            QPushButton *btn = new QPushButton();
            QFont ft;
            ft.setPixelSize(14);
            btn->setFont(ft);
            btn->setText(appname);
            btn->setFixedSize(256, 56);
            btn->setIcon(appicon);
            btn->setIconSize(QSize(40, 40));
            btn->setCheckable(true);
            btn->setStyleSheet("text-align:left");

            m_mailButtonList.append(btn);

            QHBoxLayout *m_addMailHBoxLayout = new QHBoxLayout();
            m_addMailHBoxLayout->addStretch();
            m_addMailHBoxLayout->addWidget(m_mailButtonList[i]);
            m_addMailHBoxLayout->addStretch();

            if (i == 0) {
                m_addMailHBoxLayout->setContentsMargins(0, 16, 0, 0);
            } else {
                m_addMailHBoxLayout->setContentsMargins(0, 0, 0, 0);
            }
            m_addMailVBoxLayout->addSpacing(4);
            m_addMailVBoxLayout->addLayout(m_addMailHBoxLayout);

            connect(m_mailButtonList[i], &QPushButton::toggled, this, &SendMailDialog::mailButtonClickedSlot);

            free(maillist[i].appid);
        }
        m_addMailVBoxLayout->addStretch();
        widget->setLayout(m_addMailVBoxLayout);

        m_scrollArea->setWidget(widget);
        m_scrollArea->setFrameShape(QFrame::NoFrame);

        if (i < 2) {
            // Only 2 btnMail, so hide scrollbar
            m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
        free(maillist);
    }
    m_mainVBoxLayout->addWidget(m_scrollArea);
    setLayout(m_mainVBoxLayout);
}

void SendMailDialog::openMail(QString name)
{
    QFile aFile(DesktopFilePath + name);
    QString cmd(BashType);
    QString str;
    QStringList  arglists;
    QString mailPicture(MailPicturePath);

    if (!aFile.exists())
        qDebug() << DesktopFilePath << name << " no exists!";
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug() << DesktopFilePath << name << " open false!";

    QTextStream aStream(&aFile);
    aStream.setAutoDetectUnicode(true);
    while (!aStream.atEnd()) {
        str = aStream.readLine();
        if (str.startsWith("Exec=", Qt::CaseSensitive))
            break;
    }
    str = str.section("Exec=", 1, 1);
    str = str.section(" ", 0, 0);
    qDebug() << "exec str = " << str;
    QProcess *process = new QProcess();

    arglists << "-c";
    arglists << str;
    qDebug() << "str = " << str;
    if (str == "thunderbird") {
        arglists << "-compose" << "attachment='/tmp/scanner/present_image.jpg'";
    } else if (str == "claws-mail") {
        arglists << "--attach" << mailPicture;
    } else if (str == "mutt") {
        arglists << "-a" << mailPicture;
    } else if (str == "evolution") {
        QString attach = QString("mailto:?attach=") + mailPicture;
        arglists << "--component=mail" << attach;
    } else {
        arglists << " ";
    }
    qDebug() << "cmd = " << cmd << arglists;

    if (str == "evolution") {
        process->start("evolution", arglists);
        return;
    }
    process->start(cmd, arglists);
}

void SendMailDialog::mailButtonClickedSlot()
{
    for (int i = 0; i < m_mailButtonList.size(); ++i) {
        if (m_mailButtonList[i]->isChecked()) {
            openMail(m_desktopName[i]);
            reject();
            break;
        }
    }
}

void SendMailDialog::closeSendMailWindowSlot()
{
    reject();
//    emit sendMailWindowClose();
}

void SendMailDialog::themeChangedWhite()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(47, 44, 43));
    setAutoFillBackground(true);
    setPalette(pal);

    m_titleLabel->setStyleSheet("color:#D9FFFFFF"); // 85% => D9, 255,255,255 => FFFFFF

}

void SendMailDialog::themeChangedBlack()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(255, 255, 255));
    setAutoFillBackground(true);
    setPalette(pal);

    m_titleLabel->setStyleSheet("color:#D9000000"); // 85% => D9, 255,255,255 => FFFFFF
}

