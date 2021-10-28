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

#define MailWindowWidth 320
#define MainWindowHeight 260


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
    QPushButton *m_closeButton;
    QLabel *m_titleLabel;
    QTextEdit *m_textEdit;
    QFrame *m_lineFrame;
    QPushButton *m_confirmButton;
    QPushButton *m_cancelButton;
    QHBoxLayout *m_buttonsHBoxLayout;
    QHBoxLayout *m_closeHBoxLayout;
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

    void setMailButtonList();
    void openMail(QString name);

private:
    QLabel *m_titleLabel;
    QPushButton *m_closeButton;
    QHBoxLayout *m_closeHBoxLayout;
    QHBoxLayout *m_titleHBoxLayout;
    QVBoxLayout *m_mainVBoxLayout;
    QVBoxLayout *m_addMailVBoxLayout;
    QScrollArea *m_scrollArea;
    QWidget *widget;
    QList<QPushButton *> m_mailButtonList;
    QList<QString> m_desktopName;

Q_SIGNALS:
    void sendMailWindowClose();

public slots:
    void mailButtonClickedSlot();
    void closeSendMailWindowSlot();

    void themeChangedWhite();
    void themeChangedBlack();
};


#endif // SENDMAIL_H
