#include "s3eOSReadString.h"
#include "s3eAndroidXAPK.h"
#include "IwDebug.h"
#include "IwGx.h"
#include "s3eExt.h"
#include "s3ePointer.h"
#include "s3eFile.h"
#include "s3eSurface.h"
#include <string>
#include <sstream>

static const char *g_Status = "Waiting for request";

int32 responseReceived(void *systemData, void *userData)
{
    s3eAndroidXAPKResponse *response = (s3eAndroidXAPKResponse*)systemData;

    switch(response->result)
    {
    case s3eAndroidXAPKResult_CheckInProgress:
        g_Status = "Check in progress";
        break;
    case s3eAndroidXAPKResult_InvalidPackageName:
        g_Status = "Invalid package name";
        break;
    case s3eAndroidXAPKResult_InvalidPublicKey:
        g_Status = "Invalid public key";
        break;
    case s3eAndroidXAPKResult_MissingPermission:
        g_Status = "Missing permission";
        break;
    case s3eAndroidXAPKResult_NonMatchingUID:
        g_Status = "Non matching UID";
        break;
    case s3eAndroidXAPKResult_NotAllowed:
        g_Status = "License check result: not allowed";
        break;
    case s3eAndroidXAPKResult_NotMarketManaged:
        g_Status = "Not market managed";
        break;
    case s3eAndroidXAPKResult_Retry:
        g_Status = "Cannot perform check, try again later";
        break;
    case s3eAndroidXAPKResult_Success:
        g_Status = "License check suceeded";
        break;
    default:
        g_Status = "Unknown response received";
        break;
    }

    return 0;
}

void buttonEvent(s3ePointerTouchEvent *event)
{
    IW_CALLSTACK("buttonEvent");

    if(event->m_Pressed)
    {
        g_Status = "Sending request";

        s3eAndroidXAPKGetFiles("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxlbYXIzMCHNWwtZ3lDUCqJlYNzv/5PqG/VeOK3PGOY3G2ax9eINebzy6XJIrScABZ3os5b8SBF7HcbFBlo0/qe8gByb/j4chqg1nEWdnEjhnr5V4lQdgWKfCvSt2v7/QAKBT7YtMa3Dqi9F9xSTHiITj4y5sxjkMItOi1O8MbT4PSIqel6t/K8qMtsv+VQvLCzNB02KtzfVJVtA6zrUQYV5M7GjaLTkD2uFeTDjJf8Heom0A4ydVvE0J7zMiU9HMS01LvEOIvej1jSiYEp5qIWCpz6lV5FvwWtHQw+6ACfekV7qfo0PLqdMPFv6UCdSBzKOz0gPf9rzeHQuxmSk9lwIDAQAB", "ASDJSAHDKASDUWANDSADHASDPWDJASD", 20, &responseReceived, NULL);
    }
}

// Example showing how to use the s3eWwise extension
int main()
{
    IW_CALLSTACK("main");

    s3eDebugOutputString("Booting XAPK example");

    s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)buttonEvent, NULL);

    IwGxInit();

    IwGxSetColClear(0, 0, 0, 0xff);

    while(!s3eDeviceCheckQuitRequest())
    {
        std::stringstream str;

        IwGxClear();

        IwGxPrintString(100, 100, "s3eAndroidXAPK");

        IwGxPrintString(100, 300, g_Status);

        IwGxFlush();
        IwGxSwapBuffers();
        s3eDeviceYield(0);
    }

    IwGxTerminate();

    s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)buttonEvent);

    return 0;
}