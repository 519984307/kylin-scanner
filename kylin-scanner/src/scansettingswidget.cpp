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
#include "scansettingswidget.h"
#include "include/common.h"
#include <ukui-log4qt.h>

#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidgetList>
#include <QApplication>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QDateTime>

ScanSettingsWidget::ScanSettingsWidget(QWidget *parent) :
    QWidget(parent),
    m_scanButton(new QPushButton()),
    m_deviceSettingsLabel(new QLabel()),
    m_deviceLabel(new QLabel()),
    m_deviceComboBox(new QComboBox()),
    m_pageNumberLabel(new QLabel()),
    m_pageNumberComboBox(new QComboBox()),
    m_timeLabel(new QLabel()),
    m_timeComboBox(new QComboBox()),
    m_typeLabel(new QLabel()),
    m_typeComboBox(new QComboBox()),
    m_colorLabel(new QLabel()),
    m_colorComboBox(new QComboBox()),
    m_resolutionLabel(new QLabel()),
    m_resolutionComboBox(new QComboBox()),
    m_fileSettingsLabel(new QLabel()),
    m_sizeLabel(new QLabel()),
    m_sizeComboBox(new QComboBox()),
    m_formatLabel(new QLabel()),
    m_formatComboBox(new QComboBox()),
    m_nameLabel(new QLabel()),
    m_nameEdit(new QLineEdit()),
    m_saveLabel(new QLabel()),
    m_saveButton(new QPushButton()),
    m_settingsFormLayout(new QFormLayout()),
    m_emailButton(new QPushButton()),
    m_SaveAsButton(new QPushButton()),
    m_buttonsHLayout(new QHBoxLayout()),
    m_mainVLayout(new QVBoxLayout(this))
{
    setupGui();
    initConnect();
}

/**
 * @brief ScanSettingsWidget::paintEvent
 * Set background color
 * @param event
 */
void ScanSettingsWidget::paintEvent(QPaintEvent *event)
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

void ScanSettingsWidget::selectSaveDirectorySlot()
{
    if (currentSaveDirectory.isEmpty()) {
        currentSaveDirectory = QDir::homePath() ;
    }

    QString midPath = currentSaveDirectory;
    QString dlgTitle = tr("Select a directory");
    QString selectedDir = QFileDialog::getExistingDirectory(this, dlgTitle, currentSaveDirectory, QFileDialog::ShowDirsOnly);
    KyInfo() << "selected directory: " << selectedDir;

    if (!selectedDir.isEmpty()) {
        QFileInfo file(selectedDir);
        if (file.permission(QFileDevice::WriteUser | QFileDevice::ReadGroup)) {
            KyInfo() << "The user could read and write " << selectedDir;

            currentSaveDirectory = selectedDir;
            setSaveButtonAttributes(m_saveButton, currentSaveDirectory, ScanSettingsButtonElideWidth);
        } else {
            if (file.isWritable()) {
                KyInfo() << "The user can read and write " << selectedDir;

                currentSaveDirectory = selectedDir;
                setSaveButtonAttributes(m_saveButton, currentSaveDirectory, ScanSettingsButtonElideWidth);

                return;
            }
            KyInfo() << "The user can't read and write " << selectedDir;

            QString msg = tr("Currently user has no permission to modify directory ") + selectedDir;
            warnMsg(msg);
        }
    }
}

void ScanSettingsWidget::deviceCurrentTextChangedSlot(QString text)
{
    // Need this, for remove the only one scan device to show dialog
    g_sane_object->userInfo.name = text;
    int curTextLen = m_deviceComboBox->currentText().length();

    KyInfo() << "device name: " << g_sane_object->userInfo.name;
    KyInfo() << "Text: " << m_deviceComboBox->currentText() << "len= " << curTextLen << "isWork= " << deviceComboBoxTextChangedIsWork;

    if (! deviceComboBoxTextChangedIsWork) {
        KyInfo() << "Device changed before openSaneDevice, so this function will not work.";
        return;
    }

    KyInfo() << "Device changed, therefore, we need open device again.";

    if ( curTextLen >= 20) {
        m_deviceComboBox->setToolTip(m_deviceComboBox->currentText());
    } else {
        m_deviceComboBox->setToolTip("");
    }

    // index is chosen by user manually, or default(0)
    int index = m_deviceComboBox->currentIndex();
    if (index == -1) {
        index = 0;
    }

    g_sane_object->userInfo.deviceNameIndex = index;

    KyInfo() << "[" << index << "]";

    // while switch scan device, we should open the scan device to get some parameters
    g_sane_object->openSaneDeviceForPage(index);

    updateSettingsForSwitchDevices();
}

