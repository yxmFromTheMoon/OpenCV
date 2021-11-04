#include "cardocr.h"

int co1::find_card_area(const Mat &mat, Rect area) {
    // 高斯模糊，降噪
    Mat blur;
    GaussianBlur(mat, blur, Size(5, 5), BORDER_DEFAULT, BORDER_DEFAULT);

    //边缘梯度增强,比较耗时
    Mat gard_x, gard_y;
    Scharr(blur, gard_x, CV_32F, 1, 0);
    Scharr(blur, gard_y, CV_32F, 0, 1);

    Mat abs_gard_x, abs_gard_y;
    convertScaleAbs(gard_x, abs_gard_x);
    convertScaleAbs(gard_y, abs_gard_y);
    Mat gard;
    addWeighted(abs_gard_x, 0.5, abs_gard_y, 0.5, 0, gard);

    //写到内存卡
    imwrite("/storage/emulated/0/Android/data/com.yxm.ndktest/grad_n2.jpg",gard);

    return 0;
}