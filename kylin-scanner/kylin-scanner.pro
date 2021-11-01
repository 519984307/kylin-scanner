# QT modules
QT       += \
        core \
        gui \
        KWindowSystem \
        dbus \
        x11extras \
        network \
        printsupport \
        concurrent \
        svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kylin-scanner

TEMPLATE = app

PROJECT_ROOTDIR = $$PWD

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG += \
        c++11 \
        link_pkgconfig

PKGCONFIG   += \
        gio-2.0 \
        gio-unix-2.0 \
        opencv4 \
        gsettings-qt

LIBS += \
        -lpthread \
        -ltesseract \
        -lX11 \
        -L/usr/lib/libukui-log4qt.so.1.0.0 -lukui-log4qt

unix:!macx: LIBS += -L$$PWD/../../../usr/lib/x86_64-linux-gnu/ -lsane

INCLUDEPATH += $$PWD/../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../usr/lib/x86_64-linux-gnu

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
        MY_DEBUG

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/about/about.cpp \
    src/detectpagewidget.cpp \
    src/displaywidget.cpp \
    src/failedpagewidget.cpp \
    src/globalsignal.cpp \
    src/leftimagehandlesuccesspagewidget.cpp \
    src/leftsuccesspagewidget.cpp \
    src/main.cpp \
    src/mainwidget.cpp \
    src/saneobject.cpp \
    src/scandialog.cpp \
    src/scansettingswidget.cpp \
    src/sendmail.cpp \
    src/showimagewidget.cpp \
    src/singleapplication.cpp \
    src/successpagewidget.cpp \
    src/svghandler.cpp \
    src/thumbnaildelegate.cpp \
    src/thumbnailwidget.cpp \
    src/titlebar/titlebar.cpp \
    src/toolbarwidget.cpp \
    src/utils/daemondbus.cpp \
    src/utils/xatom-helper.cpp \
    src/watermarkdialog.cpp

HEADERS += \
    src/about/about.h \
    src/detectpagewidget.h \
    src/displaywidget.h \
    src/failedpagewidget.h \
    src/globalsignal.h \
    src/include/common.h \
    src/include/theme.h \
    src/leftimagehandlesuccesspagewidget.h \
    src/leftsuccesspagewidget.h \
    src/mainwidget.h \
    src/saneobject.h \
    src/scandialog.h \
    src/scansettingswidget.h \
    src/sendmail.h \
    src/showimagewidget.h \
    src/singleapplication.h \
    src/successpagewidget.h \
    src/svghandler.h \
    src/thumbnaildelegate.h \
    src/thumbnailwidget.h \
    src/titlebar/titlebar.h \
    src/toolbarwidget.h \
    src/utils/daemondbus.h \
    src/utils/xatom-helper.h \
    src/watermarkdialog.h

# Manual
DISTFILES += \
    man/kylin-scanner.1 \
    src/icons/waiting/loading1.svg \
    src/icons/waiting/loading2.svg \
    src/icons/waiting/loading3.svg \
    src/icons/waiting/loading4.svg \
    src/icons/waiting/loading5.svg \
    src/icons/waiting/loading6.svg \
    src/icons/waiting/loading7.svg

# UI files
FORMS += \
    src/about/about.ui \
    src/titlebar/titlebar.ui

# Translation
TRANSLATIONS += \
    translations/kylin-scanner_zh_CN.ts \
    translations/kylin-scanner_bo_CN.ts

!system($$PWD/translations/generate_translations_pm.sh): error("Failed to generate pm")
system($$shell_path(cp $$PROJECT_ROOTDIR/translations/*.qm  $$OUT_PWD/))

# custom variable: target
target.files += $$TARGET
target.path = /usr/bin/

# custom variable: desktop
desktop.files += data/kylin-scanner.desktop
desktop.path = /usr/share/applications/

# custom variable: icons
icons.files += data/scanner.png
icons.path = /usr/share/pixmaps/

# custom variable: qm_files
qm_files.files = translations/*.qm
qm_files.path = /usr/share/kylin-scanner/translations/

# Install file in system
INSTALLS += target desktop icons qm_files

RESOURCES += \
    src/icons/icons.qrc \
    src/qss/scrollbar.qrc
