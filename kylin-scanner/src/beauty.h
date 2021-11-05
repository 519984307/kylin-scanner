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

#ifndef BEAUTY_H
#define BEAUTY_H

#include <vector>
#include <iostream>
#include <fstream>
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <opencv2/core/utility.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
using namespace cv;
using namespace std;

#include "globalsignal.h"
#include "saneobject.h"

//#define CV_MINMAX       32

void luminanceContrastCV(Mat src, Mat &dst);
void saturationCV(Mat src, Mat &dst);
void sharpenCV(Mat src, Mat &dst);
void hslCV(Mat src, Mat &dst);

void averageFilterCV(Mat src, Mat &dst);
void gaussianFilterCV(Mat src, Mat &dst);
void medianFilterCV(Mat src, Mat &dst);
void bilateralFilterCV(Mat src, Mat &dst);

void histogramEqualizationCV(Mat src, Mat &dst);
void contrastCV(Mat src, Mat &dst);
void logarithmCV(Mat src, Mat &dst);
void gammaCV(Mat src, Mat &dst);


QImage *sharpen(QImage *origin);
QImage *greyScale(QImage *origin);
QImage *lumimance(int delta, QImage *origin);
QImage *saturation(int delta, QImage *origin);
QImage *warmToned(int delta, QImage *origin);
QImage *coolToned(int delta, QImage *origin);
QImage *drawFrame(QImage *origin, const char *filename);


void oneClickBeauty(const char *filename);


class KYCBeautyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KYCBeautyWidget(QWidget *parent = nullptr);

};

#endif // BEAUTY_H
