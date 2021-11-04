package com.yxm.ndktest

import android.Manifest
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import com.yxm.ndktest.databinding.ActivityMainBinding
import com.yxm.ndktest.opencv.BankCardOcr
import com.yxm.ndktest.opencv.FaceDetection
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream


class MainActivity : AppCompatActivity() {

    private var mCascadeFile: File? = null
    private lateinit var binding: ActivityMainBinding

    private var mBitmap: Bitmap? = null

    private val faceDetection by lazy {
        FaceDetection()
    }

    private val requestPermissionLauncher =
        registerForActivityResult(ActivityResultContracts.RequestPermission()) {
            if (it) {
                BankCardOcr.cardOcr(BitmapFactory.decodeResource(resources, R.drawable.card_n))
            }
        }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        requestPermissionLauncher.launch(Manifest.permission.READ_EXTERNAL_STORAGE)

        copyCascadeFile()
        faceDetection.loadCascade(mCascadeFile?.absolutePath ?: "")

        mBitmap = BitmapFactory.decodeResource(resources, R.drawable.download)
        //.copy(Bitmap.Config.ARGB_8888,true)
        //binding.originIv.setImageBitmap(mBitmap)
//            .copy(Bitmap.Config.ARGB_8888, true)
//        val width = bitmap.width
//        val height = bitmap.height
//        val pixel = IntArray(width * height)
//        bitmap.getPixels(pixel, 0, width, 0, 0, width, height)
//        val greyPixels = faceDetection.grey(pixel, width, height)
//        val greyBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
//        greyBitmap.setPixels(greyPixels, 0, width, 0, 0, width, height)

        binding.button.setOnClickListener {
            mBitmap?.let {
                val size = faceDetection.faceDetection(it)
                Toast.makeText(this, "检测到${size}张人脸", Toast.LENGTH_SHORT).show()
            }
            binding.originIv.setImageBitmap(mBitmap)

            val cardBitmap = BitmapFactory.decodeResource(resources, R.drawable.card1)
            val number = BankCardOcr.cardOcr(cardBitmap)
            Toast.makeText(this, number, Toast.LENGTH_SHORT).show()
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
}