/* -----------------------------------------------------------------
                       Android Helper

     This module implements helper functions for interfacing with
   Android NDK apps.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2015, Michael T. Duffy II.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
// Redistributions of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGE.

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "android/keycodes.h"
#include "Sys/InputManager.hpp"
#include "Android/AndroidHelper.hpp"
#if defined( ANDROID )

TJava                 AndroidHelper::java;
struct android_app *  AndroidHelper::app;

//-------------------------------------------------------------------------
INT  AndroidHelper::TranslateVKey (INT  iKeyCodeIn)
  {
  if ((iKeyCodeIn >= AKEYCODE_A) && (iKeyCodeIn <= AKEYCODE_Z))
    {
    return ('A' + iKeyCodeIn - AKEYCODE_A);
    }
  else if ((iKeyCodeIn >= AKEYCODE_0) && (iKeyCodeIn <= AKEYCODE_9))
    {
    return ('0' + iKeyCodeIn - AKEYCODE_0);
    }
  else
    {
    switch (iKeyCodeIn)
      {
      case AKEYCODE_ALT_LEFT:         return (VKEY_ALT);
      case AKEYCODE_ALT_RIGHT:        return (VKEY_ALT);

      case AKEYCODE_APOSTROPHE:       return ('\'');
      case AKEYCODE_AT:               return ('@');
      case AKEYCODE_BACK:             return (VKEY_ESC); // hardware back button
      case AKEYCODE_BACKSLASH:        return ('\\');
      case AKEYCODE_BREAK:            return (VKEY_PAUSE);

      // requires API level 12 or later
      case AKEYCODE_BUTTON_1:         return (GAMEPAD_BUTTON_1);
      case AKEYCODE_BUTTON_2:         return (GAMEPAD_BUTTON_2);
      case AKEYCODE_BUTTON_3:         return (GAMEPAD_BUTTON_3);
      case AKEYCODE_BUTTON_4:         return (GAMEPAD_BUTTON_4);
      case AKEYCODE_BUTTON_5:         return (GAMEPAD_BUTTON_5);
      case AKEYCODE_BUTTON_6:         return (GAMEPAD_BUTTON_6);
      case AKEYCODE_BUTTON_7:         return (GAMEPAD_BUTTON_7);
      case AKEYCODE_BUTTON_8:         return (GAMEPAD_BUTTON_8);
      case AKEYCODE_BUTTON_9:         return (GAMEPAD_BUTTON_9);
      case AKEYCODE_BUTTON_10:        return (GAMEPAD_BUTTON_10);
      case AKEYCODE_BUTTON_11:        return (GAMEPAD_BUTTON_11);
      case AKEYCODE_BUTTON_12:        return (GAMEPAD_BUTTON_12);
      case AKEYCODE_BUTTON_13:        return (GAMEPAD_BUTTON_13);
      case AKEYCODE_BUTTON_14:        return (GAMEPAD_BUTTON_14);
      case AKEYCODE_BUTTON_15:        return (GAMEPAD_BUTTON_15);
      case AKEYCODE_BUTTON_16:        return (GAMEPAD_BUTTON_16);

      case AKEYCODE_BUTTON_A:         return (BUTTON_A);
      case AKEYCODE_BUTTON_B:         return (BUTTON_B);
      case AKEYCODE_BUTTON_C:         return (BUTTON_C);
      case AKEYCODE_BUTTON_X:         return (BUTTON_X);
      case AKEYCODE_BUTTON_Y:         return (BUTTON_Y);
      case AKEYCODE_BUTTON_Z:         return (BUTTON_Z);
      case AKEYCODE_BUTTON_L1:        return (BUTTON_L1);
      case AKEYCODE_BUTTON_L2:        return (BUTTON_L2);
      case AKEYCODE_BUTTON_R1:        return (BUTTON_R1);
      case AKEYCODE_BUTTON_R2:        return (BUTTON_R2);
      case AKEYCODE_BUTTON_THUMBL:    return (BUTTON_THUMBL);
      case AKEYCODE_BUTTON_THUMBR:    return (BUTTON_THUMBR);
      case AKEYCODE_BUTTON_MODE:      return (BUTTON_MODE);
      case AKEYCODE_BUTTON_SELECT:    return (BUTTON_SELECT);
      case AKEYCODE_BUTTON_START:     return (BUTTON_START);
      case AKEYCODE_COMMA:            return (',');
      case AKEYCODE_EQUALS:           return ('=');
      case AKEYCODE_GRAVE:            return ('`');
      case AKEYCODE_LEFT_BRACKET:     return ('[');
      case AKEYCODE_RIGHT_BRACKET:    return (']');
      case AKEYCODE_MINUS:            return ('-');
      case AKEYCODE_PLUS:             return ('+');
      case AKEYCODE_PERIOD:           return ('.');
      case AKEYCODE_POUND:            return ('#');
      case AKEYCODE_SEMICOLON:        return (';');
      case AKEYCODE_SLASH:            return ('/');
      case AKEYCODE_SPACE:            return (VKEY_SPACE);
      case AKEYCODE_STAR:             return ('*');
      case AKEYCODE_TAB:              return (VKEY_TAB);
      case AKEYCODE_F1:               return (VKEY_F1);
      case AKEYCODE_F2:               return (VKEY_F2);
      case AKEYCODE_F3:               return (VKEY_F3);
      case AKEYCODE_F4:               return (VKEY_F4);
      case AKEYCODE_F5:               return (VKEY_F5);
      case AKEYCODE_F6:               return (VKEY_F6);
      case AKEYCODE_F7:               return (VKEY_F7);
      case AKEYCODE_F8:               return (VKEY_F8);
      case AKEYCODE_F9:               return (VKEY_F9);
      case AKEYCODE_F10:              return (VKEY_F10);
      case AKEYCODE_F11:              return (VKEY_F11);
      case AKEYCODE_F12:              return (VKEY_F12);
      case AKEYCODE_PAGE_UP:          return (VKEY_PAGEUP);
      case AKEYCODE_PAGE_DOWN:        return (VKEY_PAGEDOWN);
      case AKEYCODE_SHIFT_LEFT:       return (VKEY_SHIFT);
      case AKEYCODE_SHIFT_RIGHT:      return (VKEY_SHIFT);
      case AKEYCODE_DEL:              return (VKEY_BACKSPACE);
      case AKEYCODE_CTRL_LEFT:        return (VKEY_CONTROL);
      case AKEYCODE_CTRL_RIGHT:       return (VKEY_CONTROL);
      case AKEYCODE_ENTER:            return (VKEY_ENTER);
      case AKEYCODE_ESCAPE:           return (VKEY_ESC);
      case AKEYCODE_HOME:             return (VKEY_HOME);
      case AKEYCODE_INSERT:           return (VKEY_INSERT);
      case AKEYCODE_MENU:             return (VKEY_MENU);
      case AKEYCODE_NUMPAD_0:         return (VKEY_NUMPAD0);
      case AKEYCODE_NUMPAD_1:         return (VKEY_NUMPAD1);
      case AKEYCODE_NUMPAD_2:         return (VKEY_NUMPAD2);
      case AKEYCODE_NUMPAD_3:         return (VKEY_NUMPAD3);
      case AKEYCODE_NUMPAD_4:         return (VKEY_NUMPAD4);
      case AKEYCODE_NUMPAD_5:         return (VKEY_NUMPAD5);
      case AKEYCODE_NUMPAD_6:         return (VKEY_NUMPAD6);
      case AKEYCODE_NUMPAD_7:         return (VKEY_NUMPAD7);
      case AKEYCODE_NUMPAD_8:         return (VKEY_NUMPAD8);
      case AKEYCODE_NUMPAD_9:         return (VKEY_NUMPAD9);
      case AKEYCODE_NUMPAD_ADD:       return (VKEY_NUMPAD_ADD);
      case AKEYCODE_NUMPAD_DIVIDE:    return (VKEY_NUMPAD_DIVIDE);
      case AKEYCODE_NUMPAD_DOT:       return (VKEY_NUMPAD_DECIMAL);
      case AKEYCODE_NUMPAD_SUBTRACT:  return (VKEY_NUMPAD_SUBTRACT);
      case AKEYCODE_NUMPAD_MULTIPLY:  return (VKEY_NUMPAD_MULTIPLY);
      case AKEYCODE_NUMPAD_EQUALS:    return (VKEY_NUMPAD_EQUAL);
      case AKEYCODE_NUMPAD_ENTER:     return (VKEY_NUMPAD_ENTER);
      case AKEYCODE_DPAD_CENTER:      return (DPAD_CENTER);
      case AKEYCODE_DPAD_DOWN:        return (DPAD_DOWN);
      case AKEYCODE_DPAD_LEFT:        return (DPAD_LEFT);
      case AKEYCODE_DPAD_RIGHT:       return (DPAD_RIGHT);
      case AKEYCODE_DPAD_UP:          return (DPAD_UP);
      case AKEYCODE_MOVE_END:         return (VKEY_END);
      case AKEYCODE_MOVE_HOME:        return (VKEY_HOME);
      //case KeyEvent.:  return (VKEY_);

      default:
        return (VKEY_INVALID);
      }
    }
  }


//-------------------------------------------------------------------------
jclass AndroidHelper::RetrieveClass (JNIEnv *     jniEnvIn,
                                     const char * szClassNameIn)
  {
  jclass    classActivity     = jniEnvIn->FindClass ("android/app/NativeActivity");
  jmethodID midGetClassLoader = jniEnvIn->GetMethodID (classActivity,
                                                       "getClassLoader",
                                                       "()Ljava/lang/ClassLoader;");
  jobject cls = jniEnvIn->CallObjectMethod (app->activity->clazz, midGetClassLoader);
  jclass classLoader = jniEnvIn->FindClass ("java/lang/ClassLoader");
  jmethodID midFindClass = jniEnvIn->GetMethodID (classLoader,
                                                  "loadClass",
                                                  "(Ljava/lang/String;)Ljava/lang/Class;" );

  jstring jstrClassName = jniEnvIn->NewStringUTF (szClassNameIn);
  jclass classRetrieved = (jclass) jniEnvIn->CallObjectMethod (cls, midFindClass, jstrClassName);
  jniEnvIn->DeleteLocalRef (jstrClassName);
  return classRetrieved;
  };


//-----------------------------------------------------------------------------
VOID  AndroidHelper::GetJString (JNIEnv *  jniEnvIn,
                                 jstring & jstrIn,
                                 RStr &    strOut)
  {
  const jsize len     = jniEnvIn->GetStringUTFLength (jstrIn);
  const char* szChars = jniEnvIn->GetStringUTFChars  (jstrIn, (jboolean *)0);
  strOut.AppendChars (szChars, len);
  jniEnvIn->ReleaseStringUTFChars (jstrIn, szChars);
  };

#endif



/*
// Unhandled KeyEvent s from TranslateVKey (java constants)

int   KEYCODE_CLEAR   Key code constant: Clear key.
int   KEYCODE_FORWARD   Key code constant: Forward key.
int   KEYCODE_NUM_LOCK  Key code constant: Num Lock key.
int   KEYCODE_NUMPAD_COMMA  Key code constant: Numeric keypad ',' key (for decimals or digit grouping).
int   KEYCODE_NUMPAD_LEFT_PAREN   Key code constant: Numeric keypad '(' key.
int   KEYCODE_NUMPAD_RIGHT_PAREN  Key code constant: Numeric keypad ')' key.

int   KEYCODE_CAPS_LOCK   Key code constant: Caps Lock key.
int   KEYCODE_3D_MODE   Key code constant: 3D Mode key.
int   KEYCODE_APP_SWITCH  Key code constant: App switch key.
int   KEYCODE_ASSIST  Key code constant: Assist key.
int   KEYCODE_AVR_INPUT   Key code constant: A/V Receiver input key.
int   KEYCODE_AVR_POWER   Key code constant: A/V Receiver power key.
int   KEYCODE_BOOKMARK  Key code constant: Bookmark key.
int   KEYCODE_BRIGHTNESS_DOWN   Key code constant: Brightness Down key.
int   KEYCODE_BRIGHTNESS_UP   Key code constant: Brightness Up key.
int   KEYCODE_CALCULATOR  Key code constant: Calculator special function key.
int   KEYCODE_CALENDAR  Key code constant: Calendar special function key.
int   KEYCODE_CALL  Key code constant: Call key.
int   KEYCODE_CAMERA  Key code constant: Camera key.
int   KEYCODE_CAPTIONS  Key code constant: Toggle captions key.
int   KEYCODE_CHANNEL_DOWN  Key code constant: Channel down key.
int   KEYCODE_CHANNEL_UP  Key code constant: Channel up key.
int   KEYCODE_CONTACTS  Key code constant: Contacts special function key.
int   KEYCODE_DVR   Key code constant: DVR key.
int   KEYCODE_EISU  Key code constant: Japanese alphanumeric key.
int   KEYCODE_ENDCALL   Key code constant: End Call key.
int   KEYCODE_ENVELOPE  Key code constant: Envelope special function key.
int   KEYCODE_EXPLORER  Key code constant: Explorer special function key.
int   KEYCODE_FOCUS   Key code constant: Camera Focus key.
int   KEYCODE_FORWARD_DEL   Key code constant: Forward Delete key.
int   KEYCODE_FUNCTION  Key code constant: Function modifier key.
int   KEYCODE_GUIDE   Key code constant: Guide key.
int   KEYCODE_HEADSETHOOK   Key code constant: Headset Hook key.
int   KEYCODE_HENKAN  Key code constant: Japanese conversion key.
int   KEYCODE_INFO  Key code constant: Info key.
int   KEYCODE_KANA  Key code constant: Japanese kana key.
int   KEYCODE_KATAKANA_HIRAGANA   Key code constant: Japanese katakana / hiragana key.
int   KEYCODE_LANGUAGE_SWITCH   Key code constant: Language Switch key.
int   KEYCODE_MANNER_MODE   Key code constant: Manner Mode key.
int   KEYCODE_MEDIA_AUDIO_TRACK   Key code constant: Audio Track key.
int   KEYCODE_MEDIA_CLOSE   Key code constant: Close media key.
int   KEYCODE_MEDIA_EJECT   Key code constant: Eject media key.
int   KEYCODE_MEDIA_FAST_FORWARD  Key code constant: Fast Forward media key.
int   KEYCODE_MEDIA_NEXT  Key code constant: Play Next media key.
int   KEYCODE_MEDIA_PAUSE   Key code constant: Pause media key.
int   KEYCODE_MEDIA_PLAY  Key code constant: Play media key.
int   KEYCODE_MEDIA_PLAY_PAUSE  Key code constant: Play/Pause media key.
int   KEYCODE_MEDIA_PREVIOUS  Key code constant: Play Previous media key.
int   KEYCODE_MEDIA_RECORD  Key code constant: Record media key.
int   KEYCODE_MEDIA_REWIND  Key code constant: Rewind media key.
int   KEYCODE_MEDIA_STOP  Key code constant: Stop media key.
int   KEYCODE_META_LEFT   Key code constant: Left Meta modifier key.
int   KEYCODE_META_RIGHT  Key code constant: Right Meta modifier key.
int   KEYCODE_MUHENKAN  Key code constant: Japanese non-conversion key.
int   KEYCODE_MUSIC   Key code constant: Music special function key.
int   KEYCODE_MUTE  Key code constant: Mute key.
int   KEYCODE_NOTIFICATION  Key code constant: Notification key.
int   KEYCODE_NUM   Key code constant: Number modifier key.
int   KEYCODE_PICTSYMBOLS   Key code constant: Picture Symbols modifier key.
int   KEYCODE_POWER   Key code constant: Power key.
int   KEYCODE_PROG_BLUE   Key code constant: Blue "programmable" key.
int   KEYCODE_PROG_GREEN  Key code constant: Green "programmable" key.
int   KEYCODE_PROG_RED  Key code constant: Red "programmable" key.
int   KEYCODE_PROG_YELLOW   Key code constant: Yellow "programmable" key.
int   KEYCODE_RO  Key code constant: Japanese Ro key.
int   KEYCODE_SCROLL_LOCK   Key code constant: Scroll Lock key.
int   KEYCODE_SEARCH  Key code constant: Search key.
int   KEYCODE_SETTINGS  Key code constant: Settings key.
int   KEYCODE_SLEEP   Key code constant: Sleep key.
int   KEYCODE_SOFT_LEFT   Key code constant: Soft Left key.
int   KEYCODE_SOFT_RIGHT  Key code constant: Soft Right key.
int   KEYCODE_STB_INPUT   Key code constant: Set-top-box input key.
int   KEYCODE_STB_POWER   Key code constant: Set-top-box power key.
int   KEYCODE_SWITCH_CHARSET  Key code constant: Switch Charset modifier key.
int   KEYCODE_SYM   Key code constant: Symbol modifier key.
int   KEYCODE_SYSRQ   Key code constant: System Request / Print Screen key.
int   KEYCODE_TV  Key code constant: TV key.
int   KEYCODE_TV_INPUT  Key code constant: TV input key.
int   KEYCODE_TV_POWER  Key code constant: TV power key.
int   KEYCODE_UNKNOWN   Key code constant: Unknown key code.
int   KEYCODE_VOLUME_DOWN   Key code constant: Volume Down key.
int   KEYCODE_VOLUME_MUTE   Key code constant: Volume Mute key.
int   KEYCODE_VOLUME_UP   Key code constant: Volume Up key.
int   KEYCODE_WAKEUP  Key code constant: Wakeup key.
int   KEYCODE_WINDOW  Key code constant: Window key.
int   KEYCODE_YEN   Key code constant: Japanese Yen key.
int   KEYCODE_ZENKAKU_HANKAKU   Key code constant: Japanese full-width / half-width key.
int   KEYCODE_ZOOM_IN   Key code constant: Zoom in key.
int   KEYCODE_ZOOM_OUT  Key code constant: Zoom out key.

int   META_ALT_LEFT_ON

This mask is used to check whether the left ALT meta key is pressed.
int   META_ALT_MASK   This mask is a combination of META_ALT_ON, META_ALT_LEFT_ON and META_ALT_RIGHT_ON.
int   META_ALT_ON

This mask is used to check whether one of the ALT meta keys is pressed.
int   META_ALT_RIGHT_ON

This mask is used to check whether the right the ALT meta key is pressed.
int   META_CAPS_LOCK_ON

This mask is used to check whether the CAPS LOCK meta key is on.
int   META_CTRL_LEFT_ON

This mask is used to check whether the left CTRL meta key is pressed.
int   META_CTRL_MASK  This mask is a combination of META_CTRL_ON, META_CTRL_LEFT_ON and META_CTRL_RIGHT_ON.
int   META_CTRL_ON

This mask is used to check whether one of the CTRL meta keys is pressed.
int   META_CTRL_RIGHT_ON

This mask is used to check whether the right CTRL meta key is pressed.
int   META_FUNCTION_ON

This mask is used to check whether the FUNCTION meta key is pressed.
int   META_META_LEFT_ON

This mask is used to check whether the left META meta key is pressed.
int   META_META_MASK  This mask is a combination of META_META_ON, META_META_LEFT_ON and META_META_RIGHT_ON.
int   META_META_ON

This mask is used to check whether one of the META meta keys is pressed.
int   META_META_RIGHT_ON

This mask is used to check whether the right META meta key is pressed.
int   META_NUM_LOCK_ON

This mask is used to check whether the NUM LOCK meta key is on.
int   META_SCROLL_LOCK_ON

This mask is used to check whether the SCROLL LOCK meta key is on.
int   META_SHIFT_LEFT_ON

This mask is used to check whether the left SHIFT meta key is pressed.
int   META_SHIFT_MASK   This mask is a combination of META_SHIFT_ON, META_SHIFT_LEFT_ON and META_SHIFT_RIGHT_ON.
int   META_SHIFT_ON

This mask is used to check whether one of the SHIFT meta keys is pressed.
int   META_SHIFT_RIGHT_ON

This mask is used to check whether the right SHIFT meta key is pressed.
int   META_SYM_ON

This mask is used to check whether the SYM meta key is pressed.
*/

