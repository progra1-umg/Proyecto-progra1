#define UNICODE
#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <vector>
#include <sstream>
#include <codecvt>
#include <locale>

#define ID_BTN_REGRESAR 1

HWND hListResultados;

struct Libro {
    std::wstring titulo;
    std::wstring autor;
    std::wstring isbn;
    std::wstring editorial;
    std::wstring anio;
    std::wstring materia;
    std::wstring estado;
};

std::vector<Libro> listaLibros;


std::wstring utf8ToWString(const char* utf8Str) {
    if (utf8Str == nullptr) return L"";
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8Str);
}

void ListarTodosLosLibros(HWND hwnd) {
    listaLibros.clear();
    SendMessage(hListResultados, LB_RESETCONTENT, 0, 0);

    const char* conninfo = "dbname=postgres user=postgres password=iker3112 host=localhost port=5432";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        MessageBox(hwnd, L"Error de conexión a la base de datos", L"Error", MB_ICONERROR);
        PQfinish(conn);
        return;
    }

    std::string query = "SELECT titulo, autor, isbn, editorial, anio_publicacion, materia, estado_libro FROM Libros ORDER BY titulo ASC";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::wstring errorMsg = utf8ToWString(PQerrorMessage(conn));
        MessageBox(hwnd, errorMsg.c_str(), L"Error al ejecutar la consulta", MB_ICONERROR);
        PQclear(res);
        PQfinish(conn);
        return;
    }
    

    int rows = PQntuples(res);
    if (rows == 0) {
        MessageBox(hwnd, L"No hay libros registrados en la biblioteca", L"Información", MB_ICONINFORMATION);
    } else {
        for (int i = 0; i < rows; i++) {
            Libro libro;
            libro.titulo = utf8ToWString(PQgetvalue(res, i, 0));
            libro.autor = utf8ToWString(PQgetvalue(res, i, 1));
            libro.isbn = utf8ToWString(PQgetvalue(res, i, 2));
            libro.editorial = utf8ToWString(PQgetvalue(res, i, 3));
            libro.anio = utf8ToWString(PQgetvalue(res, i, 4));
            libro.materia = utf8ToWString(PQgetvalue(res, i, 5));
            libro.estado = utf8ToWString(PQgetvalue(res, i, 6));

            listaLibros.push_back(libro);

            std::wstring item = libro.titulo + L" | " + libro.autor + L" | ISBN: " + libro.isbn + L" | Estado: " + libro.estado;
            SendMessage(hListResultados, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
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
            CreateWindow(L"STATIC", L"Lista de libros registrados en la biblioteca:", WS_CHILD | WS_VISIBLE,
                         20, 20, 400, 20, hwnd, NULL, NULL, NULL);

            hListResultados = CreateWindow(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                                           20, 50, 450, 370, hwnd, NULL, NULL, NULL);

            CreateWindow(L"BUTTON", L"Regresar al Menú", WS_CHILD | WS_VISIBLE,
                         20, 430, 150, 30, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);

            ListarTodosLosLibros(hwnd);
            return 0;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BTN_REGRESAR) {
                RegresarAlMenu(hwnd);
            }
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
    wc.lpszClassName = L"ListarLibros";

    RegisterClass(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 500;
    int windowHeight = 500;

    HWND hwnd = CreateWindowEx(0, L"ListarLibros", L"Listado de Libros",
                               WS_OVERLAPPEDWINDOW,
                               (screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2,
                               windowWidth, windowHeight,
                               NULL, NULL, hInstance, NULL);

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
