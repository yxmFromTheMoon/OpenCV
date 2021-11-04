
#ifndef BITMAP_UTIL
#define BITMAP_UTIL

#include <jni.h>
#include "opencv2/opencv.hpp"

using namespace cv;

class BitmapMatUtil {
public:
     static int mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap);
     static int bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap);
};

#endif BITMAP_UTIL