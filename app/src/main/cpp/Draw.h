#ifndef _DRAW_H
#define _DRAW_H
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class Draw{
public:
    static int drawLine(const Mat &mat);
    static int drawText(const Mat &mat);
};

#endif