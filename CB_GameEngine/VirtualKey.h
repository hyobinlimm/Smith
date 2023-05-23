#pragma once

/// <summary>
/// 마이크로소프트에서 지원하는 Virtual Key-Code에 1:1 대응하는 열거형입니다.
/// 
/// </summary>
enum class VirtualKey
{
	/*
	 * Mouse virtual keys
	 */

	MouseLeft = 0x01,
	MouseRight = 0x02,
	MouseMiddle = 0x04,
	// mouse extension button 1
	XButton1 = 0x05,
	// mouse extension button 2
	XButton2 = 0x06,

	/*
	 * Keyboard virtual keys
	 */

	 Backspace = 0x08,
	 Tab = 0x09,
	 // VK_RETURN
	 Enter = 0x0D,
	 Shift = 0x10,
	 Control = 0x11,
	 // VK_MENU
	 Alt = 0x12,
	 Pause = 0x13,
	 CapsLock = 0x14,
	 // VK_ESCAPE
	 Escape = 0x1B,
	 SpaceBar = 0x20,
	 // VK_PRIOR
	 PageUp = 0x21,
	 // VK_NEXT
	 PageDown = 0x22,
	 End = 0x23,
	 Home = 0x24,
	 LeftArrow = 0x25,
	 UpArrow = 0x26,
	 RightArrow = 0x27,
	 DownArrow = 0x28,
	 // VK_SNAPSHOT
	 PrintScreen = 0x2C,
	 Insert = 0x2D,
	 Delete = 0x2E,
	 Key_1 = 0x30,
	 Key_2 = 0x31,
	 Key_3 = 0x32,
	 Key_4 = 0x33,
	 Key_5 = 0x34,
	 Key_6 = 0x35,
	 Key_7 = 0x36,
	 Key_8 = 0x37,
	 Key_9 = 0x38,
	 Key_A = 0x41,
	 Key_B = 0x42,
	 Key_C = 0x43,
	 Key_D = 0x44,
	 Key_E = 0x45,
	 Key_F = 0x46,
	 Key_G = 0x47,
	 Key_H = 0x48,
	 Key_I = 0x49,
	 Key_J = 0x4A,
	 Key_K = 0x4B,
	 Key_L = 0x4C,
	 Key_M = 0x4D,
	 Key_N = 0x4E,
	 Key_O = 0x4F,
	 Key_P = 0x50,
	 Key_Q = 0x51,
	 Key_R = 0x52,
	 Key_S = 0x53,
	 Key_T = 0x54,
	 Key_U = 0x55,
	 Key_V = 0x56,
	 Key_W = 0x57,
	 Key_X = 0x58,
	 Key_Y = 0x59,
	 Key_Z = 0x5A,
	 // VK_LWIN
	 LeftWindow = 0x5B,
	 // VK_RWIN
	 RightWindow = 0x5C,
	 // Key ','
	 Separator = 0x6C,
	 Numpad_0 = 0x60,
	 Numpad_1 = 0x61,
	 Numpad_2 = 0x62,
	 Numpad_3 = 0x63,
	 Numpad_4 = 0x64,
	 Numpad_5 = 0x65,
	 Numpad_6 = 0x66,
	 Numpad_7 = 0x67,
	 Numpad_8 = 0x68,
	 Numpad_9 = 0x69,
	 // Numpad '*'
	 Multiply = 0x6A,
	 // Key '+'
	 Add = 0x6B,
	 // Numpad '.'
	 Decimal = 0x6E,
	 // Key '-'
	 Subtract = 0x6D,
	 // Key '/'
	 Divide = 0x6F,
	 F1 = 0x70,
	 F2 = 0x71,
	 F3 = 0x72,
	 F4 = 0x73,
	 F5 = 0x74,
	 F6 = 0x75,
	 F7 = 0x76,
	 F8 = 0x77,
	 F9 = 0x78,
	 F10 = 0x79,
	 F11 = 0x7A,
	 F12 = 0x7B,
	 NumLock = 0x90,
	 ScrollLock = 0x91,
	 LeftShift = 0xA0,
	 RightShift = 0xA1,
	 LeftControl = 0xA2,
	 RightControl = 0xA3,

	 /*
	  * IME keys
	  */

	  // VK_KANA
	  IME_Kana = 0x15,
	  // (deprecated) VK_HANGUEL
	  IME_Hanguel = 0x15,
	  // VK_HANGUL
	  IME_Hangul = 0x15,
	  IME_On = 0x16,
	  IME_Junja = 0x17,
	  IME_Final = 0x18,
	  IME_Kanji = 0x19,
	  IME_Off = 0x1A,
	  IME_Convert = 0x1C,
	  IME_NonConvert = 0x1D,
	  IME_Accept = 0x1E,
	  IME_ModeChange = 0x1F,

	  /*
	   * unused / rarely used
	   */

	   // VK_CANCEL
	   Break = 0x03,
	   Clear = 0x0C,
	   Select = 0x29,
	   // VK_PRINT
	   Print = 0x2A,
	   Execute = 0x2B,
	   Help = 0x2F,
	   // VK_APPS
	   Applications = 0x5D,
	   // VK_SLEEP
	   Sleep = 0x5F,
	   F13 = 0x7C,
	   F14 = 0x7D,
	   F15 = 0x7E,
	   F16 = 0x7F,
	   F17 = 0x80,
	   F18 = 0x81,
	   F19 = 0x82,
	   F20 = 0x83,
	   F21 = 0x84,
	   F22 = 0x85,
	   F23 = 0x86,
	   F24 = 0x87,
	   LeftMenu = 0xA4,
	   RightMenu = 0xA5,
	   Browser_Back = 0xA6,
	   Browser_Forward = 0xA7,
	   Browser_Refresh = 0xA8,
	   Browser_Stop = 0xA9,
	   Browser_Search = 0xAA,
	   Browser_Favorites = 0xAB,
	   Browser_Home = 0xAC,
	   Volume_Mute = 0xAD,
	   Volume_Down = 0xAE,
	   Volume_Up = 0xAF,
	   Media_NextTrack = 0xB0,
	   Media_PrevTrack = 0xB1,
	   Media_Stop = 0xB2,
	   // VK_MEDIA_PLAY_PAUSE
	   Media_PlayOrPause = 0xB3,
	   Launch_Mail = 0xB4,
	   // VK_LAUNCH_MEDIA_SELCT
	   Launce_SelectMedia = 0xB5,
	   Launch_App1 = 0xB6,
	   Launch_App2 = 0xB7,
	   OEM_1 = 0xBA,
	   OEM_Plus = 0xBB,
	   OEM_Comma = 0xBC,
	   OEM_Minus = 0xBD,
	   OEM_Period = 0xBE,
	   OEM_2 = 0xBF,
	   OEM_3 = 0xC0,
	   OEM_4 = 0xDB,
	   OEM_5 = 0xDC,
	   OEM_6 = 0xDD,
	   OEM_7 = 0xDE,
	   OEM_8 = 0xDF,
	   OEM_102 = 0xE2,
	   // VK_PROCESSKEY
	   IME_Process = 0xE5,
	   // VK_PACKET
	   Packet = 0xE7,
	   Attn = 0xF6,
	   CrSel = 0xF7,
	   ExSel = 0xF8,
	   EraseEOF = 0xF9,
	   Play = 0xFA,
	   Zoom = 0xFB,
	   NoName = 0xFC,
	   PA1 = 0xFD,
	   OEM_Clear = 0xFE,

	   // 키입력 없음
	   None = 0xFF,
};
