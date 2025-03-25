#define UNICODE
#include <windows.h>

#define ID_BTN1 101
#define ID_BTN2 102
#define ID_BTN3 103
#define ID_BTN4 104
#define ID_BTN5 105

// Función para centrar la ventana en pantalla
void CentrarVentana(HWND hwnd) {
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

// Función para ejecutar un programa y cerrar el menú
void EjecutarYSalir(LPCWSTR programa, HWND hwnd) {
    ShellExecuteW(NULL, L"open", programa, NULL, NULL, SW_SHOWNORMAL);
    PostMessage(hwnd, WM_CLOSE, 0, 0); // Cierra el menú
}

// Función de ventana principal
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            int btnWidth = 250;
            int btnHeight = 50;
            int xPos = 125;

            CreateWindow(L"BUTTON", L"Agregar libro", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         xPos, 50, btnWidth, btnHeight, hwnd, (HMENU)ID_BTN1, nullptr, nullptr);
            CreateWindow(L"BUTTON", L"Buscar libro", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         xPos, 120, btnWidth, btnHeight, hwnd, (HMENU)ID_BTN2, nullptr, nullptr);
            CreateWindow(L"BUTTON", L"Prestar libro", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         xPos, 190, btnWidth, btnHeight, hwnd, (HMENU)ID_BTN3, nullptr, nullptr);
            CreateWindow(L"BUTTON", L"Devolver libro", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         xPos, 260, btnWidth, btnHeight, hwnd, (HMENU)ID_BTN4, nullptr, nullptr);
            CreateWindow(L"BUTTON", L"Listar todos los libros", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                         xPos, 330, btnWidth, btnHeight, hwnd, (HMENU)ID_BTN5, nullptr, nullptr);
        } break;

        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH hBrush = CreateSolidBrush(RGB(139, 69, 19));
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            return 1;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            switch (wmId) {
                case ID_BTN1:
                    EjecutarYSalir(L"agregarlibro.exe", hwnd);
                    break;
                case ID_BTN2:
                    EjecutarYSalir(L"buscarlibro.exe", hwnd);
                    break;
                case ID_BTN3:
                    EjecutarYSalir(L"prestarlibro.exe", hwnd);
                    break;
                case ID_BTN4:
                    EjecutarYSalir(L"devolverlibro.exe", hwnd);
                    break;
                case ID_BTN5:
                    EjecutarYSalir(L"listarlibros.exe", hwnd);
                    break;
            }
        } break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MenuModernoWinAPI";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Gestión de Biblioteca",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwnd) return 0;

    // Centrar la ventana
    ShowWindow(hwnd, nCmdShow);
    CentrarVentana(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
