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

void s3eAndroidXAPKGetFiles(const char* base64PublicKey, const void* salt, int32 saltLength)
{
	s3eAndroidXAPKGetFiles_platform(base64PublicKey, salt, saltLength);
}
