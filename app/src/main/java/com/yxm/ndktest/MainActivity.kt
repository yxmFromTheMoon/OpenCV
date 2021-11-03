package com.yxm.ndktest

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.yxm.ndktest.databinding.ActivityMainBinding
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream


class MainActivity : AppCompatActivity() {

    private var mCascadeFile: File? = null
    private lateinit var binding: ActivityMainBinding

    private var mBitmap: Bitmap? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        copyCascadeFile()
        loadCascade(mCascadeFile?.absolutePath ?: "")

        mBitmap = BitmapFactory.decodeResource(resources, R.drawable.download)
            //.copy(Bitmap.Config.ARGB_8888,true)
        //binding.originIv.setImageBitmap(mBitmap)
//            .copy(Bitmap.Config.ARGB_8888, true)
//        val width = bitmap.width
//        val height = bitmap.height
//        val pixel = IntArray(width * height)
//        bitmap.getPixels(pixel, 0, width, 0, 0, width, height)
//        val greyPixels = grey(pixel, width, height)
//        val greyBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
//        greyBitmap.setPixels(greyPixels, 0, width, 0, 0, width, height)

        binding.button.setOnClickListener {
            mBitmap?.let {
                faceDetection(it)
            }
            binding.originIv.setImageBitmap(mBitmap)
        }

    }

    private fun copyCascadeFile() {
        try {
            // load cascade file from application resources
            val `is`: InputStream = resources.openRawResource(R.raw.lbpcascade_frontalface)
            val cascadeDir: File = getDir("cascade", Context.MODE_PRIVATE)
            mCascadeFile = File(cascadeDir, "lbpcascade_frontalface.xml")
            if (mCascadeFile?.exists() == true)
                return
            val os = FileOutputStream(mCascadeFile)
            val buffer = ByteArray(4096)
            var bytesRead: Int
            while (`is`.read(buffer).also { bytesRead = it } != -1) {
                os.write(buffer, 0, bytesRead)
            }
            `is`.close()
            os.close()
            cascadeDir.delete()
        } catch (e: IOException) {
            e.printStackTrace()
            Log.e("TAG", "Failed to load cascade. Exception thrown: $e")
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun grey(buffer: IntArray, width: Int, height: Int): IntArray

    external fun faceDetection(bitmap: Bitmap): Int

    /**
     * 加载人脸识别分类器
     */
    external fun loadCascade(filePath: String)

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}