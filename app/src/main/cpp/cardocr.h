#include "opencv2/opencv.hpp"

//条件编译，防止多次include时报错
#ifndef _CARD_OCR_H_
#define _CARD_OCR_H_

using namespace cv;
using namespace std;

namespace co1 {
    int find_card_area(const Mat &mat, Rect &area);

    int find_card_number_area(const Mat &mat, Rect &area);

    int find_card_numbers(const Mat &mat, const vector<Mat> numbers);
}

namespace co2 {

}

#endif //_CARD_OCR_H_