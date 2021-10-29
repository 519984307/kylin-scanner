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

NoMailDialog::NoMailDialog(QWidget *parent) : QDialog(parent)
  , m_noMailTitleTextLabel (new QLabel())
  , m_noMailCloseButton (new QPushButton())
  , m_noMailTitleHBoxLayout (new QHBoxLayout())
  , m_noMaillogoLabel (new QLabel())
  , m_noMailtitleLabel (new QLabel())
  , m_noMailLogoTitleHBoxLayout (new QHBoxLayout())
  , m_noMailInfoLabel (new QLabel())
  , m_noMailInfoHBoxLayout (new QHBoxLayout())
  , m_cancelButton (new QPushButton())
  , m_installButton (new QPushButton())
  , m_noMailButtonsHBoxLayout (new QHBoxLayout())
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
    setFixedSize(NoMailWindowWidth, NoMainWindowHeight);
}

void NoMailDialog::initLayout()
{
    m_noMailTitleTextLabel->setText(tr("Scanner"));

    m_noMailCloseButton->setIcon (QIcon::fromTheme (ICON_THEME_CLOSE));
    m_noMailCloseButton->setToolTip(tr("Close"));
    m_noMailCloseButton->setFixedSize(30, 30);
    m_noMailCloseButton->setIconSize (QSize(16, 16));
    m_noMailCloseButton->setProperty("isWindowButton", 0x2);
    m_noMailCloseButton->setProperty("useIconHighlightEffect", 0x8);
    m_noMailCloseButton->setFlat(true);

    m_noMailTitleHBoxLayout->setSpacing(0);
    m_noMailTitleHBoxLayout->addSpacing(12);
    m_noMailTitleHBoxLayout->addWidget(m_noMailTitleTextLabel);
    m_noMailTitleHBoxLayout->addStretch();
    m_noMailTitleHBoxLayout->addWidget(m_noMailCloseButton);
    m_noMailTitleHBoxLayout->setAlignment(Qt::AlignCenter);
    m_noMailTitleHBoxLayout->setContentsMargins(0, 5, 4, 4);

    QPixmap pix(":/warning.svg");
    m_noMaillogoLabel->setFixedSize(QSize(24, 24));
    m_noMaillogoLabel->setPixmap(pix);

    QFont ft;
    ft.setPixelSize(14);
    ft.setBold(true);
    m_noMailtitleLabel->setFont(ft);
    m_noMailtitleLabel->setText(tr("No email client"));
    m_noMailtitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_noMailLogoTitleHBoxLayout->setSpacing(0);
    m_noMailLogoTitleHBoxLayout->addSpacing(24);
    m_noMailLogoTitleHBoxLayout->addWidget(m_noMaillogoLabel);
    m_noMailLogoTitleHBoxLayout->addSpacing(8);
    m_noMailLogoTitleHBoxLayout->addWidget(m_noMailtitleLabel);
    m_noMailLogoTitleHBoxLayout->addStretch();
    m_noMailLogoTitleHBoxLayout->setContentsMargins(0, 0, 0, 0);

    QFont ft1;
    ft1.setPixelSize(14);
    m_noMailInfoLabel->setFont(ft1);
    m_noMailInfoLabel->setText(tr("Not find email client in the system, please install email client firstly."));
    m_noMailInfoLabel->setFixedSize(QSize(296, 44));
    m_noMailInfoLabel->setWordWrap(true);

    m_noMailInfoHBoxLayout->setSpacing(0);
    m_noMailInfoHBoxLayout->addSpacing(56);
    m_noMailInfoHBoxLayout->addWidget(m_noMailInfoLabel);
    m_noMailInfoHBoxLayout->addSpacing(28);
    m_noMailInfoHBoxLayout->setContentsMargins(0, 0, 0, 0);



    m_cancelButton->setText(tr("Cancel"));
    m_cancelButton->setFixedSize(96, 36);

    m_installButton->setText(tr("Install"));
    m_installButton->setFixedSize(96, 36);

    m_installButton->setFocus();
    m_installButton->setDefault(true);

    m_noMailButtonsHBoxLayout->setSpacing(0);
    m_noMailButtonsHBoxLayout->addSpacing(152);
    m_noMailButtonsHBoxLayout->addWidget(m_cancelButton);
    m_noMailButtonsHBoxLayout->addSpacing(12);
    m_noMailButtonsHBoxLayout->addWidget(m_installButton);
    m_noMailButtonsHBoxLayout->setContentsMargins(0, 0, 24, 24);

    m_mainVBoxLayout->setSpacing(0);
    m_mainVBoxLayout->addLayout(m_noMailTitleHBoxLayout);
    m_mainVBoxLayout->addSpacing(16);
    m_mainVBoxLayout->addLayout(m_noMailLogoTitleHBoxLayout);
    m_mainVBoxLayout->addSpacing(8);
    m_mainVBoxLayout->addLayout(m_noMailInfoHBoxLayout);
    m_mainVBoxLayout->addSpacing(24);
    m_mainVBoxLayout->addLayout(m_noMailButtonsHBoxLayout);
    m_mainVBoxLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_mainVBoxLayout);
}