/**
 * @brief ScanSettingsWidget::pageNumberCurrentTextChangedSlot
 * @param text
 *
 */
void ScanSettingsWidget::pageNumberCurrentTextChangedSlot(QString text)
{
    int retCompare = QString::compare(text, tr("Multiple"), Qt::CaseInsensitive);
    if (retCompare == 0) {
        // multiple scan
        g_sane_object->userInfo.pageNumber = tr("Multiple");
        showTimeRow();
    } else {
        // simple scan
        g_sane_object->userInfo.pageNumber = tr("Single");
        hideTimeRow();
    }
}

void ScanSettingsWidget::timeCurrentTextChangedSlot(QString text)
{
    g_sane_object->userInfo.time = text;

    KyInfo() << "userInfo.time = " << text;
}

void ScanSettingsWidget::typeCurrentTextChangedSlot(QString text)
{
    g_sane_object->userInfo.type = text;

    KyInfo() << "userInfo.type = " << text;

}

void ScanSettingsWidget::colorCurrentTextChangedSlot(QString text)
{
    if (0 == QString::compare(text, tr("Color"), Qt::CaseInsensitive)) {
        g_sane_object->userInfo.color = "Color";
    } else if(0 == QString::compare(text, tr("Lineart"), Qt::CaseInsensitive)) {
        g_sane_object->userInfo.color = "Lineart";
    } else {
        g_sane_object->userInfo.color = "Gray";
    }

    KyInfo() << "userInfo.color = " << text;
}

void ScanSettingsWidget::resolutionCurrentTextChangedSlot(QString text)
{
    if ((0 == text.compare(tr("4800 dpi"), Qt::CaseInsensitive))
         || (0 == text.compare(tr("2400 dpi"), Qt::CaseInsensitive))
         || (0 == text.compare(tr("1200 dpi"), Qt::CaseInsensitive))) {

        QString msg = tr("This resolution will take a loog time to scan, please choose carelly.");
        warnMsg(msg);
    }

    g_sane_object->userInfo.resolution = text;

    KyInfo() << "userInfo.resolution = " << text;
}

void ScanSettingsWidget::sizeCurrentTextChangedSlot(QString text)
{
    g_sane_object->userInfo.size = text;
    KyInfo() << "userInfo.size = " << text;
}

void ScanSettingsWidget::formatCurrentTextChangedSlot(QString text)
{
    g_sane_object->userInfo.format = text;
    KyInfo() << "userInfo.format = " << text;
}

void ScanSettingsWidget::nameCurrentTextChangedSlot(QString text)
{
    QString msg;
    if (m_nameEdit->text().contains(QChar('/'), Qt::CaseInsensitive)) {
        msg = tr("cannot contain '/' character.");
        warnMsg(msg);
        m_nameEdit->cursorBackward(true, 1); // move left a character
        m_nameEdit->del(); // delete right or selected text
    }
    if (m_nameEdit->text().startsWith(QChar('.'), Qt::CaseInsensitive)) {
        msg = tr("cannot save as hidden file.");
        warnMsg(msg);
        m_nameEdit->cursorBackward(true, 1);
        m_nameEdit->del();
    }

    setNameEditTooltip();
}

void ScanSettingsWidget::scanButtonClickedSlot()
{
    //emit scanButtonClickedSignal();
    g_user_signal->startScanOperation();
}

