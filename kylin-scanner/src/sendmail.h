/*
* Copyright (C) 2020, KylinSoft Co., Ltd.
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

#ifndef SENDMAIL_H
#define SENDMAIL_H


#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QFrame>
#include <QDialog>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QProcess>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QBitmap>
#include <QPixmap>
#include <QFileInfo>
#include <QFileIconProvider>
#include <qmath.h>

#include  <ukui-log4qt.h>

#include "utils/xatom-helper.h"
#include "globalsignal.h"
#include "include/common.h"
#include "include/theme.h"

#ifdef signals
#undef signals
#endif
extern "C" {
#include <glib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
}

#define MailType "x-scheme-handler/mailto"
#define DesktopFilePath "/usr/share/applications/"
#define MailPicturePath					"~/.config/kylin-scanner/images/mail.jpg"

#define NoMailWindowWidth 380
#define NoMainWindowHeight 216

#define SendMailWindowWidth 380
#define SendMainWindowHeight 176


typedef struct _Applist {
    char *appid;
} AppList;
AppList *getAppIdList(const char *contentType);

typedef struct _AppInfo {
    GAppInfo *item;
} Appinfo;
Appinfo *_getAppList(const char *contentType);

class NoMailDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NoMailDialog(QWidget *parent = nullptr);
    ~NoMailDialog();

    void initWindow();
    void initLayout();
    void initConnect();

private:
    QLabel *m_noMailTitleTextLabel;
    QPushButton *m_noMailCloseButton;
    QHBoxLayout *m_noMailTitleHBoxLayout;

    QLabel *m_noMaillogoLabel;
    QLabel *m_noMailtitleLabel;
    QHBoxLayout *m_noMailLogoTitleHBoxLayout;

    QLabel *m_noMailInfoLabel;
    QHBoxLayout *m_noMailInfoHBoxLayout;

    QPushButton *m_cancelButton;
    QPushButton *m_installButton;
    QHBoxLayout *m_noMailButtonsHBoxLayout;

    QVBoxLayout *m_mainVBoxLayout;

Q_SIGNALS:
   void noMailWindowClose();

private slots:

    void themeChangedWhite();
    void themeChangedBlack();

    void closeNoMailWindow();
};

class SendMailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendMailDialog(QWidget *parent = nullptr);
    ~SendMailDialog();

    void initWindow();
    void initLayout();
    void initConnect();

    void setMailSelectComboboxItems();

private:
    QLabel *m_titleTextLabel;
    QPushButton *m_closeButton;
    QHBoxLayout *m_titleHBoxLayout;

    QLabel *m_mailSelectLabel;
    QComboBox *m_mailSelectCombobox;
    QHBoxLayout *m_mailSelectHBoxLayout;

    QPushButton *m_cancelButton;
    QPushButton *m_confirmButton;
    QHBoxLayout *m_buttonsHBoxLayout;

    QVBoxLayout *m_mainVBoxLayout;

    QList<QString> m_desktopName;

Q_SIGNALS:
    void sendMailWindowClose();

public slots:
    void closeSendMailWindowSlot();
    void openSelectMailClientSlot(QString name);

    void themeChangedWhite();
    void themeChangedBlack();
};


#endif // SENDMAIL_H
