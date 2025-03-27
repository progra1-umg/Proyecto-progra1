#define UNICODE
#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <cstring>

#define ID_BTN_PRESTAR 1
#define ID_BTN_REGRESAR 2

HWND hEditISBN;

void PrestarLibro(HWND hwnd) {
    wchar_t isbn[20];
    GetWindowText(hEditISBN, isbn, 20);

    if (wcslen(isbn) == 0) {
        MessageBox(hwnd, L"El campo ISBN es obligatorio.", L"Error", MB_ICONERROR);
        return;
    }

    char isbnChar[20];
    wcstombs(isbnChar, isbn, 20);

    const char* conninfo = "dbname=postgres user=postgres password=iker3112 host=localhost port=5432";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        MessageBox(hwnd, L"Error de conexión a la base de datos", L"Error", MB_ICONERROR);
        PQfinish(conn);
        return;
    }

    std::string checkQuery = "SELECT estado_libro FROM Libros WHERE isbn = '" + std::string(isbnChar) + "'";
    PGresult* checkRes = PQexec(conn, checkQuery.c_str());

    if (PQresultStatus(checkRes) != PGRES_TUPLES_OK || PQntuples(checkRes) == 0) {
        MessageBox(hwnd, L"No se encontró el libro con el ISBN ingresado.", L"Error", MB_ICONERROR);
        PQclear(checkRes);
        PQfinish(conn);
        return;
    }

    std::string estado = PQgetvalue(checkRes, 0, 0);
    PQclear(checkRes);

    if (estado == "prestado") {
        MessageBox(hwnd, L"El libro ya está prestado y no se puede realizar la operación.", L"Error", MB_ICONERROR);
        PQfinish(conn);
        return;
    }

    std::string updateQuery = "UPDATE Libros SET estado_libro = 'prestado' WHERE isbn = '" + std::string(isbnChar) + "'";
    PGresult* res = PQexec(conn, updateQuery.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        MessageBox(hwnd, L"Error al prestar el libro.", L"Error", MB_ICONERROR);
    } else {
        MessageBox(hwnd, L"El libro ha sido prestado con éxito.", L"Éxito", MB_ICONINFORMATION);
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
        case WM_CREATE:
            CreateWindow(L"STATIC", L"ISBN:", WS_CHILD | WS_VISIBLE, 50, 50, 400, 25, hwnd, NULL, NULL, NULL);
            hEditISBN = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 80, 400, 30, hwnd, NULL, NULL, NULL);

            CreateWindow(L"BUTTON", L"Prestar Libro", WS_CHILD | WS_VISIBLE, 50, 130, 400, 40, hwnd, (HMENU)ID_BTN_PRESTAR, NULL, NULL);
            CreateWindow(L"BUTTON", L"Regresar al Menú", WS_CHILD | WS_VISIBLE, 50, 190, 400, 40, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);
            return 0;

        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            HBRUSH hBrush = CreateSolidBrush(RGB(139, 69, 19));
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            return 1;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_PRESTAR) {
                PrestarLibro(hwnd);
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
    wc.lpszClassName = L"Prestar libros";
    wc.hbrBackground = CreateSolidBrush(RGB(139, 69, 19));

    RegisterClass(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 500;
    int windowHeight = 400;

    HWND hwnd = CreateWindowEx(0, L"Prestar libros", L"Prestar libro",
        WS_OVERLAPPEDWINDOW, (screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2,
        windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"No se pudo crear la ventana", L"Error", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}