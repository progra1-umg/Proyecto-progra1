#define UNICODE
#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <cstring>

#define ID_BTN_INSERTAR 1
#define ID_BTN_REGRESAR 2

// Campos de entrada
HWND hEditTitulo, hEditAutor, hEditISBN, hEditEditorial, hEditAnio, hEditMateria;

void InsertarDatosEnDB(HWND hwnd) {
    wchar_t titulo[255], autor[255], isbn[20], editorial[255], anio[10], materia[100];
    GetWindowText(hEditTitulo, titulo, 255);
    GetWindowText(hEditAutor, autor, 255);
    GetWindowText(hEditISBN, isbn, 20);
    GetWindowText(hEditEditorial, editorial, 255);
    GetWindowText(hEditAnio, anio, 10);
    GetWindowText(hEditMateria, materia, 100);

      // Validar que ningún campo esté vacío 
      if (wcslen(titulo) == 0 || wcslen(autor) == 0 || wcslen(isbn) == 0 || wcslen(editorial) == 0 || wcslen(anio) == 0 || wcslen(materia) == 0) {
        MessageBox(hwnd, L"Todos los campos son requeridos. Favor completar la información.", L"Error FA1", MB_ICONERROR);
        return;
    }

    char tituloChar[255], autorChar[255], isbnChar[20], editorialChar[255], anioChar[10], materiaChar[100];
    wcstombs(tituloChar, titulo, 255);
    wcstombs(autorChar, autor, 255);
    wcstombs(isbnChar, isbn, 20);
    wcstombs(editorialChar, editorial, 255);
    wcstombs(anioChar, anio, 10);
    wcstombs(materiaChar, materia, 100);

    const char* conninfo = "dbname=postgres user=postgres password=iker3112 host=localhost port=5432";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        MessageBox(hwnd, L"Error de conexi\u00f3n a la base de datos", L"Error", MB_ICONERROR);
        PQfinish(conn);
        return;
    }

        // Verificar si el ISBN ya existe
        std::string checkQuery = "SELECT COUNT(*) FROM Libros WHERE isbn = '" + std::string(isbnChar) + "'";
        PGresult* checkRes = PQexec(conn, checkQuery.c_str());
    
        if (PQresultStatus(checkRes) != PGRES_TUPLES_OK) {
            MessageBox(hwnd, L"Error al verificar el ISBN", L"Error", MB_ICONERROR);
            PQclear(checkRes);
            PQfinish(conn);
            return;
        }
    
        int count = atoi(PQgetvalue(checkRes, 0, 0));
        PQclear(checkRes);
    
        if (count > 0) {
            MessageBox(hwnd, L"ISBN ya está registrado. Favor verificar.", L"Error FA2", MB_ICONERROR);
            PQfinish(conn);
            return;
        }

    std::string query = "INSERT INTO Libros (titulo, autor, isbn, editorial, anio_publicacion, materia) VALUES ('" +
                         std::string(tituloChar) + "', '" +
                         std::string(autorChar) + "', '" +
                         std::string(isbnChar) + "', '" +
                         std::string(editorialChar) + "', " +
                         std::string(anioChar) + ", '" +
                         std::string(materiaChar) + "')";

    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        MessageBox(hwnd, L"Error al insertar libro", L"Error", MB_ICONERROR);
    } else {
        MessageBox(hwnd, L"Libro insertado correctamente", L"\u00c9xito", MB_ICONINFORMATION);
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
            CreateWindow(L"STATIC", L"T\u00edtulo:", WS_CHILD | WS_VISIBLE, 20, 5, 200, 15, hwnd, NULL, NULL, NULL);
            hEditTitulo = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 20, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Autor:", WS_CHILD | WS_VISIBLE, 20, 45, 200, 15, hwnd, NULL, NULL, NULL);
            hEditAutor = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 60, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"ISBN:", WS_CHILD | WS_VISIBLE, 20, 85, 200, 15, hwnd, NULL, NULL, NULL);
            hEditISBN = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 100, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Editorial:", WS_CHILD | WS_VISIBLE, 20, 125, 200, 15, hwnd, NULL, NULL, NULL);
            hEditEditorial = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 140, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"A\u00f1o de Publicaci\u00f3n:", WS_CHILD | WS_VISIBLE, 20, 165, 200, 15, hwnd, NULL, NULL, NULL);
            hEditAnio = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 180, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Materia:", WS_CHILD | WS_VISIBLE, 20, 205, 200, 15, hwnd, NULL, NULL, NULL);
            hEditMateria = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 220, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"BUTTON", L"Insertar Libro", WS_CHILD | WS_VISIBLE, 20, 250, 200, 30, hwnd, (HMENU)ID_BTN_INSERTAR, NULL, NULL);
            CreateWindow(L"BUTTON", L"Regresar al Men\u00fa", WS_CHILD | WS_VISIBLE, 20, 290, 200, 30, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);
            return 0;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_INSERTAR) {
                InsertarDatosEnDB(hwnd);
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
    wc.lpszClassName = L"Agregar libros";

    RegisterClass(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 300;
    int windowHeight = 400;

    HWND hwnd = CreateWindowEx(0, L"Agregar libros", L"Agregar libros",
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