#ifndef CONEXION_DB_H
#define CONEXION_DB_H

#include <libpq-fe.h>
#include <string>

PGconn* conectarDB();
void cerrarConexion(PGconn* conn);
void mostrarError(PGconn* conn);
bool ejecutarConsulta(PGconn* conn, const std::string& query);

#endif
