#include <windows.h>
LPCSTR AppName = "TesT";
#define IDT_TIMER1 901
#define SETTING_INTERVAL_FLASH 200

char text_buffer[16384];
int text_buffer_length = -1;
int cursor_loc = 10;
int cursor_state = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  HMENU hM, hSM, hWndList;
  POINT cursor;
  switch(message)
  {
    case WM_TIMER:
      switch( wParam )
      {
        case IDT_TIMER1:
        {
          ++cursor_state;
          if( cursor_state > 127) cursor_state = 0;
          InvalidateRect( hWnd, 0, TRUE );
        }
      }
      break;
    case WM_CREATE:
      hM = CreateMenu();
      hSM = CreatePopupMenu();
      AppendMenu(hSM, MF_STRING, 32, "&Exit");
      AppendMenu(hM, MF_STRING | MF_POPUP, (UINT)hSM, "&File");
      SetMenu(hWnd, hM);
      sprintf( text_buffer, 
        "This is a test string.\n"
        "I like fish.\n"
        "I like girls.\n"
        "\nThis is a dopey text editor. It needs some more features.\n"
        "Mouse select, scrolling, move cursor up and down.\n"
        "Also double buffer the text drawing, and not redraw the whole\n"
        "screen every time the cursor blinks.\n"
        );
      SetTimer( hWnd, IDT_TIMER1, SETTING_INTERVAL_FLASH, (TIMERPROC) NULL );
      /*
      CreateWindowEx(0, "STATIC", "A Static Label", WS_CHILD | WS_VISIBLE, 10, 10, 96, 20, hWnd, (HMENU)2, NULL, NULL);
      CreateWindowEx(0, "BUTTON", "A Button", WS_CHILD | WS_VISIBLE, 10, 40, 72, 20, hWnd, (HMENU)33, NULL, NULL);
      CreateWindowEx(0, "EDIT", "A Textbox", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 70, 72, 20, hWnd, (HMENU)34, NULL, NULL);
      CreateWindowEx(0, "EDIT", "A Big,\n\nMultiline Textbox", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
        110, 10, 100, 100, hWnd, (HMENU)35, NULL, NULL);
      hWndList = CreateWindowEx(0, "LISTBOX", 0, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | LBS_SORT,
        220, 10, 100, 100, hWnd, (HMENU)36, NULL, NULL);
      SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)"A list box");
      SendMessage(GetDlgItem(hWnd, 36), LB_ADDSTRING, 0, (LPARAM)"with 2 items");
      */
      break;
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case 20://Listbox Add
          SendMessage(GetDlgItem(hWnd, 36), LB_ADDSTRING, 0, (LPARAM)"New Item");
          break;
        case 21://Listbox Delete
          SendMessage(GetDlgItem(hWnd, 36), LB_DELETESTRING, SendMessage(GetDlgItem(hWnd, 36), LB_GETCURSEL, 0, 0), 0);
          break;
        case 31://Default Context Menu Item
          MessageBoxA(0, "You clicked the Context-Menu item", "A Message", 0);
          break;
        case 32://File > Exit
          PostQuitMessage(0);
          break;
        case 33://Button Message
          MessageBoxA(0, "You clicked the Button!", "A Message", 0);
          break;
      }
      break;
    case WM_CHAR:
    {
      if( wParam != '\r' && wParam != '\b' )
      {
      int i;
      int x = cursor_loc;
      int k = strlen( text_buffer );
      for(  i = k; i > x; --i)
      {
        text_buffer[i] = text_buffer[i-1];
      }
      
      text_buffer[x] = wParam;
      ++cursor_loc;
      ++text_buffer_length; 
      }
    }
    break;
    case WM_KEYDOWN:
    {
      if( wParam == 0x0D )//enter
      { 
        char b[8];
        int i;
        int x = cursor_loc;
        int k = strlen( text_buffer );
        for(  i = k; i > x; --i)
        {
          text_buffer[i] = text_buffer[i-1];
        }
        if( wParam == 0x0D )
        {//enter
          text_buffer[x] = '\n';
        }
        ++cursor_loc;
        ++text_buffer_length;
      }
      else if( wParam == 0x08 )//backspace
      {
        int i;
        int x = cursor_loc-1;
        int k = strlen( text_buffer );
        for( i = x; i < k; ++i )
        {
          text_buffer[i] = text_buffer[i+1];
        }
        --cursor_loc;
        if( cursor_loc < 0 ) cursor_loc = 0;
      }
      else if( wParam == 0x25 ) //LEFT
      {
        if( cursor_loc > 0 )
          --cursor_loc;
      }
      else if( wParam == 0x27 )//right
      {
        if( cursor_loc < text_buffer_length )
          ++cursor_loc;
      }
      InvalidateRect( hWnd, 0, TRUE );
    }
    break;
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint( hWnd, &ps );
      RECT r;
      RECT r2;
      int curY = 0;
      int i,j;
      int text_width;
      GetClientRect( hWnd, &r );
      text_buffer_length = strlen( text_buffer );
      for( i = 0; i < text_buffer_length; ++i )
      {
        for( j = i; i < text_buffer_length; ++j )
        {
          if( text_buffer[j] == '\n' || text_buffer[j] == '\0' )
          {
            break;
          }//break the loop when we find the line separator.
        }
        r.top = curY;
        DrawText( hdc, text_buffer+i, j - i, &r, 0);
        
        if( cursor_state & 1 && cursor_loc == i )
        {
          Rectangle( hdc, 0, r.top, 2, r.top + 20 );
        }
        else if( cursor_state & 1 && cursor_loc >= i && cursor_loc <= j )
        {
          DrawText( hdc, text_buffer+i, cursor_loc - i, &r2, DT_CALCRECT );
          text_width = r2.right - r2.left;
          if( text_width < 1 ) text_width = 1;
          Rectangle( hdc, text_width, r.top, text_width+2, r.top + 20 );
        
        }
        
        i = j;
        curY += 20;
      }
      //DrawText( hdc, text_buffer, -1, &r, 0);
      EndPaint( hWnd, &ps );
    }
    break;
    case WM_CONTEXTMENU:
      GetCursorPos(&cursor);
      hSM = CreatePopupMenu();
      if(wParam == GetDlgItem(hWnd, 36))
      {
        AppendMenu(hSM, MF_STRING, 20, "Add Item");
        AppendMenu(hSM, MF_STRING, 21, "Delete Item");
      }
      else
      {
        AppendMenu(hSM, MF_STRING, 31, "Context Menu Item");
      }
      TrackPopupMenu(hSM, TPM_LEFTALIGN, cursor.x, cursor.y, 0, hWnd, NULL);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
  MSG msg1;
  WNDCLASS wc1;
  HWND hWnd1;
  ZeroMemory(&wc1, sizeof wc1);
  wc1.hInstance = hInst;
  wc1.lpszClassName = AppName;
  wc1.lpfnWndProc = (WNDPROC)WndProc;
  wc1.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;
  wc1.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
  wc1.hIcon = LoadIcon(NULL, IDI_INFORMATION);
  wc1.hCursor = LoadCursor(NULL, IDC_ARROW);
  if(RegisterClass(&wc1) == FALSE) return 0;
  hWnd1 = CreateWindow(AppName, AppName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 10, 10, 360, 240, 0, 0, hInst, 0);
  if(hWnd1 == NULL) return 0;
  while(GetMessage(&msg1,NULL,0,0) > 0){
    TranslateMessage(&msg1);
    DispatchMessage(&msg1);
  }
  return msg1.wParam;
}
