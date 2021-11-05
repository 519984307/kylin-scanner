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

#include "rectify.h"

#include <ukui-log4qt.h>

double DegreeTrans(double theta)
{
    double res = theta / CV_PI * 180;
    return res;
}

void rotateImage(Mat src, Mat &img_rotate, double degree)
{
    Point2f center;
    center.x = float(src.cols / 2.0);
    center.y = float(src.rows / 2.0);
    KyInfo() << "center.x" << center.x;
    KyInfo() << "center.y" << center.y;

    int length = 0;
    length = sqrt(src.cols * src.cols + src.rows * src.rows);
    KyInfo() << "length = " << length;

    Mat M = getRotationMatrix2D(center, degree, 1);
//    warpAffine(src, img_rotate, M, Size(length, length), 1, 0, Scalar(255,255,255));//仿射变换，背景色填充为白色

    warpAffine(src, img_rotate, M, img_rotate.size(), 1, 0, Scalar(255, 255, 255));
}

/**
 * @brief CalcDegree 通过霍夫变换计算角度
 * 接近水平[0-5度]和接近垂直[175-185度]都不进行纠偏
 *
 * 算法思想：
 * 1. 读取原始图并转换成灰度图，采用边缘检测算子（如Canny）转换成二值化边缘图像
 * 2. 对该图像进行霍夫变换
 *
 * 只选角度最小的作为旋转角度
 * 角度为正时以左上角为支点逆时针旋转，
 * 角度为负时已左上角为支点顺时针旋转
 * @param srcImage
 * @param dst
 * @return
 */
