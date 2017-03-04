package com.honjane.compress.utils;

import android.graphics.Bitmap;

/**
 * Created by honjane on 2017/3/4.
 */

public class ImageUtils {


    static {
        System.loadLibrary("hjpeg");
    }


    public static native boolean compressBitmap(Bitmap bitmap, int width, int height, String filePath,
                                                 int quality);
}