void NoMailDialog::initConnect()
{
    connect(m_installButton, &QPushButton::clicked, this, &NoMailDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &NoMailDialog::closeNoMailWindow);
    connect(m_noMailCloseButton, &QPushButton::clicked, this, &NoMailDialog::closeNoMailWindow);

//    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, &NoMailDialog::themeChangedBlack);
//    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, &NoMailDialog::themeChangedWhite);
}

void NoMailDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect(), 0, 0);
    QStyleOption opt;
    opt.init(this);

    QColor mainColor;
    mainColor = opt.palette.color(QPalette::Window);

    p.fillPath(rectPath,QBrush(mainColor));
}


void NoMailDialog::themeChangedWhite()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(255, 255, 255));
    setAutoFillBackground(true);
    setPalette(pal);

    m_noMailtitleLabel->setStyleSheet("color:#D9FFFFFF"); // 85% => D9, 255,255,255 => FFFFFF
}

void NoMailDialog::themeChangedBlack()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(47, 44, 43));
    setAutoFillBackground(true);
    setPalette(pal);

    m_noMailtitleLabel->setStyleSheet("color:#D9000000"); // 85% => D9, 255,255,255 => FFFFFF
}

void NoMailDialog::closeNoMailWindow()
{
    reject();
//    emit noMailWindowClose();
}

SendMailDialog::SendMailDialog(QWidget *parent) : QDialog(parent)
  , m_titleTextLabel(new QLabel())
  , m_closeButton (new QPushButton())
  , m_titleHBoxLayout (new QHBoxLayout())
  , m_mailSelectLabel (new QLabel())
  , m_mailSelectCombobox (new QComboBox())
  , m_mailSelectHBoxLayout (new QHBoxLayout())
  , m_cancelButton (new QPushButton())
  , m_confirmButton (new QPushButton())
  , m_buttonsHBoxLayout (new QHBoxLayout)
  , m_mainVBoxLayout (new QVBoxLayout())
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
    setFixedSize(SendMailWindowWidth, SendMainWindowHeight);
}

void SendMailDialog::initLayout()
{
    m_titleTextLabel->setText(tr("Scanner"));

    m_closeButton->setIcon (QIcon::fromTheme (ICON_THEME_CLOSE));
    m_closeButton->setToolTip(tr("Close"));
    m_closeButton->setFixedSize(30, 30);
    m_closeButton->setIconSize (QSize(16, 16));
    m_closeButton->setProperty("isWindowButton", 0x2);
    m_closeButton->setProperty("useIconHighlightEffect", 0x8);
    m_closeButton->setFlat(true);

    m_titleHBoxLayout->setSpacing(0);
    m_titleHBoxLayout->addSpacing(12);
    m_titleHBoxLayout->addWidget(m_titleTextLabel);
    m_titleHBoxLayout->addStretch();
    m_titleHBoxLayout->addWidget(m_closeButton);
    m_titleHBoxLayout->setAlignment(Qt::AlignCenter);
    m_titleHBoxLayout->setContentsMargins(0, 5, 4, 5);

    m_mailSelectLabel->setText(tr("Select email client"));
    m_mailSelectLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_mailSelectLabel->setFixedSize(98, 22);
    QFont ft;
    ft.setPixelSize(14);
    m_mailSelectLabel->setFont(ft);

    m_mailSelectHBoxLayout->setSpacing(0);
    m_mailSelectHBoxLayout->addWidget(m_mailSelectLabel);
    m_mailSelectHBoxLayout->addSpacing(8);
    m_mailSelectHBoxLayout->addWidget(m_mailSelectCombobox);
    m_mailSelectHBoxLayout->setContentsMargins(24, 0, 24, 0);

    m_cancelButton->setFixedSize(QSize(96, 32));
    m_cancelButton->setText(tr("Cancel"));

    m_confirmButton->setFixedSize(QSize(96, 32));
    m_confirmButton->setText(tr("Confirm"));
    m_confirmButton->setFocus();
    m_confirmButton->setShortcut( QKeySequence::InsertParagraphSeparator);
    m_confirmButton->setShortcut(Qt::Key_Enter);
    m_confirmButton->setShortcut(Qt::Key_Return);

    m_buttonsHBoxLayout->setSpacing(0);
    m_buttonsHBoxLayout->addSpacing(152);
    m_buttonsHBoxLayout->addWidget(m_cancelButton);
    m_buttonsHBoxLayout->addSpacing(12);
    m_buttonsHBoxLayout->addWidget(m_confirmButton);
    m_buttonsHBoxLayout->setContentsMargins(0, 0, 24, 24);

    m_mainVBoxLayout->setSpacing(0);
    m_mainVBoxLayout->addLayout(m_titleHBoxLayout);
    m_mainVBoxLayout->addSpacing(16);
    m_mainVBoxLayout->addLayout(m_mailSelectHBoxLayout);
    m_mainVBoxLayout->addSpacing(24);
    m_mainVBoxLayout->addLayout(m_buttonsHBoxLayout);
    m_mainVBoxLayout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(m_mainVBoxLayout);
}

