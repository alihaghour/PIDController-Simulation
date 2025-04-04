// Ali Haghour 
// 04-04-2025
// PID controller Windows GUI

#include <windows.h>
#include <commctrl.h>
#include <string>

// Global handles for the edit controls
HWND g_hwndKpEdit = NULL;
HWND g_hwndKiEdit = NULL;
HWND g_hwndKdEdit = NULL;
HWND g_hwndRunButton = NULL;

// Default values for PID parameters
const WCHAR DEFAULT_KP[] = L"1.0";
const WCHAR DEFAULT_KI[] = L"0.1";
const WCHAR DEFAULT_KD[] = L"0.05";

// ID values for controls
#define ID_KP_EDIT 101
#define ID_KI_EDIT 102
#define ID_KD_EDIT 103
#define ID_RUN_BUTTON 104

// Function to get PID values from edit controls
void GetPIDValues(double& kp, double& ki, double& kd) {
    WCHAR buffer[256];

    GetWindowText(g_hwndKpEdit, buffer, 256);
    kp = _wtof(buffer);

    GetWindowText(g_hwndKiEdit, buffer, 256);
    ki = _wtof(buffer);

    GetWindowText(g_hwndKdEdit, buffer, 256);
    kd = _wtof(buffer);
}

// Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    switch (uMsg) 
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT: 
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }

    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Entry point for a Windows application
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    const WCHAR CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Learn to Program Windows",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 480, 720,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) 
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}