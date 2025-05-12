
#include <windows.h>
#include "agregarlibro.h"
#include "buscarlibro.h"
#include "prestarlibro.h"
#include "devolverlibro.h"
#include "listarlibros.h"
#include "menu.h"

#define ID_BTN1 101
#define ID_BTN2 102
#define ID_BTN3 103
#define ID_BTN4 104
#define ID_BTN5 105

HWND hwndMenu;

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
                case ID_BTN1: agregarLibro(hwnd); break;
                case ID_BTN2: buscarLibro(hwnd); break;
                case ID_BTN3: prestarLibro(hwnd); break;
                case ID_BTN4: devolverLibro(hwnd); break;
                case ID_BTN5: listarLibros(hwnd); break;
            }
        } break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ESTA es la función esperada desde main.cpp
void IniciarMenu(HINSTANCE hInstance, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"MenuModernoWinAPI";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    RegisterClass(&wc);

    hwndMenu = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Gestión de Biblioteca",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 500,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hwndMenu) return;

    ShowWindow(hwndMenu, nCmdShow);
    CentrarVentana(hwndMenu);

    // Loop principal va aquí, porque IniciarMenu lo maneja todo
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void RegresarAlMenu(HWND hwndParent) {
    DestroyWindow(hwndParent);
}
