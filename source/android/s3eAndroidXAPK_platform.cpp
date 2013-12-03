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

static jfieldID g_fileName;
static jfieldID g_fileUrl;
static jfieldID g_fileSize;

static jfieldID g_responseResult;
static jfieldID g_responseFiles;

void responseReceived(JNIEnv* env, jobject obj, jobject response);

s3eResult s3eAndroidXAPKInit_platform()
{
    // Get the environment from the pointer
    JNIEnv* env = s3eEdkJNIGetEnv();
    jobject obj = NULL;
    jmethodID cons = NULL;
    jclass responseCls = NULL;
    jclass fileCls = NULL;

    // Get the extension class
    jclass cls = s3eEdkAndroidFindClass("s3eAndroidXAPK");
    if (!cls)
        goto fail;

    //Register the native method on the object
    static const JNINativeMethod jnm = { "responseReceived", "(Ls3eAndroidXAPK$Response;)V", (void*)&responseReceived };

    if (env->RegisterNatives(cls, &jnm, 1))
        goto fail;

    // Get its constructor
    cons = env->GetMethodID(cls, "<init>", "()V");
    if (!cons)
        goto fail;

    // Construct the java class
    obj = env->NewObject(cls, cons);
    if (!obj)
        goto fail;

    g_s3eAndroidXAPKGetFiles = env->GetMethodID(cls, "s3eAndroidXAPKGetFiles", "(Ljava/lang/String;[B;I)V");
    if (!g_s3eAndroidXAPKGetFiles)
        goto fail;

    // Get the helper classes
    fileCls = s3eEdkAndroidFindClass("s3eAndroidXAPK$File");
    if (!fileCls)
        goto fail;

    
    responseCls = s3eEdkAndroidFindClass("s3eAndroidXAPK$Response");
    if (!responseCls)
        goto fail;

    // Get the fields
    g_fileName = env->GetFieldID(fileCls, "name", "Ljava/lang/String;");
    if (!g_fileName)
        goto fail;

    g_fileUrl = env->GetFieldID(fileCls, "url", "Ljava/lang/String;");
    if (!g_fileUrl)
        goto fail;

    g_fileSize = env->GetFieldID(fileCls, "size", "J");
    if (!g_fileSize)
        goto fail;

    g_responseResult = env->GetFieldID(responseCls, "result", "I");
    if (!g_responseResult)
        goto fail;

    g_responseFiles = env->GetFieldID(responseCls, "files", "[Ls3eAndroidXAPK$File;");
    if (!g_responseFiles)
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
    return s3eEdkCallbacksRegister(S3E_EXT_ANDROIDXAPK_HASH, s3eAndroidXAPKCallback_MAX, callbackID, callbackFn, userData, false);
}

s3eResult s3eAndroidXAPKUnRegister_platform(s3eAndroidXAPKCallback callbackID, s3eCallback callbackFn)
{
    return s3eEdkCallbacksUnRegister(S3E_EXT_ANDROIDXAPK_HASH, s3eAndroidXAPKCallback_MAX, callbackID, callbackFn);
}

s3eResult s3eAndroidXAPKGetFiles_platform(const char* base64PublicKey, const void* salt, int32 saltLength)
{
    if (saltLength != 20)
    {
        s3eEdkErrorSet(S3E_EXT_ANDROIDXAPK_HASH, 1, S3E_EXT_ERROR_PRI_NORMAL);
        s3eEdkErrorSetString("Salt must be 20 bytes long");
        return S3E_RESULT_ERROR;
    }

    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring base64PublicKey_jstr = env->NewStringUTF(base64PublicKey);
    jbyteArray salt_jba = env->NewByteArray(saltLength);
    env->SetByteArrayRegion(salt_jba, 0, saltLength, (jbyte*)salt);
    env->CallVoidMethod(g_Obj, g_s3eAndroidXAPKGetFiles, base64PublicKey_jstr, salt_jba);
    env->DeleteLocalRef(salt_jba);
    env->DeleteLocalRef(base64PublicKey_jstr);

    return S3E_RESULT_SUCCESS;
}

void responseReceived(JNIEnv* env, jobject obj, jobject response)
{
    // Do stuff here
}