void ScanSettingsWidget::fontSizeChanged()
{
    setLabelAttributes(m_deviceLabel, tr("Device"));
    setLabelAttributes(m_pageNumberLabel, tr("Pages"));
    setLabelAttributes(m_timeLabel, tr("Time"));
    setLabelAttributes(m_typeLabel, tr("Type"));
    setLabelAttributes(m_colorLabel, tr("Color"));
    setLabelAttributes(m_resolutionLabel, tr("Resolution"));
    setLabelAttributes(m_sizeLabel, tr("Size"));
    setLabelAttributes(m_formatLabel, tr("Format"));
    setLabelAttributes(m_nameLabel, tr("Name"));
    setLabelAttributes(m_saveLabel, tr("Save"));

    setSaveButtonAttributes(m_saveButton, currentSaveDirectory, ScanSettingsButtonElideWidth);
    setSaveButtonAttributes(m_emailButton, tr("Mail to"), ScanSettingsMailAndButtonElideWidth);
    setSaveButtonAttributes(m_SaveAsButton, currentSaveAsDirectory, ScanSettingsMailAndButtonElideWidth);
}

void ScanSettingsWidget::themeChangedBlackSlot()
{

}

void ScanSettingsWidget::themeChangedWhiteSlot()
{
}

void ScanSettingsWidget::setupGui()
{
    setMaximumWidth(ScanSettingsWidgetWidth);

    QLabel *scanButtonLeftLabel = new QLabel();
    QLabel *scanButtonRightLabel = new QLabel();
    QHBoxLayout *scanButtonHLayout = new QHBoxLayout();

    scanButtonLeftLabel->setPixmap(QPixmap(":/default-connect-page/begin-scan.svg"));
    scanButtonRightLabel->setText(tr("Begin Scan"));

    scanButtonHLayout->addStretch();
    scanButtonHLayout->addWidget(scanButtonLeftLabel);
    scanButtonHLayout->addSpacing(4);
    scanButtonHLayout->addWidget(scanButtonRightLabel);
    scanButtonHLayout->addStretch();

    m_scanButton->setLayout(scanButtonHLayout);
    m_scanButton->setFixedSize(ScanSettingsWidgetScanButtonSize);
    m_scanButton->setCursor(Qt::PointingHandCursor);
    m_scanButton->setStyleSheet("QPushButton{background: rgba(55, 144, 250, 1); border-radius: 6px;}"
                                "QPushButton:hover{background: rgba(55, 144, 250, 0.8);border-radius: 6px;}"
                                "QPushButton:pressed{background: rgba(36, 109, 212, 1);border-radius: 6px;}"
                                );

    m_deviceComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_deviceSettingsLabel->setText(tr("Scanner device"));

    m_pageNumberComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_timeComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_typeComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_colorComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_resolutionComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_fileSettingsLabel->setText(tr("File settings"));

    m_sizeComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_formatComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_nameEdit->setText(tr("scanner01"));
    m_nameEdit->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_saveButton->setFixedSize(ScanSettingsWidgetComboboxSize);
    if (currentSaveDirectory.isEmpty()) {
        currentSaveDirectory = QDir::homePath();
        KyInfo() << currentSaveDirectory;
    }

    m_settingsFormLayout->setSpacing(7);
    m_settingsFormLayout->setVerticalSpacing(8);
    m_settingsFormLayout->addRow(m_scanButton);
    m_settingsFormLayout->addRow(m_deviceSettingsLabel);
    m_settingsFormLayout->addRow(m_deviceLabel, m_deviceComboBox);
    m_settingsFormLayout->addRow(m_pageNumberLabel, m_pageNumberComboBox);
    //m_settingsFormLayout->addRow(m_timeLabel, m_timerComboBox);
    m_settingsFormLayout->addRow(m_typeLabel, m_typeComboBox);
    m_settingsFormLayout->addRow(m_colorLabel, m_colorComboBox);
    m_settingsFormLayout->addRow(m_resolutionLabel, m_resolutionComboBox);
    m_settingsFormLayout->addRow(m_fileSettingsLabel);
    m_settingsFormLayout->addRow(m_sizeLabel, m_sizeComboBox);
    m_settingsFormLayout->addRow(m_formatLabel, m_formatComboBox);
    m_settingsFormLayout->addRow(m_nameLabel, m_nameEdit);
    m_settingsFormLayout->addRow(m_saveLabel, m_saveButton);
    m_settingsFormLayout->setLabelAlignment(Qt::AlignRight);
    m_settingsFormLayout->setFormAlignment(Qt::AlignLeft);
    m_settingsFormLayout->setContentsMargins(0, 0, 0, 0);

    m_emailButton->setFixedSize(ScanSettingsWidgetButtonSize);
    m_emailButton->setCursor(Qt::PointingHandCursor);

    m_SaveAsButton->setFixedSize(ScanSettingsWidgetButtonSize);
    currentSaveAsDirectory = tr("Save as");
    m_SaveAsButton->setCursor(Qt::PointingHandCursor);

    fontSizeChanged();

    m_buttonsHLayout->setSpacing(0);
    m_buttonsHLayout->addWidget(m_emailButton);
    m_buttonsHLayout->addSpacing(12);
    m_buttonsHLayout->addWidget(m_SaveAsButton);
    m_buttonsHLayout->setContentsMargins(0, 0, 0, 0);

    m_mainVLayout->setSpacing(0);
    m_mainVLayout->addSpacing(17);
    m_mainVLayout->addLayout(m_settingsFormLayout);
    m_mainVLayout->addLayout(m_buttonsHLayout);

    setLayout(m_mainVLayout);
}

