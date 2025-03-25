#define UNICODE
#include <windows.h>
#include <libpq-fe.h>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>

#define ID_BTN_BUSCAR 1
#define ID_BTN_REGRESAR 2
#define ID_RADIO_TITULO 3
#define ID_RADIO_AUTOR 4

// Campos de entrada y controles
HWND hEditBusqueda, hListResultados, hRadioTitulo, hRadioAutor;

struct Libro {
    std::wstring titulo;
    std::wstring autor;
    std::wstring isbn;
    std::wstring editorial;
    std::wstring anio;
    std::wstring materia;
};

std::vector<Libro> resultadosBusqueda;

void BuscarLibros(HWND hwnd) {
    wchar_t criterio[255];
    GetWindowText(hEditBusqueda, criterio, 255);

    // Validar que se haya ingresado un criterio
    if (wcslen(criterio) == 0) {
        MessageBox(hwnd, L"Por favor, introduzca un criterio de búsqueda", L"Error", MB_ICONERROR);
        return;
    }

    // Determinar el tipo de búsqueda (título o autor)
    bool buscarPorTitulo = (SendMessage(hRadioTitulo, BM_GETCHECK, 0, 0) == BST_CHECKED);

    // Convertir el criterio a char* para PostgreSQL
    char criterioChar[255];
    wcstombs(criterioChar, criterio, 255);

    // Limpiar resultados anteriores
    resultadosBusqueda.clear();
    SendMessage(hListResultados, LB_RESETCONTENT, 0, 0);

    // Conectar a la base de datos
    const char* conninfo = "dbname=postgres user=postgres password=Awesome94 host=localhost port=5432";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        MessageBox(hwnd, L"Error de conexión a la base de datos", L"Error", MB_ICONERROR);
        PQfinish(conn);
        return;
    }

    // Construir la consulta SQL
    std::string query;
    if (buscarPorTitulo) {
        query = "SELECT titulo, autor, isbn, editorial, anio_publicacion, materia FROM Libros WHERE titulo ILIKE '%" + 
                std::string(criterioChar) + "%'";
    } else {
        query = "SELECT titulo, autor, isbn, editorial, anio_publicacion, materia FROM Libros WHERE autor ILIKE '%" + 
                std::string(criterioChar) + "%'";
    }

    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        MessageBox(hwnd, L"Error al ejecutar la consulta", L"Error", MB_ICONERROR);
        PQclear(res);
        PQfinish(conn);
        return;
    }

    int rows = PQntuples(res);
    if (rows == 0) {
        MessageBox(hwnd, L"No se encontró información", L"Información", MB_ICONINFORMATION);
    } else {
        // Procesar resultados
        for (int i = 0; i < rows; i++) {
            Libro libro;
            
            // Convertir cada campo de char* a wstring
            libro.titulo = std::wstring(PQgetvalue(res, i, 0), PQgetvalue(res, i, 0) + strlen(PQgetvalue(res, i, 0)));
            libro.autor = std::wstring(PQgetvalue(res, i, 1), PQgetvalue(res, i, 1) + strlen(PQgetvalue(res, i, 1)));
            libro.isbn = std::wstring(PQgetvalue(res, i, 2), PQgetvalue(res, i, 2) + strlen(PQgetvalue(res, i, 2)));
            libro.editorial = std::wstring(PQgetvalue(res, i, 3), PQgetvalue(res, i, 3) + strlen(PQgetvalue(res, i, 3)));
            libro.anio = std::wstring(PQgetvalue(res, i, 4), PQgetvalue(res, i, 4) + strlen(PQgetvalue(res, i, 4)));
            libro.materia = std::wstring(PQgetvalue(res, i, 5), PQgetvalue(res, i, 5) + strlen(PQgetvalue(res, i, 5)));
            
            resultadosBusqueda.push_back(libro);
            
            // Mostrar título y autor en la lista
            std::wstring item = libro.titulo + L" - " + libro.autor;
            SendMessage(hListResultados, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
    }

    PQclear(res);
    PQfinish(conn);
}

void MostrarDetalleLibro(HWND hwnd, int index) {
    if (index < 0 || index >= resultadosBusqueda.size()) return;

    const Libro& libro = resultadosBusqueda[index];
    
    std::wstringstream detalle;
    detalle << L"Título: " << libro.titulo << L"\n";
    detalle << L"Autor: " << libro.autor << L"\n";
    detalle << L"ISBN: " << libro.isbn << L"\n";
    detalle << L"Editorial: " << libro.editorial << L"\n";
    detalle << L"Año: " << libro.anio << L"\n";
    detalle << L"Materia: " << libro.materia;
    
    MessageBox(hwnd, detalle.str().c_str(), L"Detalle del Libro", MB_ICONINFORMATION);
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
            CreateWindow(L"STATIC", L"Criterio de búsqueda:", WS_CHILD | WS_VISIBLE, 20, 20, 200, 20, hwnd, NULL, NULL, NULL);
            hEditBusqueda = CreateWindow(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 40, 300, 25, hwnd, NULL, NULL, NULL);
            
            hRadioTitulo = CreateWindow(L"BUTTON", L"Buscar por título", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, 
                                       20, 75, 150, 20, hwnd, (HMENU)ID_RADIO_TITULO, NULL, NULL);
            hRadioAutor = CreateWindow(L"BUTTON", L"Buscar por autor", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, 
                                      20, 100, 150, 20, hwnd, (HMENU)ID_RADIO_AUTOR, NULL, NULL);
            
            // Seleccionar "Buscar por título" por defecto
            SendMessage(hRadioTitulo, BM_SETCHECK, BST_CHECKED, 0);
            
            CreateWindow(L"BUTTON", L"Buscar", WS_CHILD | WS_VISIBLE, 20, 130, 100, 30, hwnd, (HMENU)ID_BTN_BUSCAR, NULL, NULL);
            
            CreateWindow(L"STATIC", L"Resultados:", WS_CHILD | WS_VISIBLE, 20, 170, 100, 20, hwnd, NULL, NULL, NULL);
            hListResultados = CreateWindow(L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY, 
                                          20, 190, 450, 200, hwnd, NULL, NULL, NULL);
            
            CreateWindow(L"BUTTON", L"Regresar al Menú", WS_CHILD | WS_VISIBLE, 20, 400, 150, 30, hwnd, (HMENU)ID_BTN_REGRESAR, NULL, NULL);
            return 0;
        }
        
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BTN_BUSCAR) {
                BuscarLibros(hwnd);
            } else if (LOWORD(wParam) == ID_BTN_REGRESAR) {
                RegresarAlMenu(hwnd);
            } else if (LOWORD(wParam) == ID_RADIO_TITULO || LOWORD(wParam) == ID_RADIO_AUTOR) {
                // Manejar cambio de selección de radio buttons
            } else if (HIWORD(wParam) == LBN_DBLCLK && (HWND)lParam == hListResultados) {
                // Mostrar detalles cuando se hace doble clic en un resultado
                int index = SendMessage(hListResultados, LB_GETCURSEL, 0, 0);
                MostrarDetalleLibro(hwnd, index);
            }
            return 0;
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
    wc.lpszClassName = L"BuscarLibros";

    RegisterClass(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 500;
    int windowHeight = 500;

    HWND hwnd = CreateWindowEx(0, L"BuscarLibros", L"Buscar Libros",
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