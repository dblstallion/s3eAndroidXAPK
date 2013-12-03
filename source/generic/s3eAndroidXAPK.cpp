/*
Generic implementation of the s3eAndroidXAPK extension.
This file should perform any platform-indepedentent functionality
(e.g. error checking) before calling platform-dependent implementations.
*/

/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */


#include "s3eAndroidXAPK_internal.h"
s3eResult s3eAndroidXAPKInit()
{
    //Add any generic initialisation code here
    return s3eAndroidXAPKInit_platform();
}

void s3eAndroidXAPKTerminate()
{
    //Add any generic termination code here
    s3eAndroidXAPKTerminate_platform();
}

s3eResult s3eAndroidXAPKRegister(s3eAndroidXAPKCallback callbackID, s3eCallback callbackFn, void* userData)
{
	return s3eAndroidXAPKRegister_platform(callbackID, callbackFn, userData);
}

s3eResult s3eAndroidXAPKUnRegister(s3eAndroidXAPKCallback callbackID, s3eCallback callbackFn)
{
	return s3eAndroidXAPKUnRegister_platform(callbackID, callbackFn);
}

s3eResult s3eAndroidXAPKGetFiles(const char* base64PublicKey, const void* salt, int32 saltLength)
{
	return s3eAndroidXAPKGetFiles_platform(base64PublicKey, salt, saltLength);
}
