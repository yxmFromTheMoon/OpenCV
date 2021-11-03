package com.yxm.ndktest

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Toast
import com.yxm.ndktest.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.download)
            .copy(Bitmap.Config.ARGB_8888, true)
        val width = bitmap.width
        val height = bitmap.height
        val pixel = IntArray(width * height)
        bitmap.getPixels(pixel, 0, width, 0, 0, width, height)
        val greyPixels = grey(pixel, width, height)
        val greyBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        greyBitmap.setPixels(greyPixels, 0, width, 0, 0, width, height)
        binding.greyIv.setImageBitmap(greyBitmap)

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun grey(buffer: IntArray, width: Int, height: Int): IntArray

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}