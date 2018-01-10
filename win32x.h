#include <windows.h>
#include <stdio.h>

LPCSTR AppName = "TesT";
#define IDT_TIMER1 901
#define SETTING_INTERVAL_FLASH 200

//below here has to do with old compilers.

#ifndef WM_CONTEXTMENU
  #define WM_CONTEXTMENU 0x007B
#endif

#ifndef IDI_INFORMATION
  #define IDI_INFORMATION MAKEINTRESOURCE(32516)
#endif

