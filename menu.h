// menu.h
#ifndef MENU_H
#define MENU_H

#pragma once
#include <windows.h>
void IniciarMenu(HINSTANCE hInstance, int nCmdShow);

// Declaraciones de funciones de otros archivos .cpp que se llaman desde el menú
void agregarLibro(HWND hParent);
void prestarLibro(HWND hParent);
void devolverLibro(HWND hParent);
void listarLibros(HWND hParent);
void buscarLibro(HWND hParent);

// Declaración de la función RegresarAlMenu (aunque se define en cada .cpp)
// para evitar errores de compilación si se incluye menu.h en otros archivos.
void RegresarAlMenu(HWND hwnd);

#endif // MENU_H