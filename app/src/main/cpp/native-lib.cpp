#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <android/log.h>

using namespace cv;
using namespace std;
#define TAG "JIN_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_yxm_ndktest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

// bitmap 转成 Mat
void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap);

// mat 转成 Bitmap
void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap);

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_yxm_ndktest_MainActivity_grey(JNIEnv *env, jobject thiz, jintArray buffer, jint width,
                                       jint height) {
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

void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap) {
    //Mat 里的 type:CV_8UC4对应Bitmap中的ARGB_8888,CV_8UC2对应Bitmap中的RGB_565
    //获取bitmap信息
    AndroidBitmapInfo info;
    void *pixels;
    AndroidBitmap_getInfo(env, bitmap, &info);

    AndroidBitmap_lockPixels(env, bitmap, &pixels);

    mat.create(info.height, info.width, CV_8UC4);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        temp.copyTo(mat);
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        Mat temp(info.height, info.width, CV_8UC2, pixels);
        cvtColor(temp, mat, COLOR_BGR5652BGRA);
    }
    cvtColor(mat, mat, COLOR_RGB2BGR);
    //其他要自己转
    AndroidBitmap_unlockPixels(env, bitmap);
}

void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap) {
    // 1. 获取 bitmap 信息
    AndroidBitmapInfo info;
    void* pixels;
    AndroidBitmap_getInfo(env,bitmap,&info);

    // 锁定 Bitmap 画布
    AndroidBitmap_lockPixels(env,bitmap,&pixels);

    if(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888){// C4
        Mat temp(info.height,info.width,CV_8UC4,pixels);
        if(mat.type() == CV_8UC4){
            mat.copyTo(temp);
        }
        else if(mat.type() == CV_8UC2){
            cvtColor(mat,temp,COLOR_BGR5652BGRA);
        }
        else if(mat.type() == CV_8UC1){// 灰度 mat
            cvtColor(mat,temp,COLOR_GRAY2BGRA);
        }
    } else if(info.format == ANDROID_BITMAP_FORMAT_RGB_565){// C2
        Mat temp(info.height,info.width,CV_8UC2,pixels);
        if(mat.type() == CV_8UC4){
            cvtColor(mat,temp,COLOR_BGRA2BGR565);
        }
        else if(mat.type() == CV_8UC2){
            mat.copyTo(temp);

        }
        else if(mat.type() == CV_8UC1){// 灰度 mat
            cvtColor(mat,temp,COLOR_GRAY2BGR565);
        }
    }
    //其他要自己转
    AndroidBitmap_unlockPixels(env, bitmap);
}

CascadeClassifier cascadeClassifier;

extern "C"
JNIEXPORT jint JNICALL
Java_com_yxm_ndktest_MainActivity_faceDetection(JNIEnv *env, jobject thiz, jobject bitmap) {

    Mat mat;
    bitmap2Mat(env, mat, bitmap);

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
        rectangle(mat,faceRect,Scalar(0,0,255,255),8);
        //保存人脸信息 mat,转成图片
        mat2Bitmap(env, mat, bitmap);
    }
    return 0;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_yxm_ndktest_MainActivity_loadCascade(JNIEnv *env, jobject thiz, jstring file_path) {
    const char *filePath = env->GetStringUTFChars(file_path, nullptr);
    if (cascadeClassifier.load(filePath)) {
        LOGE("加载成功");
    } else {
        LOGE("加载失败");
    }
    env->ReleaseStringUTFChars(file_path, filePath);
}