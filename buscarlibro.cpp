// buscarlibro.cpp

#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <cstring>
#include <vector>
#include "buscarlibro.h"
#include "menu.h" // Para la declaración de RegresarAlMenu

#define ID_EDIT_BUSCAR 1
#define ID_BTN_BUSCAR 2
#define ID_LISTA_RESULTADOS 3
#define ID_BTN_REGRESAR 4

HWND hEditBuscar, hListResultados;

LRESULT CALLBACK BuscarLibroProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::vector<std::wstring> resultados;
    switch (uMsg) {
        case WM_CREATE: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            int margin = 20;
            int editHeight = 25;
            int buttonHeight = 30;

            CreateWindow(L"STATIC", L"Ingrese título o autor a buscar:", WS_CHILD | WS_VISIBLE,
                         margin, margin, 200, 20, hwnd, NULL, NULL, NULL);
            hEditBuscar = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
                                       margin, margin + 25, 300, editHeight, hwnd, (HMENU)ID_EDIT_BUSCAR, NULL, NULL);
            CreateWindow(L"BUTTON", L"Buscar", WS_CHILD | WS_VISIBLE,
                         margin + 310, margin + 25, 100, editHeight, hwnd, (HMENU)ID_BTN_BUSCAR, NULL, NULL);
            hListResultados = CreateWindow(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
                                           margin, margin + 60, rc.right - 2 * margin, rc.bottom - 100, hwnd, (HMENU)ID_LISTA_RESULTADOS, NULL, NULL);
            CreateWindow(L"BUTTON", L"Regresar al Menú", WS_CHILD | WS_VISIBLE,
                         margin, rc.bottom - margin - buttonHeight, 150, buttonHeight, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);
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
            if (LOWORD(wParam) == ID_BTN_BUSCAR) {
                wchar_t textoBuscarW[255];
                GetWindowText(hEditBuscar, textoBuscarW, 255);
                char textoBuscarChar[255];
                wcstombs(textoBuscarChar, textoBuscarW, 255);

                const char* conninfo = "dbname=postgres user=postgres password=Awesome94 host=localhost port=5432";
                PGconn* conn = PQconnectdb(conninfo);

                if (PQstatus(conn) != CONNECTION_OK) {
                    MessageBox(hwnd, L"Error de conexión a la base de datos", L"Error", MB_ICONERROR);
                    PQfinish(conn);
                    return 0;
                }

                std::string query = "SELECT titulo, autor, isbn FROM Libros WHERE LOWER(titulo) LIKE LOWER('%" + std::string(textoBuscarChar) + "%') OR LOWER(autor) LIKE LOWER('%" + std::string(textoBuscarChar) + "%')";
                PGresult* res = PQexec(conn, query.c_str());

                SendMessage(hListResultados, LB_RESETCONTENT, 0, 0);
                resultados.clear();

                if (PQresultStatus(res) == PGRES_TUPLES_OK) {
                    int rows = PQntuples(res);
                    for (int i = 0; i < rows; i++) {
                        std::string titulo = PQgetvalue(res, i, 0);
                        std::string autor = PQgetvalue(res, i, 1);
                        std::string isbn = PQgetvalue(res, i, 2);
                        std::wstring item = std::wstring(titulo.begin(), titulo.end()) + L" - " +
                                             std::wstring(autor.begin(), autor.end()) + L" (ISBN: " +
                                             std::wstring(isbn.begin(), isbn.end()) + L")";
                        SendMessage(hListResultados, LB_ADDSTRING, 0, (LPARAM)item.c_str());
                        resultados.push_back(item);
                    }
                    if (rows == 0) {
                        MessageBox(hwnd, L"No se encontraron libros con ese título o autor.", L"Información", MB_ICONINFORMATION);
                    }
                } else {
                    MessageBox(hwnd, L"Error al realizar la búsqueda.", L"Error", MB_ICONERROR);
                }

                PQclear(res);
                PQfinish(conn);
            } else if (LOWORD(wParam) == ID_BTN_REGRESAR) {
                RegresarAlMenu(hwnd);
            }
            return 0;
        case WM_DESTROY:
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void buscarLibro(HWND hParent) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"BuscarLibroClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = BuscarLibroProc;
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
        L"Buscar Libro",
        WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        x, y, width, height,
        hParent,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd) {
        MessageBox(hParent, L"No se pudo crear la ventana de Buscar libro.", L"Error", MB_ICONERROR);
        return;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
}