#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <android/log.h>
#include "BitmapMatUtil.h"
#include "cardocr.h"

using namespace cv;
using namespace std;
using namespace co1;
#define TAG "JIN_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

CascadeClassifier cascadeClassifier;

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_yxm_ndktest_opencv_FaceDetection_grey(JNIEnv *env, jobject thiz, jintArray buffer,
                                               jint width, jint height) {
    jint *cbuf = env->GetIntArrayElements(buffer, JNI_FALSE);
    if (cbuf == nullptr) {
        return nullptr;
    }
    // 将输入数组还原成一个 cv::Mat，方便进行操作。
    Mat imgData(height, width, CV_8UC4, (unsigned char *) cbuf);
    uchar *ptr = imgData.ptr(0);
    for (int i = 0; i < width * height; i++) {
        //计算公式：Y(亮度) = 0.299*R + 0.587*G + 0.114*B
        // 对于一个int四字节，其彩色值存储方式为：BGRA
        int grayScale = (int) (ptr[4 * i + 2] * 0.299 + ptr[4 * i + 1] * 0.587 +
                               ptr[4 * i + 0] * 0.114);
        ptr[4 * i + 1] = grayScale;
        ptr[4 * i + 2] = grayScale;
        ptr[4 * i + 0] = grayScale;
    }
    int size = width * height;
    jintArray result = env->NewIntArray(size);
    // 上面我们是对 Mat 做了修改，而 Mat 指向的内存空间其实就是 cbuf 的空间，因此 cbuf 中
    // 的结果就是最终结果。
    // 这里创建一个新的数组并返回。
    env->SetIntArrayRegion(result, 0, size, cbuf);
    env->ReleaseIntArrayElements(buffer, cbuf, 0);
    return result;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_yxm_ndktest_opencv_FaceDetection_faceDetection(JNIEnv *env, jobject thiz, jobject bitmap) {
    Mat mat;
    BitmapMatUtil::bitmap2Mat(env, mat, bitmap);

    Mat grey_mat;
    cvtColor(mat, grey_mat, COLOR_BGRA2GRAY);

    //再次处理，直方均衡补偿
    Mat equalizeMat;
    equalizeHist(grey_mat, equalizeMat);

    vector<Rect> faces;
    cascadeClassifier.detectMultiScale(equalizeMat, faces, 1.1, 5);
    LOGE("人脸个数:%d", faces.size());
    if (faces.size() == 1) {
        //imwrite()
        Rect faceRect = faces[0];
        rectangle(mat, faceRect, Scalar(0, 0, 255, 255), 8);
        //保存人脸信息 mat,转成图片
        BitmapMatUtil::mat2Bitmap(env, mat, bitmap);
    }
    return faces.size();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yxm_ndktest_opencv_FaceDetection_loadCascade(JNIEnv *env, jobject thiz,
                                                      jstring file_path) {
    const char *filePath = env->GetStringUTFChars(file_path, nullptr);
    if (cascadeClassifier.load(filePath)) {
        LOGE("加载成功");
    } else {
        LOGE("加载失败");
    }
    env->ReleaseStringUTFChars(file_path, filePath);
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_yxm_ndktest_opencv_BankCardOcr_00024Companion_cardOcr(JNIEnv *env, jobject thiz,
                                                               jobject bitmap) {
    Mat mat;
    BitmapMatUtil::bitmap2Mat(env, mat, bitmap);
    Rect card_area;
    co1::find_card_area(mat,card_area);

    Mat card_mat(mat,card_area);

    //写到内存卡
    imwrite("/storage/emulated/0/Android/data/com.yxm.ndktest/grad_n.jpg", card_mat);

    Rect card_number_rect;
    co1::find_card_number_area(card_mat,card_number_rect);
    Mat card_number_mat(card_mat,card_number_rect);
    imwrite("/storage/emulated/0/Android/data/com.yxm.ndktest/card_number.jpg", card_number_mat);

    vector<Mat> vector;
    co1::find_card_numbers(card_number_mat,vector);

    return env->NewStringUTF("123");
}