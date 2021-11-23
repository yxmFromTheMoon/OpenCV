#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include "Draw.h"
using namespace cv;

int Draw::drawLine(const Mat &mat) {
    line(mat,Point(100,100),Point(200,200),Scalar(255,0,0),5,LINE_8);
    return 0;
}

int Draw::drawText(const Mat &mat) {
    return 0;
}
