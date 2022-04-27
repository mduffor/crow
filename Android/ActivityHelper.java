package com.muckyrobot.crow

import android.app.Activity;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager
import android.content.res.Configuration;

import android.os.Bundle;
import android.util.Log

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;


// Note:  To use this class, define the proxy classes in the AndroidManifest as follows:
//
//    <meta-data android:name="com.class.name" android:value="CrowProxyClass" \>
//
//  The name contains the full class name, and the value contains CrowProxyClass, which
//  we will search for.  In the app's onCreate method, instantiate an instance of
//  ProxyActivityHelper, and it will find all of the CrowProxyClass tagged classes.
//  Then in your app's main class, it can call the associated methods on ProxyActivityHelper
//  and the calls will be forwarded to all of the marked classes.


//=============================================================================
public class ProxyActivityHelper
  {
  private final String LOG_TAG = "Crow";
  private List<Class<?>> listProxyClasses = new ArrayList<Class<?>>();
  private Activity context;


  //---------------------------------------------------------------------------
  public ProxyActivityHelper (Activity context_in)
    {
    context = context_in;
    }

  //---------------------------------------------------------------------------
  public void onCreate (Bundle bundleSavedState)
    {
    Bundle bundleMetaData;

    try
      {
      // find all of the meta-data tags in the AndroidManifest
      ApplicationInfo  appInfo = context.getPackageManager().getApplicationInfo (context.getPackageName(), PackageManager.GET_META_DATA);
      bundleMetaData = appInfo.metaData;

      for (String key : bundleMetaData.keySet ())
        {
        try
          {
          Object objValue = bundleMetaData.get(key);
          if ((objValue instanceof String))
            {
            String  strValue = (String) objValue;

            if (strValue.equalsIgnoreCase ("CrowProxyClass"))
              {
              // Found a marked class.  Try to instantiate it so you know it exists.
              try
                {
                Class<?> objClass = Class.forName(key);
                listProxyClasses.add (objClass);

                Log.i(LOG_TAG, "Found Activity proxy class: " + objClass);
                }
              catch (ClassNotFoundException e)
                {
                Log.e(LOG_TAG, "No proxy class found for " + key);
                }
              }
            }
          }
        catch (Exception e)
          {
          }
        }
      }
    catch (PackageManager.NameNotFoundException e)
      {
      Log.i(LOG_TAG, "Failed to load meta-data, NameNotFound: " + e.getMessage());
      }
    catch (NullPointerException e)
      {
      Log.e(LOG_TAG, "Failed to load meta-data, NullPointer: " + e.getMessage());
      }

    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onCreate", new Class[] {Bundle.class});
        refMethod.invoke (null, new Object[] {bundleSavedState });
        }
      catch (Exception e)
        {
        }
      }
    }

  //---------------------------------------------------------------------------
  public void onNewIntent (Intent intent)
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onNewIntent", new Class[] {Intent.class });
        refMethod.invoke (null, new Object[] {intent});
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onActivityResult (int request, int response, Intent data)
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onActivityResult", new Class[] {Integer.TYPE, Integer.TYPE, Intent.class });
        refMethod.invoke (null, new Object[] {Integer.valueOf(request), Integer.valueOf(response), data });
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onStart ()
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onStart", new Class[0]);
        refMethod.invoke (null, new Object[0]);
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onStop ()
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onStop", new Class[0]);
        refMethod.invoke (null, new Object[0]);
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onDestroy ()
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onDestroy", new Class[0]);
        refMethod.invoke (null, new Object[0]);
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onRestart ()
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onRestart", new Class[0]);
        refMethod.invoke (null, new Object[0]);
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onPause ()
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onPause", new Class[0]);
        refMethod.invoke (null, new Object[0]);
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onResume ()
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onResume", new Class[0]);
        refMethod.invoke (null, new Object[0]);
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onBackPressed ()
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onBackPressed", new Class[0]);
        refMethod.invoke (null, new Object[0]);
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onSaveInstanceState (Bundle outState)
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onSaveInstanceState", new Class[] {Bundle.class});
        refMethod.invoke (null, new Object[] {outState});
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onRestoreInstanceState (Bundle savedInstanceState)
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onRestoreInstanceState", new Class[] {Bundle.class});
        refMethod.invoke (null, new Object[] {savedInstanceState});
        }
      catch (Exception e) {}
      }
    }

  //---------------------------------------------------------------------------
  public void onConfigurationChanged (Configuration newConfig)
    {
    for (Class<?> classCurr : listProxyClasses)
      {
      try
        {
        Method refMethod = classCurr.getMethod ("onConfigurationChanged", new Class[] {Configuration.class});
        refMethod.invoke (null, new Object[] {newConfig});
        }
      catch (Exception e) {}
      }
    }


  }

