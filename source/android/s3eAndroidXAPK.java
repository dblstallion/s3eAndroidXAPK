/*
java implementation of the s3eAndroidXAPK extension.

Add android-specific functionality here.

These functions are called via JNI from native code.
*/
/*
 * NOTE: This file was originally written by the extension builder, but will not
 * be overwritten (unless --force is specified) and is intended to be modified.
 */
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.LoaderActivity;

import com.google.android.vending.licensing.AESObfuscator;
import com.google.android.vending.licensing.APKExpansionPolicy;
import com.google.android.vending.licensing.LicenseChecker;
import com.google.android.vending.licensing.LicenseCheckerCallback;
import com.google.android.vending.licensing.Policy;

import android.content.Context;

import android.provider.Settings.Secure;

class s3eAndroidXAPK
{
    public class File
    {
        public String name;
        public String url;
        public long size;
    }
    
    public class Response
    {
        int result;
        File[] files;
    }
    
    //The native function that receives the result of the license check
	public native void responseReceived(Response response);
    
    public void s3eAndroidXAPKGetFiles(String base64PublicKey, byte[] salt)
    {
        final Context context = LoaderActivity.m_Activity;
        
        String deviceId = Secure.getString(context.getContentResolver(), Secure.ANDROID_ID);
        
        // Construct XAPK policy
        final APKExpansionPolicy aep = new APKExpansionPolicy(context, new AESObfuscator(salt, context.getPackageName(), deviceId));
        
        // reset our policy back to the start of the world to force a
        // re-check
        aep.resetPolicy();

        // let's try and get the OBB file from LVL first
        // Construct the LicenseChecker with a Policy.
        final LicenseChecker checker = new LicenseChecker(context, aep, base64PublicKey);
        
        checker.checkAccess(new LicenseCheckerCallback()
        {
            @Override
            public void allow(int reason)
            {
                Response response = new Response();
                
                int count = aep.getExpansionURLCount();
                response.files = new File[count];
                for (int i = 0; i < count; i++)
                {
                    File file = new File();
                    file.name = aep.getExpansionFileName(i);
                    file.url = aep.getExpansionURL(i);
                    file.size = aep.getExpansionFileSize(i);
                    
                    response.files[i] = file;
                }
                
                switch(reason)
                {
                case Policy.LICENSED:
                	response.result = 0;
                	break;
                case Policy.NOT_LICENSED:
                	response.result = 1;
                	break;
                case Policy.RETRY:
                default:
                	response.result = 2;
                	break;
                }
                
                responseReceived(response);
            }

            @Override
            public void dontAllow(int reason) {
                Response response = new Response();

                switch(reason)
                {
                case Policy.LICENSED:
                	response.result = 0;
                	break;
                case Policy.NOT_LICENSED:
                	response.result = 1;
                	break;
                case Policy.RETRY:
                default:
                	response.result = 2;
                	break;
                }
                
                responseReceived(response);
            }

            @Override
            public void applicationError(int errorCode) {
                Response response = new Response();
                response.result = errorCode + 2;
                
                responseReceived(response);
            }
        });
    }
}
