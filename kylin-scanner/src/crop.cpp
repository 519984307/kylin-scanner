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

#include "crop.h"

CropLabel::CropLabel(QLabel *parent)
    : QLabel(parent)
{
    startX = 0;
    startY = 0;
    endX = 0;
    endY = 0;
}

CropLabel::~CropLabel()
{

}

void CropLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event); // draw label background

    QPainter painter(this);
    QPen pen(QColor(55, 144, 250), 1);
    painter.setPen(pen);

    QPainterPath borderPath, tailorPath, outPath;

    borderPath.setFillRule(Qt::WindingFill);
    borderPath.addRect(0, 0, this->width(), this->height()); // need judge

    QRect rect(startX, startY, endX - startX, endY - startY);
    tailorPath.addRect(rect);

#if 0
    QPixmap tlPixmap("/home/yushuoqi/aaa.pnm");
    QRect tlRect(startX, startX, 4, 4);
    tlPixmap.scaled(4, 4, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawRoundedRect(tlRect, 4, 4, Qt::AbsoluteSize);
    painter.drawPixmap(tlRect,  tlPixmap);
#endif

    painter.drawPath(tailorPath);

    outPath = borderPath.subtracted(tailorPath);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(outPath, QColor(0, 0, 0, 100));
}

void CropLabel::mousePressEvent(QMouseEvent *event)
{
//    QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));

    if (event->button() == Qt::LeftButton) {
        startX = event->pos().x();
        startY = event->pos().y();
        endX = event->pos().x();
        endY = event->pos().y();

        isPressed = true;
    }
}

void CropLabel::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;

    if (event->buttons() & Qt::LeftButton) { // For avoid rightButton
        endX = event->pos().x();
        endY = event->pos().y();
    }

    endX = judgePosition(endX, 0, this->width());
    endY = judgePosition(endY, 0, this->height());
    update();

//    QApplication::restoreOverrideCursor();
}

void CropLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (isPressed && (event->buttons() & Qt::LeftButton)) {
        endX = event->pos().x();
        endY = event->pos().y();

        update();
    }
}

int CropLabel::judgePosition(int origin, int min, int max)
{
    qDebug() << "origin = " << origin
             << "min = " << min
             << "max = " << max;

    if (origin < min) {
        // left border
        return min;
    }
    if (origin > max) {
        // right border: In order to show green border, so need max-1
        return max-1;
    }

    return origin;
}

void CropLabel::initStartAndEndXY()
{
    startX = 0;
    startY = 0;
    endX = 0;
    endY = 0;

}
