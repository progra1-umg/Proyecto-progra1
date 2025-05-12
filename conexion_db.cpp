#include "conexion_db.h"
#include <iostream>

PGconn* conectarDB() {
    const char* conninfo = "host=localhost port=5432 dbname=Biblioteca user=postgres password=Awesome94";
    PGconn* conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Error al conectar a la base de datos: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return nullptr;
    }

    return conn;
}

void cerrarConexion(PGconn* conn) {
    if (conn != nullptr) {
        PQfinish(conn);
    }
}

void mostrarError(PGconn* conn) {
    std::cerr << "Error: " << PQerrorMessage(conn) << std::endl;
}

bool ejecutarConsulta(PGconn* conn, const std::string& query) {
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Error en la consulta: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}
