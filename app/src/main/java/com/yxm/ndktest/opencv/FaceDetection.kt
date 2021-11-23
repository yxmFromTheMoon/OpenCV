package com.yxm.ndktest.opencv

import android.graphics.Bitmap

/**
 * Created by yxm on 2021/11/4 11:27 上午
 * @email: yinxiangming@lightinthebox.com
 * @description: 人脸识别
 */
class FaceDetection {

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }

    external fun grey(buffer: IntArray, width: Int, height: Int): IntArray

    external fun faceDetection(bitmap: Bitmap): Int

    /**
     * 加载人脸识别分类器
     */
    external fun loadCascade(filePath: String)


    external fun drawLine(bitmap: Bitmap?)

    external fun drawText()
}