void ScanSettingsWidget::initConnect()
{
    connect(m_deviceComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::deviceCurrentTextChangedSlot);
    connect(m_pageNumberComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::pageNumberCurrentTextChangedSlot);
    connect(m_timeComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::timeCurrentTextChangedSlot);
    connect(m_typeComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::typeCurrentTextChangedSlot);
    connect(m_colorComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::colorCurrentTextChangedSlot);
    connect(m_resolutionComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::resolutionCurrentTextChangedSlot);
    connect(m_sizeComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::sizeCurrentTextChangedSlot);
    connect(m_formatComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::formatCurrentTextChangedSlot);
    connect(m_nameEdit, &QLineEdit::textChanged, this, &ScanSettingsWidget::nameCurrentTextChangedSlot);

    connect(m_scanButton, &QPushButton::clicked, this, &ScanSettingsWidget::scanButtonClickedSlot);

    connect(g_user_signal, &GlobalUserSignal::fontSizeChangedSignal, this, [=](){
        fontSizeChanged();
    });

    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, [=](){
        themeChangedBlackSlot();
    });
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, [=](){
        themeChangedWhiteSlot();
    });

    // select save directory
    connect(m_saveButton, &QPushButton::clicked, this, &ScanSettingsWidget::selectSaveDirectorySlot);

}

void ScanSettingsWidget::showTimeRow()
{
    int countRow = m_settingsFormLayout->rowCount();
    KyInfo() << "countRow = " << countRow;

    if (countRow == 12) {
        if (m_timeLabel) {
            m_timeLabel = new QLabel;
            setLabelAttributes(m_timeLabel, tr("Time"));
        }
        if (m_timeComboBox) {
            m_timeComboBox = new QComboBox;
            m_timeComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);
            connect(m_timeComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::timeCurrentTextChangedSlot);
        }
        m_settingsFormLayout->insertRow(4, m_timeLabel, m_timeComboBox);

        updateTimeSettings();
    }
}

void ScanSettingsWidget::hideTimeRow()
{
    int countRow = m_settingsFormLayout->rowCount();
    KyInfo() << "countRow = " << countRow;

    if (countRow == 13) {
        m_settingsFormLayout->removeRow(4);
    }
}

void ScanSettingsWidget::updateDeviceSettings()
{
    QStringList deviceStringList;

    bool saneStatus = g_sane_object->getSaneStatus();
    KyInfo() << "getSaneTypes " << saneStatus;

    if (! saneStatus) {
        // not find scan device
        deviceStringList << tr("No available scanners");
    } else {
        deviceStringList = g_sane_object->getSaneNames();
    }

    KyInfo() << "Devices :  " << deviceStringList;
    setComboboxAttributes(m_deviceComboBox, deviceStringList);
}

/**
 * @brief ScanSettingsWidget::updatePageNumberSettings
 * Single page:
 *      user choose it, not get from scanners,
 *      only scan once.
 *
 * Multiple page:
 *      user choose it, not get from scanners
 *      scan lots of times, only stop by clicked scan button again or not pages in ADF scanners
 */
