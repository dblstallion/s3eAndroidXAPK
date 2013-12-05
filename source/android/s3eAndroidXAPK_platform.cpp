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

typedef enum s3eAndroidXAPKCallback
{
    /**
     * Called after s3eAndroidXAPKGetFiles is called to return the response.
     * systemData is a pointer to an s3eAndroidXAPKResponse instance
     */
    s3eAndroidXAPKCallback_ResponseReceived,

    // Marker for the last callback
    s3eAndroidXAPKCallback_MAX
} s3eAndroidXAPKCallback;

static jobject g_Obj;
static jmethodID g_s3eAndroidXAPKGetFiles;

static jfieldID g_fileName;
static jfieldID g_fileUrl;
static jfieldID g_fileSize;

static jfieldID g_responseResult;
static jfieldID g_responseFiles;

void responseReceived(JNIEnv* env, jobject obj, jobject responseObj);
void responseCleanup(uint32 extID, int32 notification, void* systemData, void* instance, int32 returnCode, void* completeData);

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

    g_s3eAndroidXAPKGetFiles = env->GetMethodID(cls, "s3eAndroidXAPKGetFiles", "(Ljava/lang/String;[B)V");
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

s3eResult s3eAndroidXAPKGetFiles_platform(const char* base64PublicKey, const void* salt, int32 saltLength, s3eCallback callbackFn, void* userData)
{
    if (saltLength != 20)
    {
        s3eEdkErrorSet(S3E_EXT_ANDROIDXAPK_HASH, 1, S3E_EXT_ERROR_PRI_NORMAL);
        s3eEdkErrorSetString("Salt must be 20 bytes long");
        return S3E_RESULT_ERROR;
    }

    if(s3eEdkCallbacksRegister(S3E_EXT_ANDROIDXAPK_HASH, s3eAndroidXAPKCallback_MAX, s3eAndroidXAPKCallback_ResponseReceived, callbackFn, userData, false) != S3E_RESULT_SUCCESS)
        return S3E_RESULT_ERROR;

    JNIEnv* env = s3eEdkJNIGetEnv();
    jstring base64PublicKey_jstr = env->NewStringUTF(base64PublicKey);
    jbyteArray salt_jba = env->NewByteArray(saltLength);
    env->SetByteArrayRegion(salt_jba, 0, saltLength, (jbyte*)salt);
    env->CallVoidMethod(g_Obj, g_s3eAndroidXAPKGetFiles, base64PublicKey_jstr, salt_jba);
    env->DeleteLocalRef(salt_jba);
    env->DeleteLocalRef(base64PublicKey_jstr);

    return S3E_RESULT_SUCCESS;
}

void responseReceived(JNIEnv* env, jobject obj, jobject responseObj)
{
    IwTrace(s3eAndroidXAPK, ("Response received"));
    if (s3eEdkCallbacksIsRegistered(S3E_EXT_ANDROIDXAPK_HASH, s3eAndroidXAPKCallback_ResponseReceived))
    {
        IwTrace(s3eAndroidXAPK, ("Creating response object"));
        s3eAndroidXAPKResponse response;

        response.result = (s3eAndroidXAPKResult)env->GetIntField(responseObj, g_responseResult);
        jobjectArray fileArray = (jobjectArray)env->GetObjectField(responseObj, g_responseFiles);
        if(fileArray)
        {
            jsize count = env->GetArrayLength(fileArray);

            response.fileCount = count;
            s3eAndroidXAPKFile* files = reinterpret_cast<s3eAndroidXAPKFile*>(s3eEdkMallocOS(count * sizeof(s3eAndroidXAPKFile)));
            for(int i = 0; i < count; ++i)
            {
                jobject file = env->GetObjectArrayElement(fileArray, i);

                jstring fileName = (jstring)env->GetObjectField(file, g_fileName);
                jstring fileUrl = (jstring)env->GetObjectField(file, g_fileUrl);
                jlong fileSize = env->GetLongField(file, g_fileSize);

                files[i].name = fileName ? s3eEdkGetStringUTF8Chars(fileName) : NULL;
                files[i].url = fileUrl ? s3eEdkGetStringUTF8Chars(fileUrl) : NULL;
                files[i].size = fileSize;

                if (fileName)
                    env->DeleteLocalRef(fileName);
                if (fileUrl)
                    env->DeleteLocalRef(fileUrl);
                env->DeleteLocalRef(file);
            }
            response.files = files;
            env->DeleteLocalRef(fileArray);
        }
        else
        {
            response.fileCount = 0;
            response.files = NULL;
        }

        s3eEdkCallbacksEnqueue(S3E_EXT_ANDROIDXAPK_HASH, s3eAndroidXAPKCallback_ResponseReceived, &response, sizeof(response), NULL, true, responseCleanup);
    }
}

void responseCleanup(uint32 extID, int32 notification, void* systemData, void* instance, int32 returnCode, void* completeData)
{
    s3eAndroidXAPKResponse* response = reinterpret_cast<s3eAndroidXAPKResponse*>(systemData);

    for(int i = 0; i < response->fileCount; ++i)
    {
        if (response->files[i].name)
            s3eEdkReleaseStringUTF8Chars(response->files[i].name);
        if (response->files[i].url)
            s3eEdkReleaseStringUTF8Chars(response->files[i].url);
    }

    s3eEdkFreeOS((void*)response->files);
}