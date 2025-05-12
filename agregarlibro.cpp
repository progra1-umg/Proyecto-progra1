// agregarlibro.cpp

#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <cstring>
#include "agregarlibro.h"
#include "menu.h" // Incluimos menu.h para la declaración de RegresarAlMenu

#define ID_BTN_INSERTAR 1
#define ID_BTN_REGRESAR 2

// Campos de entrada
HWND hEditTitulo_agregar, hEditAutor_agregar, hEditISBN_agregar, hEditEditorial_agregar, hEditAnio_agregar, hEditMateria_agregar;

void InsertarDatosEnDB(HWND hwnd) {
    wchar_t titulo[255], autor[255], isbn[20], editorial[255], anio[10], materia[100];
    GetWindowText(hEditTitulo_agregar, titulo, 255);
    GetWindowText(hEditAutor_agregar, autor, 255);
    GetWindowText(hEditISBN_agregar, isbn, 20);
    GetWindowText(hEditEditorial_agregar, editorial, 255);
    GetWindowText(hEditAnio_agregar, anio, 10);
    GetWindowText(hEditMateria_agregar, materia, 100);

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

    const char* conninfo = "dbname=postgres user=postgres password=Awesome94 host=localhost port=5432";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        MessageBox(hwnd, L"Error de conexión a la base de datos", L"Error", MB_ICONERROR);
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

    // Convertir año a número, usar std::stoi para manejar conversiones incorrectas
    int anioNum = 0;
    try {
        anioNum = std::stoi(anioChar);
    } catch (...) {
        MessageBox(hwnd, L"Año de publicación inválido", L"Error FA3", MB_ICONERROR);
        PQfinish(conn);
        return;
    }

    std::string query = "INSERT INTO Libros (titulo, autor, isbn, editorial, anio_publicacion, materia, estado_libro) VALUES ('" +
                        std::string(tituloChar) + "', '" +
                        std::string(autorChar) + "', '" +
                        std::string(isbnChar) + "', '" +
                        std::string(editorialChar) + "', " +
                        std::to_string(anioNum) + ", '" +
                        std::string(materiaChar) + "', 'Disponible')";

    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        MessageBox(hwnd, L"Error al insertar libro", L"Error", MB_ICONERROR);
    } else {
        MessageBox(hwnd, L"Libro insertado correctamente", L"Éxito", MB_ICONINFORMATION);
    }

    PQclear(res);
    PQfinish(conn);
}

LRESULT CALLBACK AgregarLibroProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateWindow(L"STATIC", L"Título:", WS_CHILD | WS_VISIBLE, 20, 5, 200, 15, hwnd, NULL, NULL, NULL);
            hEditTitulo_agregar = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 20, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Autor:", WS_CHILD | WS_VISIBLE, 20, 45, 200, 15, hwnd, NULL, NULL, NULL);
            hEditAutor_agregar = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 60, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"ISBN:", WS_CHILD | WS_VISIBLE, 20, 85, 200, 15, hwnd, NULL, NULL, NULL);
            hEditISBN_agregar = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 100, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Editorial:", WS_CHILD | WS_VISIBLE, 20, 125, 200, 15, hwnd, NULL, NULL, NULL);
            hEditEditorial_agregar = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 140, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Año de Publicación:", WS_CHILD | WS_VISIBLE, 20, 165, 200, 15, hwnd, NULL, NULL, NULL);
            hEditAnio_agregar = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 180, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"STATIC", L"Materia:", WS_CHILD | WS_VISIBLE, 20, 205, 200, 15, hwnd, NULL, NULL, NULL);
            hEditMateria_agregar = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 220, 200, 20, hwnd, NULL, NULL, NULL);

            CreateWindow(L"BUTTON", L"Insertar Libro", WS_CHILD | WS_VISIBLE, 20, 250, 200, 30, hwnd, (HMENU)ID_BTN_INSERTAR, NULL, NULL);
            CreateWindow(L"BUTTON", L"Regresar al Menú", WS_CHILD | WS_VISIBLE, 20, 290, 200, 30, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);
            return 0;

        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hwnd, &rc);

            HBRUSH hBrush = CreateSolidBrush(RGB(139, 69, 19));
            FillRect(hdc, &rc, hBrush);

            DeleteObject(hBrush);
            return 1; // Indica que el fondo se ha manejado correctamente
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BTN_INSERTAR) {
                InsertarDatosEnDB(hwnd);
            } else if (LOWORD(wParam) == ID_BTN_REGRESAR) {
                RegresarAlMenu(hwnd);
            }
            return 0;

        case WM_DESTROY:
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void agregarLibro(HWND hParent) {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"AgregarLibrosClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = AgregarLibroProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); // Optional: set background color

    RegisterClass(&wc);

    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    int parentWidth = rcParent.right - rcParent.left;
    int parentHeight = rcParent.bottom - rcParent.top;

    int windowWidth = 300;
    int windowHeight = 400;
    int x = rcParent.left + (parentWidth - windowWidth) / 2;
    int y = rcParent.top + (parentHeight - windowHeight) / 2;

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Agregar libros",             // Window text
        WS_CAPTION | WS_SYSMENU,        // Window style

        // Size and position
        x, y, windowWidth, windowHeight,

        hParent,                        // Parent window
        nullptr,                        // Menu
        hInstance,                      // Instance handle
        nullptr                         // Additional application data
    );

    if (!hwnd) {
        MessageBox(hParent, L"No se pudo crear la ventana de Agregar libro.", L"Error", MB_ICONERROR);
        return;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
}