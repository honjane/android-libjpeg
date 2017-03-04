package com.honjane.compress;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import com.honjane.compress.utils.ImageUtils;

import java.io.File;
import java.io.InputStream;

public class MainActivity extends Activity {
    private String mFilePath ;
    private ImageView mDisplayIv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btn_path);
        findViewById(R.id.btn_display);
        mDisplayIv = (ImageView) findViewById(R.id.img_view);

    }


    public void onGetPath(View view) {
        try {

            InputStream in = getResources().getAssets()
                    .open("test.jpg");
            Bitmap bitmap = BitmapFactory.decodeStream(in);
            File dirFile = getExternalCacheDir();
            if (!dirFile.exists()) {
                dirFile.mkdirs();
            }

            String filename = System.currentTimeMillis() + ".jpg";
            File jpegFile = new File(dirFile, filename);

            mFilePath = jpegFile.getAbsolutePath();
            boolean flag = false;
            if (bitmap != null) {
                flag = ImageUtils.compressBitmap(bitmap, bitmap.getWidth(), bitmap.getHeight(), mFilePath, 20);
            } else {
                Toast.makeText(MainActivity.this, "file not found", Toast.LENGTH_SHORT).show();
            }
            Toast.makeText(MainActivity.this, flag ? "success 文件大小" + (jpegFile.length()/1024)+"kb": "fail", Toast.LENGTH_SHORT).show();
        } catch (Exception e) {

        }



    }

    public void onDisplay(View view) {
        if (TextUtils.isEmpty(mFilePath)) {
            Toast.makeText(MainActivity.this, "请先压缩图片", Toast.LENGTH_SHORT).show();
            return;
        }
        Bitmap bitmap = BitmapFactory.decodeFile(mFilePath);
        mDisplayIv.setImageBitmap(bitmap);
    }



}