void ScanSettingsWidget::updatePageNumberSettings()
{
    QStringList pageNumberStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    pageNumberStringList << tr("Single") << tr("Multiple");

    KyInfo() << "PageNumber :  " << pageNumberStringList;

    m_pageNumberComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_pageNumberComboBox, pageNumberStringList);
}

void ScanSettingsWidget::updateTimeSettings()
{
    QStringList timerStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    timerStringList << tr("3s") << tr("5s") << tr("7s") << tr("10s") << tr("15s");

    KyInfo() << "Timer :  " << timerStringList;

    m_timeComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_timeComboBox, timerStringList);

}

void ScanSettingsWidget::updateTypeSettings()
{
    QStringList typeStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    if (! saneStatus) {
        typeStringList << tr("Flatbed") << tr("ADF");
    } else {
        typeStringList = g_sane_object->getSaneTypes();
    }

    KyInfo() << "Type :  " << typeStringList;

    m_typeComboBox->setEnabled(saneStatus);
    if (g_sane_object->haveSourceFlag == 0) {
        // For ESCL scanner device, which not get source type parameter
        m_typeComboBox->setEnabled(false);
    }
    setComboboxAttributes(m_typeComboBox, typeStringList);
}

void ScanSettingsWidget::updateColorSettings()
{
    QStringList colorStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    if (! saneStatus) {
        colorStringList << tr("Gray") << tr("Color") << tr("Lineart");
    } else {
        colorStringList = g_sane_object->getSaneColors();
    }



    // segmentation fault
    if (! colorStringList.isEmpty()) {
        KyInfo() << "Color: " << colorStringList;
    }

    m_colorComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_colorComboBox, colorStringList);
}

void ScanSettingsWidget::updateResolutionSettings()
{
    QStringList resolutionStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    if (! saneStatus) {
        resolutionStringList << tr("75 dpi") << tr("100 dpi") << tr("150 dpi");
    } else {
        resolutionStringList = g_sane_object->getSaneResolutions();
    }

    if (! resolutionStringList.isEmpty()) {
        KyInfo() << "resolution: " << resolutionStringList;
    }

    m_resolutionComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_resolutionComboBox, resolutionStringList);
}

void ScanSettingsWidget::updateSizeSettings()
{
    QStringList sizeStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    if (! saneStatus) {
        sizeStringList << tr("A4") << tr("A5");
    } else {
        sizeStringList = g_sane_object->getSaneSizes();
    }

//    KyInfo() << "size: " << sizeStringList;

    m_sizeComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_sizeComboBox, sizeStringList);
}

/**
 * @brief ScanSettingsWidget::updateFormatSettings
 * format is choosed by user, so we need set it default.
 */
void ScanSettingsWidget::updateFormatSettings()
{
    QStringList formatStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    formatStringList << "jpg" << "png" << "pdf" << "bmp";

//    KyInfo() << "format: " << formatStringList;

    m_formatComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_formatComboBox, formatStringList);

}

/**
 * @brief ScanSettingsWidget::updateNameTextSettings
 * Consider multiple scan, so we set nameEdit timestamp.
 */
void ScanSettingsWidget::updateNameTextSettings()
{
    bool saneStatus = g_sane_object->getSaneStatus();

    QDateTime currentTime = QDateTime::currentDateTime();
    KyInfo() << "current time: " <<currentTime;

    QString currentTimeString = currentTime.toString("yyyy_MM_dd-hh_mm_ss_zzz");
    KyInfo() << currentTimeString;

    //int currentTimestamp = currentTime.toTime_t();
    //m_nameEdit->setText(QString::number(currentTimestamp));
    m_nameEdit->setText(currentTimeString);
    m_nameEdit->setAlignment(Qt::AlignLeft);

    m_nameEdit->setEnabled(saneStatus);
}


/**
 * @brief ScanSettingsWidget::updateSettings
 * update scan settings while detect and open the default scanner successfully
 */
void ScanSettingsWidget::updateSettings()
{
    updateDeviceSettings();
    updatePageNumberSettings();
    updateTimeSettings();
    updateTypeSettings();
    updateColorSettings();
    updateResolutionSettings();
    updateSizeSettings();
    updateFormatSettings();
    updateNameTextSettings();
}

