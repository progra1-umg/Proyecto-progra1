#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <cstring>
#include "devolverlibro.h"
#include "menu.h" // Para la declaración de RegresarAlMenu

#define ID_BTN_DEVOLVER 1
#define ID_BTN_REGRESAR 2

HWND hEditISBNDevolver;

LRESULT CALLBACK DevolverLibroProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            int centerX = (rc.right - 250) / 2;

            CreateWindow(L"STATIC", L"Ingrese ISBN del libro a devolver:", WS_CHILD | WS_VISIBLE, centerX, 20, 250, 20, hwnd, NULL, NULL, NULL);
            hEditISBNDevolver = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, centerX, 45, 250, 25, hwnd, NULL, NULL, NULL);
            CreateWindow(L"BUTTON", L"Devolver Libro", WS_CHILD | WS_VISIBLE, centerX, 80, 250, 30, hwnd, (HMENU)ID_BTN_DEVOLVER, NULL, NULL);
            CreateWindow(L"BUTTON", L"Regresar al Menú", WS_CHILD | WS_VISIBLE, centerX, 120, 250, 30, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);
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
            if (LOWORD(wParam) == ID_BTN_DEVOLVER) {
                wchar_t isbnW[20];
                GetWindowText(hEditISBNDevolver, isbnW, 20);

                if (wcslen(isbnW) == 0) {
                    MessageBox(hwnd, L"Debe ingresar el ISBN del libro a devolver.", L"Error", MB_ICONERROR);
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
                }

                char isbnChar[20];
                wcstombs(isbnChar, isbnW, 20);

                const char* conninfo = "dbname=postgres user=postgres password=Awesome94 host=localhost port=5432";
                PGconn* conn = PQconnectdb(conninfo);

                if (PQstatus(conn) != CONNECTION_OK) {
                    MessageBox(hwnd, L"Error de conexión a la base de datos", L"Error", MB_ICONERROR);
                    PQfinish(conn);
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
                }

                std::string queryBuscar = "SELECT estado_libro FROM Libros WHERE isbn = '" + std::string(isbnChar) + "'";
                PGresult* res = PQexec(conn, queryBuscar.c_str());

                if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
                    MessageBox(hwnd, L"No se encontró el libro con el ISBN ingresado", L"Error", MB_ICONINFORMATION);
                    PQclear(res);
                    PQfinish(conn);
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
                }

                std::string estado = PQgetvalue(res, 0, 0);
                PQclear(res);

                if (estado != "Prestado" && estado != "prestado" && estado != "PRESTADO") {
                    MessageBox(hwnd, L"El libro no está actualmente prestado.", L"Advertencia", MB_ICONWARNING);
                    PQfinish(conn);
                    return DefWindowProc(hwnd, uMsg, wParam, lParam);
                }

                std::string queryUpdate = "UPDATE Libros SET estado_libro = 'Disponible' WHERE isbn = '" + std::string(isbnChar) + "'";
                PGresult* resUpdate = PQexec(conn, queryUpdate.c_str());

                if (PQresultStatus(resUpdate) == PGRES_COMMAND_OK) {
                    MessageBox(hwnd, L"El libro ha sido devuelto.", L"Éxito", MB_ICONINFORMATION);
                } else {
                    MessageBox(hwnd, L"Ocurrió un error al devolver el libro.", L"Error", MB_ICONERROR);
                }

                PQclear(resUpdate);
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

void devolverLibro(HWND hParent) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"DevolverLibroClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = DevolverLibroProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    RegisterClass(&wc);

    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    int parentWidth = rcParent.right - rcParent.left;
    int parentHeight = rcParent.bottom - rcParent.top;

    int width = 400;
    int height = 200;
    int x = rcParent.left + (parentWidth - width) / 2;
    int y = rcParent.top + (parentHeight - height) / 2;

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Devolver Libro",
        WS_CAPTION | WS_SYSMENU,
        x, y, width, height,
        hParent,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd) {
        MessageBox(hParent, L"No se pudo crear la ventana de Devolver libro.", L"Error", MB_ICONERROR);
        return;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
}
