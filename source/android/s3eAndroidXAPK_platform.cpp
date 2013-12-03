/*
 * android-specific implementation of the s3eAndroidXAPK extension.
 * Add any platform-specific functionality here.
 */
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
#include "s3eAndroidXAPK_internal.h"

#include "s3eEdk.h"
#include "s3eEdk_android.h"
#include <jni.h>
#include "IwDebug.h"

static jobject g_Obj;
static jmethodID g_s3eAndroidXAPKGetFiles;

s3eResult s3eAndroidXAPKInit_platform()
{
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;

    // Get the extension class
    jclass cls = s3eEdkAndroidFindClass("s3eAndroidXAPK");
    if (!cls)
        goto fail;

    // Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    // Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    g_s3eAndroidXAPKGetFiles = env->GetMethodID(cls, "s3eAndroidXAPKGetFiles", "(Ljava/lang/String;LFIXME;I)V");
    if (!g_s3eAndroidXAPKGetFiles)
        goto fail;



    IwTrace(ANDROIDXAPK, ("ANDROIDXAPK init success"));
    g_Obj = env->NewGlobalRef(obj);
    env->DeleteLocalRef(obj);
    env->DeleteGlobalRef(cls);

    // Add any platform-specific initialisation code here
    return S3E_RESULT_SUCCESS;

fail:
    jthrowable exc = env->ExceptionOccurred();
    if (exc)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        IwTrace(s3eAndroidXAPK, ("One or more java methods could not be found"));
    }
    return S3E_RESULT_ERROR;

}

void s3eAndroidXAPKTerminate_platform()
{
    // Add any platform-specific termination code here
}

s3eResult s3eAndroidXAPKRegister_platform(s3eAndroidXAPKCallback callbackID, s3eCallback callbackFn, void* userData)
{
    return S3E_RESULT_ERROR;
}

s3eResult s3eAndroidXAPKUnRegister_platform(s3eAndroidXAPKCallback callbackID, s3eCallback callbackFn)
{
    return S3E_RESULT_ERROR;
}

void s3eAndroidXAPKGetFiles_platform(const char* base64PublicKey, const void* salt, int32 saltLength)
{
    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring base64PublicKey_jstr = env->NewStringUTF(base64PublicKey);
    env->CallVoidMethod(g_Obj, g_s3eAndroidXAPKGetFiles, base64PublicKey_jstr, salt, saltLength);
}
