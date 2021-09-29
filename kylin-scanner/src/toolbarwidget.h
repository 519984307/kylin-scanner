#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "globalsignal.h"
#include "svghandler.h"
#include <ukui-log4qt.h>

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QColor>
#include <QIcon>
#include <QPixmap>

#define ToolBarWidgetColor QColor(249, 249, 249, 0.8*255)
#define ToolBarWidgetBlur 18
#define ToolBarWidgetFixedSize QSize(421, 36)
#define ToolBarWidgetButtonSize QSize(36, 36)
#define ToolBarWidgetFrameSize QSize(1, 16)
#define ToolBarWidgetZoomButtonSize QSize(20, 20)

class ToolBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolBarWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();

    void themeChangedIconBlackSettings();
    void themeChangedIconWhiteSettings();

signals:

protected:
    void paintEvent(QPaintEvent *event) override;


public slots:
    void themeChangedWhiteSlot();
    void themeChangedBlackSlot();

private:

    QPushButton *m_beautyButton;
    QPushButton *m_rectifyButton;
    QPushButton *m_ocrButton;
    QFrame *m_leftFrame;
    QPushButton *m_cropButton;
    QPushButton *m_rotateButton;
    QPushButton *m_mirrorHButton;
    QPushButton *m_watermarkButton;
    QFrame *m_rightFrame;
    QPushButton *m_zoomOutButton;
    QLabel *m_percentageLabel;
    QPushButton *m_zoomInButton;

    QHBoxLayout *m_mainHLayout;
};

#endif // TOOLBARWIDGET_H
