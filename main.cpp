// Ali Haghour 
// 04-04-2025
// PID controller Windows GUI

#include <windows.h>
#include <commctrl.h>
#include <string>
#include "PIDController.h"

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

// After simulation completes and saves the CSV file
void plotResultsWithGnuplot(const char* filename) {
	// GNUPLOT command to plot the results
    char gnuplotCommand[512];
    sprintf_s(gnuplotCommand,
        "gnuplot -e \"set datafile separator ','; "
        "plot '%s' using 1:3 with lines title 'Position/Speed'; "
        "pause -1\"",
        filename);

    // Execute the command using system()
    system(gnuplotCommand);
}

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
    case WM_CREATE:
    {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_WIN95_CLASSES;
        InitCommonControlsEx(&icex);

        RECT rcClient;
        GetClientRect(hwnd, &rcClient);
        int windowWidth = rcClient.right - rcClient.left;

        const int LABEL_HEIGHT = 20;
        const int EDIT_HEIGHT = 25;
        const int CONTROL_WIDTH = 80;
        const int CONTROL_SPACING = 20;
        const int CONTROLS_TOP = 30;

        int totalWidth = (CONTROL_WIDTH * 3) + (CONTROL_SPACING * 2);
        int startX = (windowWidth - totalWidth) / 2;

        CreateWindow(
            L"STATIC",
            L"Kp",
            WS_VISIBLE | WS_CHILD,
            startX, CONTROLS_TOP,
            CONTROL_WIDTH, LABEL_HEIGHT,
            hwnd, NULL, NULL, NULL);

        g_hwndKpEdit = CreateWindow(
            L"EDIT",
            DEFAULT_KP,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            startX, CONTROLS_TOP + LABEL_HEIGHT,
            CONTROL_WIDTH, EDIT_HEIGHT,
            hwnd, (HMENU)ID_KP_EDIT, NULL, NULL);

        CreateWindow(
            L"STATIC",
            L"Ki",
            WS_VISIBLE | WS_CHILD,
            startX + CONTROL_WIDTH + CONTROL_SPACING, CONTROLS_TOP,
            CONTROL_WIDTH, LABEL_HEIGHT,
            hwnd, NULL, NULL, NULL);

        g_hwndKiEdit = CreateWindow(
            L"EDIT",
            DEFAULT_KI,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            startX + CONTROL_WIDTH + CONTROL_SPACING, CONTROLS_TOP + LABEL_HEIGHT,
            CONTROL_WIDTH, EDIT_HEIGHT,
            hwnd, (HMENU)ID_KI_EDIT, NULL, NULL);

        CreateWindow(
            L"STATIC",
            L"Kd",
            WS_VISIBLE | WS_CHILD,
            startX + (CONTROL_WIDTH + CONTROL_SPACING) * 2, CONTROLS_TOP,
            CONTROL_WIDTH, LABEL_HEIGHT,
            hwnd, NULL, NULL, NULL);

        g_hwndKdEdit = CreateWindow(
            L"EDIT",
            DEFAULT_KD,
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            startX + (CONTROL_WIDTH + CONTROL_SPACING) * 2, CONTROLS_TOP + LABEL_HEIGHT,
            CONTROL_WIDTH, EDIT_HEIGHT,
            hwnd, (HMENU)ID_KD_EDIT, NULL, NULL);

        g_hwndRunButton = CreateWindow(
            L"BUTTON",
            L"Run Simulation",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            startX, CONTROLS_TOP + LABEL_HEIGHT + EDIT_HEIGHT + 20,
            totalWidth, 30,
            hwnd, (HMENU)ID_RUN_BUTTON, NULL, NULL);
    }
    return 0;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case ID_RUN_BUTTON:
            // When Run button is clicked
            if (HIWORD(wParam) == BN_CLICKED) 
            {
                double kp, ki, kd;
                GetPIDValues(kp, ki, kd);

                WCHAR message[256];
                swprintf_s(message, L"PID Values:\nKp: %.2f\nKi: %.2f\nKd: %.2f", kp, ki, kd);
                MessageBox(hwnd, message, L"PID Parameters", MB_OK | MB_ICONINFORMATION);

                // Define default values for all other parameters
                double setpoint = 1.0;               // Target position/speed
                double simulation_time = 5.0;        // 5 seconds simulation
                double dt = 0.0001;                  // Time step

                // Motor parameters (using default preset)
                double r = 2.0;                      // Resistance (ohms)
                double l = 0.001;                    // Inductance (henry)
                double kt = 0.01;                    // Torque constant (N·m/A)
                double ke = 0.01;                    // Back-EMF constant (V·s/rad)
                double j = 0.0001;                   // Inertia (kg·m²)
                double f = 0.0001;                   // Friction (N·m·s/rad)
                bool position_control = true;        // Position control mode
                bool add_disturbance = true;         // Add disturbance at 50% time

                // Generate a filename with timestamp (to avoid overwriting)
                char filename[100];
                SYSTEMTIME st;
                GetLocalTime(&st);
                sprintf_s(filename, "pid_sim_%d%02d%02d_%02d%02d%02d.csv",
                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

                // Run simulation with inputted PID values
                std::vector<std::vector<double>> data = runMotorSimulation(
                    kp, ki, kd, setpoint, simulation_time, dt,
                    r, l, kt, ke, j, f, position_control, add_disturbance);

                // Save results to CSV
                saveToCSV(data, filename);

                // Show completion message with filename
                swprintf_s(message, L"Simulation complete!\nResults saved to: %hs", filename);
                MessageBox(hwnd, message, L"Simulation Finished", MB_OK | MB_ICONINFORMATION);

                // Automatically plot the results
				plotResultsWithGnuplot(filename);

                // NOTE:
				// Uncommenting the following line would also open the CSV file with the default application 
                // Not needed for now
				// ShellExecute(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL);     
            }
        break;
        }
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        // Draw a title for the application
        RECT rcTitle;
        GetClientRect(hwnd, &rcTitle);
        rcTitle.top = 5;
        rcTitle.bottom = 25;

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        DrawText(hdc, L"DC Motor PID Controller", -1, &rcTitle, DT_CENTER);

        EndPaint(hwnd, &ps);
    }
    return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Entry point for a Windows application
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    const WCHAR CLASS_NAME[] = L"PID Controller Window";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DC Motor PID Controller",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
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
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
