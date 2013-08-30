
/////////////////////////////////////////////
//                                         //
// Minimizing C++ Win32 App To System Tray //
//                                         //
// You found this at bobobobo's weblog,    //
// http://bobobobo.wordpress.com           //
//                                         //
// Creation date:  Mar 30/09               //
// Last modified:  Mar 30/09               //
//                                         //
/////////////////////////////////////////////

// GIVING CREDIT WHERE CREDIT IS DUE!!
// Thanks ubergeek!  http://www.gidforums.com/t-5815.html

#pragma region include and define
#include "systray.h"
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

#ifdef UNICODE
#define stringcopy wcscpy
#else
#define stringcopy strcpy
#endif

#define ID_TRAY_APP_ICON                5000
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM  3000
#define WM_TRAYICON ( WM_USER + 1 )
#pragma endregion

#pragma region constants and globals
UINT WM_TASKBARCREATED = 0 ;

HWND g_hwnd ;
HMENU g_menu ;

NOTIFYICONDATA g_notifyIconData ;

WebServer* web = NULL;
Environment* env = NULL;
#pragma endregion


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);



#pragma region helper funcs
// These next 2 functions are the STARS of this example.
// They perform the "minimization" function and "restore"
// functions for our window.  Notice how when you "minimize"
// the app, it doesn't really "minimize" at all.  Instead,
// you simply HIDE the window, so it doesn't display, and
// at the same time, stick in a little icon in the system tray,
// so the user can still access the application.
void Minimize()
{
  // add the icon to the system tray
  Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

  // ..and hide the main window
  ShowWindow(g_hwnd, SW_HIDE);
}

// Basically bring back the window (SHOW IT again)
// and remove the little icon in the system tray.
void Restore()
{
  // Remove the icon from the system tray
  Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

  // ..and show the window
  ShowWindow(g_hwnd, SW_SHOW);
}

