-- Creación de la base de datos
CREATE DATABASE Biblioteca;

-- Una vez conectados a la base, crear la tabla:
CREATE TABLE Libros (
    id SERIAL PRIMARY KEY,
    titulo VARCHAR(255) NOT NULL,
    autor VARCHAR(255) NOT NULL,
    isbn VARCHAR(20) UNIQUE NOT NULL,
    editorial VARCHAR(255) NOT NULL,
    anio_publicacion INTEGER NOT NULL,
    materia VARCHAR(100) NOT NULL
);

-- Agregar columna para estado del libro
ALTER TABLE Libros ADD COLUMN estado_libro VARCHAR(50) DEFAULT 'disponible';

-- Inserción de datos de prueba
INSERT INTO Libros (titulo, autor, isbn, editorial, anio_publicacion, materia)
VALUES
('Ciencia de Datos', 'Juan Pérez', '978-3-16-148410-0', 'Editorial ABC', 2020, 'Ciencias de la Computación'),
('Matemáticas Avanzadas', 'María López', '978-1-234-56789-7', 'Editorial XYZ', 2019, 'Matemáticas');

-- Procedimiento almacenado para agregar un libro
CREATE OR REPLACE FUNCTION agregar_libro(
    p_titulo VARCHAR,
    p_autor VARCHAR,
    p_isbn VARCHAR,
    p_editorial VARCHAR,
    p_anio_publicacion INTEGER,
    p_materia VARCHAR
) RETURNS TEXT AS $$
BEGIN
    INSERT INTO Libros (titulo, autor, isbn, editorial, anio_publicacion, materia)
    VALUES (p_titulo, p_autor, p_isbn, p_editorial, p_anio_publicacion, p_materia);
    RETURN 'Libro agregado correctamente.';
EXCEPTION
    WHEN OTHERS THEN
        RETURN 'Error al agregar el libro: ' || SQLERRM;
END;
$$ LANGUAGE plpgsql;
