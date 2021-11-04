package com.yxm.ndktest.opencv

import android.graphics.Bitmap

/**
 * Created by yxm on 2021/11/4 11:32 上午
 * @email: yinxiangming@lightinthebox.com
 * @description: 银行卡识别
 */
class BankCardOcr {
    companion object {
        init {
            System.loadLibrary("native-lib")
        }

        external fun cardOcr(bitmap: Bitmap): String

    }

}