// Initialize the NOTIFYICONDATA structure.
// See MSDN docs http://msdn.microsoft.com/en-us/library/bb773352(VS.85).aspx
// for details on the NOTIFYICONDATA structure.
void InitNotifyIconData()
{
  memset( &g_notifyIconData, 0, sizeof( NOTIFYICONDATA ) ) ;
  
  g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
  
  /////
  // Tie the NOTIFYICONDATA struct to our
  // global HWND (that will have been initialized
  // before calling this function)
  g_notifyIconData.hWnd = g_hwnd;
  // Now GIVE the NOTIFYICON.. the thing that
  // will sit in the system tray, an ID.
  g_notifyIconData.uID = ID_TRAY_APP_ICON;
  // The COMBINATION of HWND and uID form
  // a UNIQUE identifier for EACH ITEM in the
  // system tray.  Windows knows which application
  // each icon in the system tray belongs to
  // by the HWND parameter.
  /////
  
  /////
  // Set up flags.
  g_notifyIconData.uFlags = NIF_ICON | // promise that the hIcon member WILL BE A VALID ICON!!
    NIF_MESSAGE | // when someone clicks on the system tray icon,
    // we want a WM_ type message to be sent to our WNDPROC
    NIF_TIP;      // we're gonna provide a tooltip as well, son.

  g_notifyIconData.uCallbackMessage = WM_TRAYICON; //this message must be handled in hwnd's window procedure. more info below.
  
  // Load da icon.  Be sure to include an icon "green_man.ico" .. get one
  // from the internet if you don't have an icon
  g_notifyIconData.hIcon = (HICON)LoadImage( NULL, TEXT("green_man.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE  ) ;

  // set the tooltip text.  must be LESS THAN 64 chars
  stringcopy(g_notifyIconData.szTip, TEXT("Green man.. here's looking at ya!"));
}
#pragma endregion

int StartSystray(WebServer* _web, Environment* _env)
{
    web = _web;
    env = _env;
  TCHAR className[] = TEXT( "tray icon class" );

  // I want to be notified when windows explorer
  // crashes and re-launches the taskbar.  the WM_TASKBARCREATED
  // event will be sent to my WndProc() AUTOMATICALLY whenever
  // explorer.exe starts up and fires up the taskbar again.
  // So its great, because now, even if explorer crashes,
  // I have a way to re-add my system tray icon in case
  // the app is already in the "minimized" (hidden) state.
  // if we did not do this an explorer crashed, the application
  // would remain inaccessible!!
  WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated") ;

  #pragma region add a console
  // add a console, because I love consoles.
  // To disconnect the console, just comment out
  // the next 3 lines of code.
  //// AllocConsole();
  //// AttachConsole( GetCurrentProcessId() ) ;
  //// freopen( "CON", "w", stdout ) ;
  #pragma endregion
  
  #pragma region get window up
  WNDCLASSEX wnd = { 0 };

  HINSTANCE hInstance = GetModuleHandle(NULL);
  wnd.hInstance = hInstance;
  wnd.lpszClassName = className;
  wnd.lpfnWndProc = WndProc;
  wnd.style = CS_HREDRAW | CS_VREDRAW ;
  wnd.cbSize = sizeof (WNDCLASSEX);

  wnd.hIcon = LoadIcon (NULL, IDI_APPLICATION);
  wnd.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
  wnd.hCursor = LoadCursor (NULL, IDC_ARROW);
  wnd.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE ;
  
  if (!RegisterClassEx(&wnd))
  {
    FatalAppExit( 0, TEXT("Couldn't register window class!") );
  }
  
  g_hwnd = CreateWindowEx (
    
    0, className,
    
    TEXT( "Using the system tray" ),
    WS_OVERLAPPEDWINDOW,
    
    CW_USEDEFAULT, CW_USEDEFAULT, 
    400, 400, 

    NULL, NULL, 
    hInstance, NULL
  );

  // Initialize the NOTIFYICONDATA structure once
  InitNotifyIconData();


  ShowWindow (g_hwnd, SW_SHOWDEFAULT);
  #pragma endregion

  MSG msg ;
  while (GetMessage (&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if( !IsWindowVisible( g_hwnd ) )
  {
    Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
  }

  return msg.wParam;
}
#define IDC_MAIN_START 101
#define IDC_MAIN_STOP 102
#define IDC_MAIN_EXIT 103

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if ( message==WM_TASKBARCREATED && !IsWindowVisible( g_hwnd ) )
  {
    Minimize();
    return 0;
  }

  switch (message)
  {
  case WM_CREATE:
    {
    g_menu = CreatePopupMenu();

    AppendMenu(g_menu, MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM,  TEXT( "Exit" ) );
    HWND hWndstop=CreateWindowEx(NULL,
            "BUTTON",
            "Stop",
            WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
            10,
            10,
            100,
            24,
            hwnd,
            (HMENU)IDC_MAIN_STOP,
            GetModuleHandle(NULL),
            NULL);
    ShowWindow(hWndstop, SW_SHOWDEFAULT);
    HWND hWndStart=CreateWindowEx(NULL,
            "BUTTON",
            "Start",
            WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
            10,
            40,
            100,
            24,
            hwnd,
            (HMENU)IDC_MAIN_START,
            GetModuleHandle(NULL),
            NULL);
    ShowWindow(hWndStart, SW_SHOWDEFAULT);
    HWND hWndExit=CreateWindowEx(NULL,
            "BUTTON",
            "Exit",
            WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
            10,
            70,
            100,
            24,
            hwnd,
            (HMENU)IDC_MAIN_EXIT,
            GetModuleHandle(NULL),
            NULL);
    ShowWindow(hWndExit, SW_SHOWDEFAULT);
    break;
  }

  case WM_COMMAND:
  {
      switch(LOWORD(wParam))
      {
          case IDC_MAIN_EXIT:
          {
          if (web != NULL)
              web->Stop(*env);
            PostQuitMessage( 0 ) ;
          }
          break;
      }
      break;
  }
  case WM_SYSCOMMAND:
  {
    switch( wParam & 0xfff0 )  // (filter out reserved lower 4 bits:  see msdn remarks http://msdn.microsoft.com/en-us/library/ms646360(VS.85).aspx)
    {
    case SC_MINIMIZE:
    case SC_CLOSE:  // redundant to WM_CLOSE, it appears
      Minimize() ; 
      return 0 ;
      break;
    }
    break;
  }

  case WM_TRAYICON:
    {
      switch(wParam)
      {
      case ID_TRAY_APP_ICON:
        printf( "Its the ID_TRAY_APP_ICON.. one app can have several tray icons, ya know..\n" ) ;
        break;
      }
      if (lParam == WM_RBUTTONDOWN)
      {
        POINT curPoint ;
        GetCursorPos( &curPoint ) ;
        SetForegroundWindow(hwnd); 

        UINT clicked = TrackPopupMenu(
          g_menu,
          TPM_RETURNCMD | TPM_NONOTIFY,
          curPoint.x,
          curPoint.y,
          0,
          hwnd,
          NULL
        );

        if (clicked == ID_TRAY_EXIT_CONTEXT_MENU_ITEM)
        {
            if (web != NULL)
                web->Stop(*env);
          PostQuitMessage(0);
        }
      }
    }
    break;

  case WM_NCHITTEST:
  {
    // http://www.catch22.net/tuts/tips
    // this tests if you're on the non client area hit test
    UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
    if(uHitTest == HTCLIENT)
      return HTCAPTION;
    else
      return uHitTest;
  }

  case WM_CLOSE:
    Minimize() ;
    return 0;
    break;

  case WM_DESTROY:
    PostQuitMessage (0);
    break;

  }

  return DefWindowProc( hwnd, message, wParam, lParam ) ;
}
