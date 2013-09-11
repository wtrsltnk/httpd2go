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

UINT WM_TASKBARCREATED = 0 ;

HWND g_hwnd ;
HMENU g_menu ;

NOTIFYICONDATA g_notifyIconData ;

WebServer* web = NULL;
Environment* env = NULL;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

void Minimize()
{
  Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);
  ShowWindow(g_hwnd, SW_HIDE);
}

void Restore()
{
    Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
    ShowWindow(g_hwnd, SW_SHOW);
}

void InitNotifyIconData()
{
    memset( &g_notifyIconData, 0, sizeof( NOTIFYICONDATA ) ) ;

    g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    g_notifyIconData.hWnd = g_hwnd;
    g_notifyIconData.uID = ID_TRAY_APP_ICON;
    g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_notifyIconData.uCallbackMessage = WM_TRAYICON;
    g_notifyIconData.hIcon = (HICON)LoadImage( NULL, TEXT("green_man.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE  ) ;

    stringcopy(g_notifyIconData.szTip, TEXT("Green man.. here's looking at ya!"));
}

int StartSystray(WebServer* _web, Environment* _env)
{
    web = _web;
    env = _env;
    TCHAR className[] = TEXT( "tray icon class" );
    WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated") ;

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

            HWND hWndstop = CreateWindowEx(NULL,
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

            HWND hWndStart = CreateWindowEx(NULL,
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

            HWND hWndExit = CreateWindowEx(NULL,
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
                    break;
                }
                case IDC_MAIN_START:
                {
                    if (web != NULL)
                        web->Start(*env);
                    break;
                }
                case IDC_MAIN_STOP:
                {
                    if (web != NULL)
                        web->Stop(*env);
                    break;
                }
            }
            break;
        }
        case WM_SYSCOMMAND:
        {
            switch( wParam & 0xfff0 )
            {
                case SC_MINIMIZE:
                case SC_CLOSE:
                {
                    Minimize() ;
                    return 0 ;
                }
            }
            break;
        }

        case WM_TRAYICON:
        {
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
            else if (lParam == WM_LBUTTONDOWN)
            {
                Restore();
            }
        }
        break;

        case WM_NCHITTEST:
        {
            UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
            if(uHitTest == HTCLIENT)
                return HTCAPTION;
            else
                return uHitTest;
        }

        case WM_CLOSE:
        {
            Minimize();
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage (0);
            break;
        }

    }

    return DefWindowProc( hwnd, message, wParam, lParam ) ;
}
