// pingpong.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "pingpong.h"
#include <string>
#include <Windows.h>
#include <commdlg.h>
#include <vector>
struct TrailCircle {
    int x, y; // position
    int radius; // radius
    COLORREF color; // color
};
#define MAX_LOADSTRING 100
#define ID_PADDLE 10
#define ID_BALL 5
#define ID_TIMERBALL 1
#define ID_MAIN 2
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#define WM_UPDATE_COUNTER1 WM_USER + 1
#define WM_UPDATE_COUNTER2 WM_USER + 2
POINT paddlePos = { 0, 0 };
HBITMAP hBitmap1;
const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 350;
const int BALL_RADIUS = 10;
const int PADDLE_WIDTH = 20;
const int PADDLE_HEIGHT = 60;
bool strech = false;
bool tile = false;
HBITMAP hBitmap;
void resetGame(HWND);
int scoreleft = 0;
int scoreright = 0;
std::vector<TrailCircle> trail;
UINT_PTR ballTimer;
int BALL_POS_X = 0;
int BALL_POS_Y = 0;
int BALL_SPEED_X = 5;
int BALL_SPEED_Y = 5;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProcMainWindow(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProcMainBall(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProcMainPaddle(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Color(HWND, UINT, WPARAM, LPARAM);
HWND CreateChildWindowBall(HWND);
HWND CreateChildWindowPaddle(HWND);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PINGPONG, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PINGPONG));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    ///MainWindow
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProcMainWindow;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PINGPONG));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(144, 238, 144));//setting light green colour
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PINGPONG);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    ///register ball
    RegisterClassExW(&wcex);
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProcMainBall;
    wc.lpszClassName = L"Ball";
    RegisterClass(&wc);
    ///register paddle
    WNDCLASS wcp = { 0 };
    wcp.lpfnWndProc = WndProcMainPaddle;
    wcp.lpszClassName = L"Paddle";
    RegisterClass(&wcp);
    return TRUE;

}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   int screenWidth = GetSystemMetrics(SM_CXSCREEN);
   int screenHeight = GetSystemMetrics(SM_CYSCREEN);
   //int windowWidth = 500;
   //int windowHeight = 350;
   int windowX = (screenWidth - WINDOW_WIDTH) / 2;
   int windowY = (screenHeight - WINDOW_HEIGHT) / 2;
   HWND MainWindow = CreateWindowW(
       szWindowClass,
       szTitle,
       WS_POPUP | WS_CAPTION | WS_SYSMENU,
       windowX,
       windowY,
       WINDOW_WIDTH,
       WINDOW_HEIGHT,
       NULL,
       NULL,
       hInstance,
       NULL
   );
   
   ///setting  transparency to 20%
  // SetLayeredWindowAttributes(MainWindow, 0, 255 * 80 / 100, LWA_ALPHA);
   ///setting at the centre of the screen
   int cxScreen = GetSystemMetrics(SM_CXSCREEN);
   int cyScreen = GetSystemMetrics(SM_CYSCREEN);
   RECT rect;
   GetWindowRect(MainWindow, &rect);
   int x = (cxScreen - (rect.right - rect.left)) / 2;
   int y = (cyScreen - (rect.bottom - rect.top)) / 2;
   SetWindowPos(MainWindow, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
   HWND Ball= CreateChildWindowBall(MainWindow);
   HWND Paddle = CreateChildWindowPaddle(MainWindow);
   if (!MainWindow || !Ball  || !Paddle)
   {
      return FALSE;
   }
    ShowWindow(MainWindow, nCmdShow);
    UpdateWindow(MainWindow);
    return TRUE;
}
LRESULT CALLBACK WndProcMainWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        ///transparnecy
        SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, 0, (255 * 80) / 100, LWA_ALPHA);
        SetTimer(hWnd, ID_MAIN, 200, NULL);
        break;

    }
    case WM_ERASEBKGND:
    {
        // Get the background brush and paint the background with it
        HBRUSH hbrBackground = (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND);
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect((HDC)wParam, &rc, hbrBackground);
        return TRUE;
    }
    case WM_GETMINMAXINFO:
    {
        ///setting that window has constant size
        MINMAXINFO* MinMax = reinterpret_cast<MINMAXINFO*>(lParam);
        MinMax->ptMinTrackSize.x = 500;  // Minimum width
        MinMax->ptMinTrackSize.y = 350;  // Minimum height
        MinMax->ptMaxTrackSize.x = 500;  // Maximum width
        MinMax->ptMaxTrackSize.y = 350;  // Maximum height
        return 0;
    }
    case WM_MOUSEMOVE:
        // Move paddle along the right edge of the client window based on mouse position
        paddlePos.y = GET_Y_LPARAM(lParam) - PADDLE_HEIGHT / 2;
        if (GetDlgItem(hWnd, ID_PADDLE) != NULL) {
            RECT clientRect, paddleRect;
            GetClientRect(hWnd, &clientRect);
            GetWindowRect(GetDlgItem(hWnd, ID_PADDLE), &paddleRect);
            paddlePos.y = max(paddlePos.y, clientRect.top);
            paddlePos.y = min(paddlePos.y, clientRect.bottom - (paddleRect.bottom - paddleRect.top));
            SetWindowPos(GetDlgItem(hWnd, ID_PADDLE), NULL, clientRect.right - PADDLE_WIDTH, paddlePos.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        break;
        
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_COLOR:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_COLOR), hWnd, Color);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDM_BITMAP: {
                ///loading bitmap
                OPENFILENAME ofn;
                TCHAR File[MAX_PATH] = { 0 };
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFile = File;
               ofn.lpstrFilter = L"Bitmap files (*.bmp)\0*.bmp\0All files (*.*)\0*.*\0";
                ofn.nFilterIndex = 1;
               ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                if (GetOpenFileName(&ofn))
                {
                    hBitmap = (HBITMAP)LoadImageW(GetModuleHandle(NULL), File, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                    if (hBitmap) {
                        BITMAP bmp;
                        GetObject(hBitmap, sizeof(BITMAP), &bmp);
                        RECT clientRect;
                        GetClientRect(hWnd, &clientRect);
                        EnableMenuItem(GetMenu(hWnd), IDM_STRECH, MF_BYCOMMAND | MF_ENABLED);
                        EnableMenuItem(GetMenu(hWnd), IDM_TILE, MF_BYCOMMAND | MF_ENABLED);
                       // CheckMenuItem(GetMenu(hWnd), IDM_TILE, MF_BYCOMMAND | MF_CHECKED);
                        SendMessage(hWnd, WM_COMMAND, IDM_TILE, 0);
                    }
                   
                }
                break;
            }
            case IDM_STRECH: 
            {
                strech = true;
                tile = false;
                CheckMenuItem(GetMenu(hWnd), IDM_STRECH, MF_BYCOMMAND | MF_CHECKED);
                CheckMenuItem(GetMenu(hWnd), IDM_TILE, MF_BYCOMMAND | MF_UNCHECKED);
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            }
            case IDM_TILE:
            {
                strech = false;
                tile = true;
                CheckMenuItem(GetMenu(hWnd), IDM_TILE, MF_BYCOMMAND | MF_CHECKED);
                CheckMenuItem(GetMenu(hWnd), IDM_STRECH, MF_BYCOMMAND | MF_UNCHECKED);
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            }
                break;
             case IDM_NEWGAME:
                resetGame(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (hBitmap != NULL) {
                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);
                BITMAP bmp;
                GetObject(hBitmap, sizeof(BITMAP), &bmp);
                int windowWidth = 500;
                int windowHeight = 350;
                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                if (strech) {

                    int stretchedWidth = windowWidth;
                    int stretchedHeight = windowHeight;
                    if (bmp.bmWidth < windowWidth || bmp.bmHeight < windowHeight) {
                        float widthRatio = (float)windowWidth / bmp.bmWidth;
                        float heightRatio = (float)windowHeight / bmp.bmHeight;
                        float ratio = max(widthRatio, heightRatio);
                        stretchedWidth = bmp.bmWidth * ratio;
                        stretchedHeight = bmp.bmHeight * ratio;
                    }

                    SetStretchBltMode(hdc, COLORONCOLOR);
                   
                    StretchBlt(hdc, (windowWidth - stretchedWidth) / 2, (windowHeight - stretchedHeight) / 2,
                        stretchedWidth, stretchedHeight, memDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
                }
                if (tile) {
                    int repeatX = (clientRect.right + bmp.bmWidth - 1) / bmp.bmWidth;
                    int repeatY = (clientRect.bottom + bmp.bmHeight - 1) / bmp.bmHeight;

                    for (int y = 0; y < repeatY; ++y) {
                        for (int x = 0; x < repeatX; ++x) {
                            BitBlt(hdc, x * bmp.bmWidth, y * bmp.bmHeight, bmp.bmWidth, bmp.bmHeight, memDC, 0, 0, SRCCOPY);
                         }
                     }
                 }
                SelectObject(memDC, oldBitmap);
                DeleteDC(memDC);
            }
            ///counters
            TCHAR left[64], right[64];
            RECT rc;
            GetClientRect(hWnd, &rc);
            _stprintf_s(right, 64, _T("%d \0"), scoreright);
            _stprintf_s(left, 64, _T("%d \0"), scoreleft);
            int quarterWidth = rc.right / 4;
            int threeQuarterWidth = rc.right * 3 / 4;
            SetBkMode(hdc, TRANSPARENT);
            HBRUSH hbrBackground = (HBRUSH)GetClassLongPtr(hWnd, GCLP_HBRBACKGROUND);
            LOGBRUSH logBrush;
            GetObject(hbrBackground, sizeof(logBrush), &logBrush);
            COLORREF color = logBrush.lbColor;
            SetTextColor(hdc, RGB(255 - static_cast<int>(GetRValue(color)), 255 - static_cast<int>(GetGValue(color)), 255 - static_cast<int>(GetBValue(color))));
            HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
            LOGFONT lf = {};
            lf.lfHeight = 50;
            lf.lfWeight = FW_NORMAL;
            _tcscpy_s(lf.lfFaceName, _T("Arial"));
            HFONT hNewFont = CreateFontIndirect(&lf);
            HFONT hOldFont = (HFONT)SelectObject(hdc, hNewFont);
            TextOut(hdc, quarterWidth, 0, left, (int)_tcsclen(left));
            TextOut(hdc, threeQuarterWidth, 0, right, (int)_tcsclen(right));
            ///trail
            for (size_t i = 0; i < trail.size(); i++) {
                HBRUSH hBrush = CreateSolidBrush(trail[i].color);
                HPEN hPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
                Ellipse(hdc, trail[i].x - trail[i].radius, trail[i].y - trail[i].radius,
                    trail[i].x + trail[i].radius, trail[i].y + trail[i].radius);
                SelectObject(hdc, hOldPen);
                DeleteObject(hPen);
                SelectObject(hdc, hOldBrush);
                DeleteObject(hBrush);
            }
            SelectObject(hdc, hOldFont);
            DeleteObject(hNewFont);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SYSKEYDOWN:
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
       
        }
     break;
    case WM_TIMER:
    {
        RECT rc;
         GetWindowRect(GetDlgItem(hWnd, ID_BALL), &rc);
        TrailCircle circle; 
        POINT pt;
        pt.x = rc.left + (rc.right - rc.left) / 2;
        pt.y = rc.top + (rc.bottom - rc.top) / 2;
        ScreenToClient(hWnd, &pt);
        circle.x = pt.x;
        circle.y = pt.y;
        circle.radius = BALL_RADIUS;
        circle.color = RGB(255, 0, 0);
        trail.push_back(circle);
        for (size_t i = 0; i < trail.size(); i++) {
            trail[i].radius -= 2;
        }
        while (trail.size() > 5) {
            trail.erase(trail.begin());
        }
        BOOL XD = InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK WndProcMainBall(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
      SetTimer(hWnd, ID_TIMERBALL, 50, NULL);
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0)); // red color
        HRGN hRgn = CreateEllipticRgn(0, 0, 2*BALL_RADIUS, 2 * BALL_RADIUS);
        SetWindowRgn(hWnd, hRgn, TRUE);
        int regionType = GetWindowRgn(hWnd, hRgn);
        SelectClipRgn(hdc, hRgn);
        FillRect(hdc, &ps.rcPaint, brush);
        SelectClipRgn(hdc, NULL);
        DeleteObject(brush);
        DeleteObject(hRgn);

        EndPaint(hWnd, &ps);
        
    }
    break;
    case WM_TIMER:
    {
        if (wParam == 1)
        {
            BALL_POS_X += BALL_SPEED_X;
            BALL_POS_Y += BALL_SPEED_Y;
            RECT rect;
            HWND hPaddle = GetDlgItem(GetParent(hWnd), ID_PADDLE);
            RECT paddleRect;
            GetWindowRect(hPaddle, &paddleRect);
            POINT pt = { paddleRect.left, paddleRect.top };
            ScreenToClient(GetParent(hPaddle), &pt);
            paddleRect.left = pt.x;
            paddleRect.top = pt.y;
            pt.x = paddleRect.right;
            pt.y = paddleRect.bottom;
            ScreenToClient(GetParent(hPaddle), &pt);
            paddleRect.right = pt.x;
            paddleRect.bottom = pt.y;

            ///bounce off left edge
            GetClientRect(GetParent(hWnd), &rect);
            if (BALL_POS_X - BALL_RADIUS < rect.left)
            {
                BALL_SPEED_X = -BALL_SPEED_X;
                BALL_POS_X = rect.left + BALL_RADIUS;
            }
            ///bounce off paddle
            if ((BALL_POS_X + BALL_RADIUS > paddleRect.left) && ((BALL_POS_Y + BALL_RADIUS >= paddleRect.top && BALL_POS_Y + BALL_RADIUS<=paddleRect.bottom) || (BALL_POS_Y - BALL_RADIUS >= paddleRect.top && BALL_POS_Y - BALL_RADIUS<=paddleRect.bottom)) && BALL_SPEED_X>0) {
               scoreright++;
                BALL_SPEED_X = -BALL_SPEED_X;
                BALL_POS_X = paddleRect.left - BALL_RADIUS;
            }
            ///bounce off right edge
            if (BALL_POS_X + BALL_RADIUS > rect.right) {
                scoreleft++;
                BALL_SPEED_X = -BALL_SPEED_X;
                BALL_POS_X = rect.right - BALL_RADIUS;
            }
            // Bounce off top edge
            if (BALL_POS_Y - BALL_RADIUS < rect.top)
            {
                BALL_SPEED_Y = -BALL_SPEED_Y;
                BALL_POS_Y = rect.top + BALL_RADIUS;
            }

            // Bounce off bottom edge
            if (BALL_POS_Y + BALL_RADIUS > rect.bottom)
            {
                BALL_SPEED_Y = -BALL_SPEED_Y;
                BALL_POS_Y = rect.bottom - BALL_RADIUS;
            }
            SetWindowPos(hWnd, nullptr, BALL_POS_X - BALL_RADIUS, BALL_POS_Y - BALL_RADIUS, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            InvalidateRect(hWnd, nullptr, FALSE);
            UpdateWindow(hWnd);
        }
        break;
    
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK WndProcMainPaddle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
    static bool isDragging = false; // Flag to track if the paddle is currently being dragged
    static POINT prevPos; // Previous position of the mouse cursor
    switch (message)
    {
    case WM_CREATE:
    {
        // Set the window color to the active title bar color
        HBRUSH brush = CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
        SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CTLCOLORSTATIC:
        if ((HWND)lParam == hWnd)
        {
            // Set the background color of the child window to the active title bar color.
           /* HDC hdc = (HDC)wParam;
            SetBkColor(hdc, GetSysColor(COLOR_ACTIVECAPTION));
            return (LRESULT)GetStockObject(NULL_BRUSH);*/
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
// Message handler for about box.

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
INT_PTR CALLBACK Color(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        CHOOSECOLOR cc;
        static COLORREF acrCustClr[16];
        ZeroMemory(&cc, sizeof(cc));
        cc.lStructSize = sizeof(cc);
        cc.hwndOwner = hDlg;
        cc.lpCustColors = acrCustClr;
        cc.rgbResult = RGB(255, 255, 255);
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;
        if (ChooseColor(&cc))
        {
           
            HBRUSH hBrush = CreateSolidBrush(cc.rgbResult);
            SetClassLongPtr(GetParent(hDlg), GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
            InvalidateRect(GetParent(hDlg), NULL, TRUE);
            EndDialog(hDlg, IDOK);
            hBitmap = NULL;
            EnableMenuItem(GetMenu(GetParent(hDlg)), IDM_STRECH, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
            EnableMenuItem(GetMenu(GetParent(hDlg)), IDM_TILE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
            //CheckMenuItem(GetMenu(GetParent(hDlg)), IDM_TILE, MF_BYCOMMAND | MF_CHECKED);
            //CheckMenuItem(GetMenu(GetParent(hDlg)), IDM_STRECH, MF_BYCOMMAND | MF_UNCHECKED);
        }
        else {
            EndDialog(hDlg, IDCANCEL);
        }
        break;
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hDlg, IDOK);
            break;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            break;
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

HWND CreateChildWindowBall(HWND hwndParent)
{
    int x = 0;
    int y = 0;
    int width = 2*BALL_RADIUS;
    int height = 2*BALL_RADIUS;

    // Create the child window
    HWND hwnd = CreateWindowEx(
        0,                    
        L"Ball",       
        NULL,                  
        WS_CHILD | WS_VISIBLE, 
        x, y, width, height,   
        hwndParent,            
        (HMENU)ID_BALL,                  
        NULL,                  
        NULL                   
    );

    return hwnd;
}
HWND CreateChildWindowPaddle(HWND hwndParent)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProcMainPaddle;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"Paddle";
    RegisterClass(&wc);
    RECT rect;
    GetClientRect(hwndParent, &rect);

    HWND hWnd = CreateWindowEx(
        0,                     
        L"Paddle",       
        NULL,                  
        WS_CHILD | WS_VISIBLE, 
        rect.right-PADDLE_WIDTH, 0, 
        PADDLE_WIDTH, PADDLE_HEIGHT,   
        hwndParent,            
        (HMENU)ID_PADDLE,                  
        GetModuleHandle(NULL), 
        NULL                   
    );
    return hWnd;
}
void resetGame(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    BALL_POS_X = 0;
    BALL_POS_Y = 0;
    scoreright = 0;
    scoreleft = 0;
    SetWindowPos(GetDlgItem(hWnd, ID_BALL), NULL, 0, 0 ,0, 0, SWP_NOSIZE | SWP_NOZORDER);
    InvalidateRect(hWnd, NULL, TRUE);
}

