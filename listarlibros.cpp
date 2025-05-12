// listarlibros.cpp
#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <vector>
#include "listarlibros.h"
#include "menu.h" // Para la declaración de RegresarAlMenu

#define ID_LISTA 1
#define ID_BTN_REGRESAR 2

HWND hListado;
std::vector<std::wstring> listaDeLibros;

LRESULT CALLBACK ListarLibrosProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            RECT rc;
            GetClientRect(hwnd, &rc);

            hListado = CreateWindow(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
                                    20, 20, rc.right - 40, rc.bottom - 80, hwnd, (HMENU)ID_LISTA, NULL, NULL);
            CreateWindow(L"BUTTON", L"Regresar al Menú", WS_CHILD | WS_VISIBLE, 20, rc.bottom - 50, 150, 30, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);

            const char* conninfo = "dbname=postgres user=postgres password=Awesome94 host=localhost port=5432";
            PGconn* conn = PQconnectdb(conninfo);

            if (PQstatus(conn) != CONNECTION_OK) {
                MessageBox(hwnd, L"Error de conexión a la base de datos", L"Error", MB_ICONERROR);
                PQfinish(conn);
                return 0;
            }

            PGresult* res = PQexec(conn, "SELECT titulo, autor FROM Libros");

            if (PQresultStatus(res) == PGRES_TUPLES_OK) {
                int rows = PQntuples(res);
                for (int i = 0; i < rows; i++) {
                    std::wstring titulo = std::wstring(PQgetvalue(res, i, 0), PQgetvalue(res, i, 0) + strlen(PQgetvalue(res, i, 0)));
                    std::wstring autor = std::wstring(PQgetvalue(res, i, 1), PQgetvalue(res, i, 1) + strlen(PQgetvalue(res, i, 1)));
                    std::wstring item = titulo + L" - " + autor;
                    SendMessage(hListado, LB_ADDSTRING, 0, (LPARAM)item.c_str());
                }
            } else {
                MessageBox(hwnd, L"Error al listar los libros", L"Error", MB_ICONERROR);
            }

            PQclear(res);
            PQfinish(conn);
            return 0;
        }
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH hBrush = CreateSolidBrush(RGB(139, 69, 19));
            FillRect(hdc, &rc, hBrush);
            DeleteObject(hBrush);
            return 1;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_REGRESAR) {
                RegresarAlMenu(hwnd);
            }
            return 0;
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void listarLibros(HWND hParent) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"ListarLibrosClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = ListarLibrosProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    RegisterClass(&wc);

    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    int parentWidth = rcParent.right - rcParent.left;
    int parentHeight = rcParent.bottom - rcParent.top;

    int width = 600;
    int height = 400;
    int x = rcParent.left + (parentWidth - width) / 2;
    int y = rcParent.top + (parentHeight - height) / 2;

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Listado de Libros",
        WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        x, y, width, height,
        hParent,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd) {
        MessageBox(hParent, L"No se pudo crear la ventana de Listar libros.", L"Error", MB_ICONERROR);
        return;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
}