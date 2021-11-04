#include "cardocr.h"
#include "vector"
#include <android/log.h>

using namespace std;
#define TAG "JIN_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

int co1::find_card_area(const Mat &mat, Rect &area) {
    // 高斯模糊，降噪
    Mat blur;
    GaussianBlur(mat, blur, Size(5, 5), BORDER_DEFAULT, BORDER_DEFAULT);

    //边缘梯度增强,比较耗时   //边缘检测 //腐蚀
    Mat gard_x, gard_y;
    Scharr(blur, gard_x, CV_32F, 1, 0);
    Scharr(blur, gard_y, CV_32F, 0, 1);

    Mat abs_gard_x, abs_gard_y;
    convertScaleAbs(gard_x, abs_gard_x);
    convertScaleAbs(gard_y, abs_gard_y);
    Mat gard;
    addWeighted(abs_gard_x, 0.5, abs_gard_y, 0.5, 0, gard);

    //灰度处理
    Mat grey;
    cvtColor(gard, grey, COLOR_BGRA2GRAY);
    //二值化
    Mat binary;
    threshold(grey, binary, 40, 255, THRESH_BINARY);

    vector<vector<Point>> contours;
    findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (auto &contour : contours) {
        Rect rect = boundingRect(contour);
        if (rect.width > (mat.cols / 2) && rect.width != mat.cols && rect.height > mat.rows / 2) {
            area = rect;
            break;
        }
    }

    blur.release();
    gard_x.release();
    gard_y.release();
    abs_gard_x.release();
    abs_gard_y.release();
    gard.release();
    grey.release();
    binary.release();

    return 0;
}

int co1::find_card_number_area(const Mat &mat, Rect &area) {
    area.x = mat.cols / 14;
    area.y = mat.rows / 3;
    area.width = mat.cols * 6 / 7;
    area.height = mat.cols / 6;
    return 0;
}

int co1::find_card_numbers(const Mat &mat, const vector<Mat> numbers) {
    // 二值化，灰度处理（常见）
    Mat gray;
    cvtColor(mat, gray, COLOR_BGRA2GRAY);
    // THRESH_OTSU THRESH_TRIANGLE自己去找合适的值 ，这一步非常关键
    Mat binary;
    threshold(gray, binary, 39, 255, THRESH_BINARY);
    imwrite("/storage/emulated/0/Android/data/com.yxm.ndktest/card_number_binary_n.jpg", binary);

    // 降噪过滤
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(binary, binary, MORPH_CLOSE, kernel);
    // 去掉干扰过滤填充，找数字就是轮廓查询 （card.io 16 位 ，19 位）
    // 查找轮廓 白色轮廓 binary  黑色的背景，白色的数字
    // 取反 白黑 -> 黑白
    Mat binary_not = binary.clone();
    bitwise_not(binary_not, binary_not);
    imwrite("/storage/emulated/0/Android/data/com.yxm.ndktest/card_number_binary_not_n.jpg", binary_not);
    vector<vector<Point> > contours;
    findContours(binary_not, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    int mat_area = mat.rows * mat.cols;
    int min_h = mat.rows / 4;
    for (int i = 0; i < contours.size(); i++) {
        Rect rect = boundingRect(contours[i]);
        // 多个条件，面积太小的过滤 1/100
        int area = rect.width * rect.height;
        if (area < mat_area / 200) {
            // 小面积填充为 白色背景
            drawContours(binary, contours, i, Scalar(255), -1);
        } else if (rect.height < min_h) {
            drawContours(binary, contours, i, Scalar(255), -1);
        }
    }

    imwrite("/storage/emulated/0/Android/data/com.yxm.ndktest/card_number_binary_noise_n.jpg", binary);

    gray.release();
    binary.release();
    binary_not.release();
    return 0;
}