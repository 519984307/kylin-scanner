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

#include "globalsignal.h"
#include "toolbarwidget.h"
#include "saneobject.h"
#include "include/common.h"

class showOcrWidget : public QLabel
{
    Q_OBJECT
public:
    explicit showOcrWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;


public slots:
    void showScanLine();

    void startScanSlot();
    void stopScanStop();

    void updateOcrTextEdit();

private:
    QLabel *m_ocrImageLabel;
//    QHBoxLayout *m_ocrImageHLayout;

    QTextEdit *m_ocrTextEdit;
//    QHBoxLayout *m_ocrTextEditHLayout;

    ToolBarWidget *m_toolbarWidget;

    QVBoxLayout *m_ocrVLayout;

    QTimer *timer;
    int scanHeight = 0;

};

#endif // SHOWOCRWIDGET_H