/**
 * @brief ScanSettingsWidget::updateSettingsForSwitchDevices
 * update scan settings except some paramers, such as device setting, and so on
 */
void ScanSettingsWidget::updateSettingsForSwitchDevices()
{
//    updatePageNumberSettings();
//    updateTimerSettings();
    updateTypeSettings();
    updateColorSettings();
    updateResolutionSettings();
    updateSizeSettings();
    updateFormatSettings();
    updateNameTextSettings();
}

void ScanSettingsWidget::setDeviceComboBoxTextChangedIsWork(bool isWork)
{
    deviceComboBoxTextChangedIsWork = isWork;
}

void ScanSettingsWidget::setLabelAttributes(QLabel *label, const QString &text)
{
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setFixedWidth(ScanSettingsLabelElideWidth);
    int elideWidth = ScanSettingsLabelElideWidth;

    QFontMetrics fontMetrics(label->font()); // label font width

    //KyInfo() << "label width: " << label->width() << "font width: " << fontMetrics.width(text);

    if (fontMetrics.width(text) > label->width()) {
        QString elideText = QFontMetrics(label->font()).elidedText(text, Qt::ElideRight, elideWidth);
        label->setText(elideText);
        label->setToolTip(text);
    } else {
        label->setText(text);
        label->setToolTip("");
    }
}


void ScanSettingsWidget::setSaveButtonAttributes(QPushButton *button, const QString &text, int elideWidth)
{
    QFontMetrics fontMetrics(button->font());

    if (fontMetrics.width(text) > elideWidth) {
        QString elideText = QFontMetrics(button->font()).elidedText(text, Qt::ElideRight, elideWidth);
        button->setText(elideText);
        button->setToolTip(text);
    } else {
        button->setText(text);
        button->setToolTip("");
    }
}

/**
 * @brief ScanSettingsWidget::setComboboxAttributes
 * @param combobox
 * @param strList
 *
 * Add tooltips in every rows while len(str) >= 20
 */
void ScanSettingsWidget::setComboboxAttributes(QComboBox *combobox, QStringList strList)
{
    QListView *listView = new QListView();
    QStandardItemModel *model = new QStandardItemModel();

    combobox->clear();

    for (int i = 0; i < strList.size(); ++i) {
        QStandardItem *item = new QStandardItem(strList.at(i));;
        int curTextLen = strList.at(i).length();

        if ( curTextLen >= 20) {
            item->setToolTip(strList.at(i));
        } else {
            item->setToolTip("");
        }

        model->appendRow(item);
    }
    combobox->setModel(model);
    combobox->setInsertPolicy(QComboBox::NoInsert);
    combobox->setFocusPolicy(Qt::ClickFocus);
    combobox->setView(listView);
}

void ScanSettingsWidget::setNameEditTooltip()
{
    int lenTextName = m_nameEdit->text ().length ();
    const int MaxLengthTextName = 256;

    // Because image format len(.jpg) = 4
    if (lenTextName >= MaxLengthTextName - 4) {
        m_nameEdit->setToolTip (tr("Scanning images's length cannot be large than 252"));
    } else {
        m_nameEdit->setToolTip ("");
    }
}

void ScanSettingsWidget::warnMsg(QString msg)
{
    QMessageBox *msgBox = new QMessageBox();

    msgBox->setText(msg);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    msgBox->setWindowTitle(tr("Scanner"));
    msgBox->setStandardButtons(QMessageBox::Yes);
    msgBox->setContextMenuPolicy(Qt::NoContextMenu);
    msgBox->button(QMessageBox::Yes)->setText(tr("Yes"));

    // center saveDialog in mainwindow
    QWidget *widget = nullptr;
    QWidgetList widgetList = QApplication::allWidgets();
    for (int i=0; i<widgetList.length(); ++i) {
        if (widgetList.at(i)->objectName() == "MainWindow") {
            widget = widgetList.at(i);
        }
    }
    if (widget) {
        msgBox->setParent(widget); // this is important
        QRect rect = widget->geometry();
        int x = rect.x() + rect.width()/2 - msgBox->width()/2;
        int y = rect.y() + rect.height()/2 - msgBox->height()/2;
        msgBox->move(x,y);
    }

    msgBox->exec();
}
