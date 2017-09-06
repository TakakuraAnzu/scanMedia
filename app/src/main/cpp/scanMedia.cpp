#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <unistd.h>
#include <sys/types.h>
#include "JniCommon.h"
#include <string.h>
#include <cstddef>
#include <unistd.h>
//#include <ftw.h>

const char *base_path;
jobject ArrayList;
jmethodID add;
/***
 * 判断文件目录下是否有.nomedia文件
 * @param ent 文件目录指针
 * @param i 文件目录下文件数量
 * @return bool
 */
bool isNoMedia(struct dirent **ent, int i){
    int n = 0;
    while (n != i){
        if(strcmp(ent[n]->d_name, ".nomedia") == 0 ){
            free(ent);//释放当前文件目录指针
            return true;
        }
        n++;
    }
    return false;
}
/**
 * 判断是否视频文件
 * @param name 文件名称
 * @return bool
 */
bool isMedia(char *name){
    char *last = strrchr(name, '.');//获取文件后缀
    //LOGD("%s", last);
    if(last != NULL){
        if(!strcasecmp(".mp4", last)
           || !strcasecmp(".3gp", last)
           || !strcasecmp(".wmv", last)
           || !strcasecmp(".ts", last)
           || !strcasecmp(".rmvb", last)
           || !strcasecmp(".mov", last)
           || !strcasecmp(".m4v", last)
           || !strcasecmp(".avi", last)
           || !strcasecmp(".m3u8", last)
           || !strcasecmp(".3gpp", last)
           || !strcasecmp(".3gpp2", last)
           || !strcasecmp(".mkv", last)
           || !strcasecmp(".flv", last)
           || !strcasecmp(".divx", last)
           || !strcasecmp(".f4v", last)
           || !strcasecmp(".rm", last)
           || !strcasecmp(".asf", last)
           || !strcasecmp(".ram", last)
           || !strcasecmp(".mpg", last)
           || !strcasecmp(".v8", last)
           || !strcasecmp(".swf", last)
           || !strcasecmp(".m2v", last)
           || !strcasecmp(".asx", last)
           || !strcasecmp(".ra", last)
           || !strcasecmp(".ndivx", last)
           || !strcasecmp(".xvid", last)){
            return true;
        }
    }
    return false;
}
/**
 * 历遍文件夹
 * @param env 指针
 * @param path 目录路径
 */
void scan(JNIEnv *env, const char* path){
    int i;
    int n = 0;
    char child_path[512];
    struct dirent **ent; //定义一个结构体dirent指针保存后续目录
    struct stat statbuf;//定义一个结构体stat指针保存文件属性
    i = scandir(path, &ent, 0, alphasort);//获取目录下文件夹以及文件数量
    if(i<0){//没有文件则返回
        return;
    }
    if(isNoMedia(ent, i)){//目录下有.nomedia文件则返回
        return;
    }
    memset(child_path, 0, sizeof(child_path));
    while (n != i){
        lstat(ent[n]->d_name, &statbuf);
        if(strncmp(ent[n]->d_name, ".", 1) == 0 ){//跳过.开头文件夹以及文件的扫描,例如.android
            free(ent[n]);//释放文件目录指针
            n++;
            continue;
        }
        sprintf(child_path, "%s/%s", path, ent[n]->d_name);//当前文件的文件路径
        jstring result = env->NewStringUTF(child_path);
        if(isMedia(ent[n]->d_name)){//如果是视频文件则加入ArrayList
            env->CallBooleanMethod(ArrayList, add, result);
            LOGD("%s", child_path);
        }
        if(S_IFDIR &statbuf.st_mode){//判断下一级成员是否目录
            scan(env, child_path);
        } else{
        }
        env-> DeleteLocalRef(result);//释放资源
        free(ent[n]);//释放文件目录指针
        n++;
    }
    free(ent);//释放文件目录指针
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_illya_scanmedia_MainActivity_StartScan(JNIEnv *env, jclass type, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    // TODO
    base_path = path;
    LOGD("start");
    jclass listClass = env->FindClass("java/util/ArrayList");//获取ArrayList类
    jmethodID listInit = env->GetMethodID(listClass, "<init>", "()V");//初始化ArrayList
    ArrayList = env->NewObject(listClass,listInit);//创建ArrayList对象
    add = env->GetMethodID(listClass, "add", "(Ljava/lang/Object;)Z");//获取ArrayList的add方法
    scan(env, path);//历遍目录
    LOGD("success");
    env->ReleaseStringUTFChars(path_, path);//释放字符对象
    return ArrayList;
}
