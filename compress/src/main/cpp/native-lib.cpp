#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/bitmap.h>

extern "C" {
#include "jpeglib.h"
}

typedef uint8_t BYTE;

#define TAG "honjane"
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

#define true 1
#define false 0

extern "C"
int generateJPEG(BYTE *data, int w, int h, int quality,
                 const char *outfile, jboolean optimize);


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_honjane_compress_utils_ImageUtils_compressBitmap(JNIEnv *env, jclass jcls, jobject bitmap,
                                                          jint width, jint height,
                                                          jstring jfilepath,
                                                          jint quality) {

    AndroidBitmapInfo infoColor;
    BYTE *pixelColor;
    BYTE *data;
    BYTE *tempData;
    const char *filepath = env->GetStringUTFChars(jfilepath, 0);
    LOGE("------ 文件目录 %s", filepath);
    if ((AndroidBitmap_getInfo(env, bitmap, &infoColor)) < 0) {
        LOGE("parse error");
        env->ReleaseStringUTFChars(jfilepath, filepath);
        return false;
    }
    //锁住 与操作canvas类似，操作前先锁住
    if ((AndroidBitmap_lockPixels(env, bitmap, (void **) &pixelColor)) < 0) {
        LOGE("lock pixels error");
        env->ReleaseStringUTFChars(jfilepath, filepath);
        return false;
    }

    BYTE r, g, b;
    int color;
    data = (BYTE *) malloc(width * height * 3);
    //1.获取bitmap对应的rgb值 存入data
    tempData = data;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            color = *((int *) pixelColor);
            //获取对应的r,g,b值，其中a为高8位  color >> 24
            r = ((color & 0x00FF0000) >> 16);//r为次高8位  color >> 16
            g = ((color & 0x0000FF00) >> 8); //g为中8位  color >> 8
            b = color & 0X000000FF;          //b为低8位

            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;
            pixelColor += 4;
        }
    }
    //释放锁
    AndroidBitmap_unlockPixels(env, bitmap);
    int resultCode = generateJPEG(tempData, width, height, quality, filepath, true);
    LOGE("------压缩完成 0 失败 ，1 成功 resultCode= %d", resultCode);

    free(tempData);
    if (resultCode == 0) {
        env->ReleaseStringUTFChars(jfilepath, filepath);
        return false;
    }

    env->ReleaseStringUTFChars(jfilepath, filepath);

    return true;
}

extern "C"
//图片压缩方法
int generateJPEG(BYTE *data, int w, int h, int quality,
                 const char *outfile, jboolean optimize) {
    int nComponent = 3;

    struct jpeg_compress_struct jcs;

    struct jpeg_error_mgr jem;

    jcs.err = jpeg_std_error(&jem);

    //2.为JPEG对象分配空间并初始化
    jpeg_create_compress(&jcs);
    //3.获取文件信息
    FILE *file = fopen(outfile, "wb");
    if (file == NULL) {
        return 0;
    }
    //4.指定压缩数据源
    jpeg_stdio_dest(&jcs, file);
    //image_width->JDIMENSION->typedef unsigned int

    jcs.image_width = w;
    jcs.image_height = h;
    //false 使用哈夫曼算法
    jcs.arith_code = false;
    //input_components为1代表灰度图，在等于3时代表彩色位图图像
    jcs.input_components = nComponent;
    if (nComponent == 1) {
        //in_color_space为JCS_GRAYSCALE表示灰度图，在等于JCS_RGB时代表彩色位图图像
        jcs.in_color_space = JCS_GRAYSCALE;
    } else {
        jcs.in_color_space = JCS_RGB;
    }

    jpeg_set_defaults(&jcs);
    //optimize_coding为TRUE，将会使得压缩图像过程中基于图像数据计算哈弗曼表，由于这个计算会显著消耗空间和时间，默认值被设置为FALSE。
    jcs.optimize_coding = optimize;
    //5. 为压缩设定参数，包括图像大小，颜色空间
    jpeg_set_quality(&jcs, quality, true);

    //6.开始压缩
    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];
    int row_stride;
    //行宽经过compress中循环变为了image宽度的3倍了，需要通过循环截成正常宽度
    row_stride = jcs.image_width * nComponent;
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &data[jcs.next_scanline * row_stride];
        //写入数据 http://www.cnblogs.com/darkknightzh/p/4973828.html
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }

    //7.压缩完毕
    jpeg_finish_compress(&jcs);
    //8.释放资源
    jpeg_destroy_compress(&jcs);
    fclose(file);

    return JNI_TRUE;
}