void SendMailDialog::initConnect()
{
    connect(m_closeButton, &QPushButton::clicked, this, &SendMailDialog::close);

    connect(m_cancelButton, &QPushButton::clicked, this, &SendMailDialog::closeSendMailWindowSlot);

    connect(m_confirmButton, &QPushButton::clicked, [this](){

        int currentIndex = m_mailSelectCombobox->currentIndex();
        KyInfo() << "current index = " << currentIndex;

        openSelectMailClientSlot(m_desktopName[currentIndex]);
        this->accept();
    });

//    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, &SendMailDialog::themeChangedBlack);
//    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, &SendMailDialog::themeChangedWhite);
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
void SendMailDialog::setMailSelectComboboxItems()
{
    int i = 0;

    AppList *maillist = getAppIdList(MailType);
    if (maillist) {
        KyInfo() << "mail list = " << maillist;

        for (i = 0; maillist[i].appid != NULL; ++i) {
            QString single(maillist[i].appid);
            KyInfo() << "maillist desktopName = " << single;

            m_desktopName.append(single);
            QByteArray ba = QString(DesktopFilePath + single).toUtf8();
            GDesktopAppInfo *mailinfo = g_desktop_app_info_new_from_filename(ba.constData());
            QString appname = g_app_info_get_name(G_APP_INFO(mailinfo));
            const char *iconname = g_icon_to_string(g_app_info_get_icon(G_APP_INFO(mailinfo)));

            QIcon appicon;

            if (QIcon::hasThemeIcon(QString(iconname))) {
                appicon = QIcon::fromTheme(QString(iconname));
            }

            m_mailSelectCombobox->addItem(appicon, appname);
            m_mailSelectCombobox->setIconSize(QSize(20, 20));
            m_mailSelectCombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

            free(maillist[i].appid);
        }

        free(maillist);
    }
}

void SendMailDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect(), 0, 0);
    QStyleOption opt;
    opt.init(this);

    QColor mainColor;
    mainColor = opt.palette.color(QPalette::Window);

    p.fillPath(rectPath,QBrush(mainColor));
}


void SendMailDialog::closeSendMailWindowSlot()
{
    reject();
    //    emit sendMailWindowClose();
}

void SendMailDialog::openSelectMailClientSlot(QString name)
{
    QFile aFile(DesktopFilePath + name);
    QString cmd(BashType);
    QString str;
    QStringList  arglists;
    QString mailPicture(MailPicturePath);

    if (!aFile.exists()) {
        KyInfo() << DesktopFilePath << name << " no exists!";
    }
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        KyInfo() << DesktopFilePath << name << " open false!";
    }

    QTextStream aStream(&aFile);
    aStream.setAutoDetectUnicode(true);
    while (!aStream.atEnd()) {
        str = aStream.readLine();
        if (str.startsWith("Exec=", Qt::CaseSensitive))
            break;
    }
    str = str.section("Exec=", 1, 1);
    str = str.section(" ", 0, 0);
    KyInfo() << "exec str = " << str;
    QProcess *process = new QProcess();

    arglists << "-c";
    arglists << str;
    KyInfo() << "str = " << str;
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
    KyInfo() << "cmd = " << cmd << arglists;

    if (str == "evolution") {
        process->start("evolution", arglists);
        return;
    }
    process->start(cmd, arglists);

    this->reject();
}

void SendMailDialog::themeChangedWhite()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(255, 255, 255));
    setAutoFillBackground(true);
    setPalette(pal);

}

void SendMailDialog::themeChangedBlack()
{
    QPalette pal(palette());
    pal.setColor(QPalette::Background, QColor(47, 44, 43));
    setAutoFillBackground(true);
    setPalette(pal);
}

