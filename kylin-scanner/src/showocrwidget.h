#ifndef SHOWOCNWIDGET_H
#define SHOWOCRWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QLinearGradient>
#include <QLabel>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "toolbarwidget.h"

class showOcrWidget : public QLabel
{
    Q_OBJECT
public:
    explicit showOcrWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();


public slots:
    void showScanLine();

    void startScan();
    void stopScan();

private:
    QLabel *m_ocrImageLabel;
    QTextEdit *m_ocrTextEdit;
    ToolBarWidget *m_toolbarWidget;

    QVBoxLayout *m_ocrVLayout;

    QTimer *timer;
    int scanHeight = 0;

};

#endif // SHOWOCRWIDGET_H
