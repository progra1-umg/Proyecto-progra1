#define UNICODE
#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <cstring>

#define ID_BTN_DEVOLVER 1
#define ID_BTN_REGRESAR 2

HWND hEditISBN;

void DevolverLibro(HWND hwnd) {
    wchar_t isbnW[20];
    GetWindowText(hEditISBN, isbnW, 20);

    if (wcslen(isbnW) == 0) {
        MessageBox(hwnd, L"Debe ingresar el ISBN del libro a devolver.", L"Error FA1", MB_ICONERROR);
        return;
    }

    char isbnChar[20];
    wcstombs(isbnChar, isbnW, 20);

    const char* conninfo = "dbname=postgres user=postgres password=iker3112 host=localhost port=5432";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        MessageBox(hwnd, L"Error de conexión a la base de datos", L"Error", MB_ICONERROR);
        PQfinish(conn);
        return;
    }

    std::string queryBuscar = "SELECT estado_libro FROM Libros WHERE isbn = '" + std::string(isbnChar) + "'";
    PGresult* res = PQexec(conn, queryBuscar.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        MessageBox(hwnd, L"No se encontró el libro con el ISBN ingresado", L"Error FA2", MB_ICONINFORMATION);
        PQclear(res);
        PQfinish(conn);
        return;
    }

    std::string estado = PQgetvalue(res, 0, 0);
    PQclear(res);

    if (estado != "prestado" && estado != "Prestado" && estado != "PRESTADO") {
        MessageBox(hwnd, L"El libro no está prestado, no se puede devolver.", L"Advertencia", MB_ICONWARNING);
        PQfinish(conn);
        return;
    }

    std::string queryUpdate = "UPDATE Libros SET estado_libro = 'Disponible' WHERE isbn = '" + std::string(isbnChar) + "'";
    res = PQexec(conn, queryUpdate.c_str());

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        MessageBox(hwnd, L"El libro ha sido devuelto correctamente.", L"Éxito", MB_ICONINFORMATION);

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcess(L"menu.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        PostQuitMessage(0);
    } else {
        MessageBox(hwnd, L"Ocurrió un error al actualizar el estado del libro.", L"Error", MB_ICONERROR);
    }

    PQclear(res);
    PQfinish(conn);
}

void RegresarAlMenu(HWND hwnd) {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcess(L"menu.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        MessageBox(hwnd, L"No se pudo abrir menu.exe", L"Error", MB_ICONERROR);
    }

    PostQuitMessage(0);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            int centerX = (rc.right - 250) / 2;

            CreateWindow(L"STATIC", L"Ingrese ISBN del libro a devolver:", WS_CHILD | WS_VISIBLE, centerX, 20, 250, 20, hwnd, NULL, NULL, NULL);
            hEditISBN = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, centerX, 45, 250, 25, hwnd, NULL, NULL, NULL);
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
                DevolverLibro(hwnd);
            } else if (LOWORD(wParam) == ID_BTN_REGRESAR) {
                RegresarAlMenu(hwnd);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Devolver libros";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, L"Devolver libros", L"Devolver libros",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"No se pudo crear la ventana", L"Error", MB_ICONERROR);
        return 0;
    }

    RECT rc;
    GetWindowRect(hwnd, &rc);
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int winWidth = rc.right - rc.left;
    int winHeight = rc.bottom - rc.top;

    SetWindowPos(hwnd, NULL, 
        (screenWidth - winWidth) / 2, 
        (screenHeight - winHeight) / 2, 
        0, 0, SWP_NOZORDER | SWP_NOSIZE);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}