#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;

extern "C" JNIEXPORT jstring JNICALL
Java_com_yxm_ndktest_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_yxm_ndktest_MainActivity_grey(JNIEnv *env, jobject thiz, jintArray buffer, jint width,
                                       jint height) {
    jint *cbuf = env->GetIntArrayElements(buffer, JNI_FALSE);
    if (cbuf == nullptr){
        return nullptr;
    }
    // 将输入数组还原成一个 cv::Mat，方便进行操作。
    Mat imgData(height, width, CV_8UC4, (unsigned char *)cbuf);
    uchar *ptr = imgData.ptr(0);
    for (int i = 0; i < width * height; i++)
    {
        //计算公式：Y(亮度) = 0.299*R + 0.587*G + 0.114*B
        // 对于一个int四字节，其彩色值存储方式为：BGRA
        int grayScale = (int)(ptr[4 * i + 2] * 0.299 + ptr[4 * i + 1] * 0.587 + ptr[4 * i + 0] * 0.114);
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