double CalcDegree(const Mat &srcImage, Mat &dst)
{
    Mat midImage;
    Mat dstImage;
    int Threshold = 300;
    int sizeLineBefore;
    int sizeLineAfter;

    Canny(srcImage, midImage, 50, 200, 3);
    cvtColor(midImage, dstImage, COLOR_GRAY2BGR);

    //通过霍夫变换检测直线
    // 600 dpi: Threshold = 750
    vector<Vec2f> lines;
    HoughLines(midImage, lines, 1, CV_PI / 180, Threshold, 0,
               0);//第5个参数就是阈值，阈值越大，检测精度越高
    //KyInfo() << lines.size() ;

    KyInfo() << "Begin to found good threshold.";
    // 由于图像不同，阈值不好设定，因为阈值设定过高导致无法检测直线，阈值过低直线太多，速度很慢
    // 所以根据阈值由大到小设置了三个阈值(300, 200, 100)，如果经过大量试验后，可以固定一个适合的阈值。
    // While Threshold is too small, lines will be large(>5000).
    // this will make rectify process slower,
    // so we should judge lines.size();
    while (lines.size() > 100 || lines.size() < 10) {
        if (lines.size() >100)
            Threshold += 300;
        else if (lines.size() < 10)
            Threshold -= 50;

        sizeLineBefore = lines.size();

        KyInfo() << "Threshold = " << Threshold << "lines.size = " << lines.size();
        HoughLines(midImage, lines, 1, CV_PI / 180, Threshold, 0, 0);

        sizeLineAfter = lines.size();

        if ((sizeLineBefore > 100) && (sizeLineAfter < 10)) {
            // break this loop
            Threshold += 50;
            HoughLines(midImage, lines, 1, CV_PI / 180, Threshold, 0, 0);
            break;
        }

        if (Threshold <= 0) {
            KyInfo() << "没有检测到直线！" ;
            return ERROR;
        }
    }
    KyInfo() << "after while, Threshold = " << Threshold
             << "lines.size() = " << lines.size();

    // 由于上述的判断可能会导致获取的线条为0，此时需要重新进行定位阈值范围
    if (lines.size() == 0 || lines.size() < 50) {
        KyInfo() << "Threshold = 300 " << "lines = " << lines.size();
        HoughLines(midImage, lines, 1, CV_PI / 180, 300, 0, 0);

        if (lines.size() == 0) {
            KyInfo() << "Threshold = 200 " << "lines = " << lines.size();
            HoughLines(midImage, lines, 1, CV_PI / 180, 200, 0, 0);

            if (lines.size() == 0) {
                KyInfo() << "Threshold = 100 " << "lines = " << lines.size();
                HoughLines(midImage, lines, 1, CV_PI / 180, 100, 0, 0);
            }
        }

    }

    float sum = 0;
    int n = 0;
    //依次画出每条线段
    size_t i = 0;

    int loop = 300;
    if (lines.size() > loop) {
        loop = 300;
    } else {
        loop = lines.size();
    }
    KyInfo() << "lines.size = " << lines.size ()
             << "draw line loop = " << loop;

    int num180 = 0;
    int num0 = 0;
    int num90 = 0;

    //for (i = 0; i < lines.size(); ++i) {
    for (i = 0; i < loop; ++i) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));
        //KyInfo() << i << ", " << "DegreeTrans() = " << DegreeTrans (theta);

        // 时常会遇到角度为5度返回内的图片，此时处理机制为默认已经高度校正，过滤该线条角度
        // 不应该为了追求效率，导致过滤掉大量高度矫正的线条，导致本就纠正的图片变歪
        /*
        if (((DegreeTrans(theta) >= 85) && (DegreeTrans(theta) <= 95))
                || (DegreeTrans(theta) >= 178.5)
                || (DegreeTrans(theta) <= 5 )) {
            n += 1;
            continue;
        }
        */
        if  (DegreeTrans(theta) <= 5) {
            ++num0;
        } else if (DegreeTrans(theta) >= 175 && DegreeTrans(theta) <= 185) {
            ++num180;
        } else if (DegreeTrans(theta) >= 85 && DegreeTrans(theta) <= 95) {
            ++num90;
        }

        KyInfo() << "theta = " << theta
                 << "degree(theta) = " << DegreeTrans(theta);

        sum += theta;
        //KyInfo() << "sum = " << sum;
        line(dstImage, pt1, pt2, Scalar(55, 100, 195), 1, LINE_AA); //Scalar函数用于调节线段颜色
        // mirror picture for deskew(rectify)
        imwrite("/tmp/scanner/mirror.png", dstImage);
    }


    KyInfo() << "sum = " << sum << "lines.size()" << lines.size () << "n = " << n;
    if (lines.size () - n == 0 || sum == 0)
        return 0.0;

    //对所有角度求平均，这样做旋转效果会更好
    //float average = sum / (lines.size() - n);

    // sum =  182.352 lines.size() 181
    // 这种情况会将已经垂直的图片纠偏歪了
    float average = sum / loop;

    double angle = DegreeTrans(average);
    KyInfo() << "angle = " << angle;

    // angel < 0: clockwise rotation
    // 经过多次反复测试，此处应该减90，整个线条接近水平，之后可以旋转进行校正。

    KyInfo() << "num0 = " << num0
             << "num180 = " << num180
             << "num90 = " << num90
             << "loop = " << loop;

    if (num0 + num180 > loop * 0.90) {
        // 接近垂直，故需要旋转90度，但是不清楚是正方向还是反方向，同时还有图片的大小问题，故不纠偏
        // angle = 90;
        angle = 0;
    } else if (num90 >= loop * 0.90) {
        // 接近水平，故不矫正
        angle = 0;
    } else {
        angle = angle - 90;
    }

    /*
    if (angle >= 135)
        angle = angle - 180;
    else if (angle >= 90)
        angle =angle - 90;
    else if (angle >= 45)
        angle =  angle - 90;
        */

    KyInfo() << "angle = " << angle;

    rotateImage(dstImage, dst, angle);
    return angle;
}

int ImageRectify(const char *pInFileName)
{
    double degree;

    Mat src = imread(pInFileName);
    if (!src.data) {
        // Avoid crash after click btnBeauty duplicately
        KyInfo() << "cannot read this image: " << pInFileName;
        return -1;
    }
    Mat dst;

    KyInfo() << "before calcDegree(): inFilename = " << pInFileName;
    degree = CalcDegree(src, dst);
    KyInfo() << "degree = " << degree;

    if (fabs (degree - ERROR) < 1e-15) {
        KyInfo() << "rectify failed ！" ;
        return -1;
    }
    if (fabs(degree) < 1e-15 + 0.4) {
        KyInfo() << "already right, so return straight!";
        return -1;
    }

    src = imread(pInFileName);
    rotateImage(src, dst, degree);
    imwrite(ScanningPicture, dst);
    return 0;
}
