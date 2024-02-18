
/*Si queremos acelerar las consultas cuando busquemos a un proveedor por su
nombre podemos crear un índice asociado al campo nompro de la tabla proveedor.*/
CREATE INDEX indice_proveedores ON proveedor(nompro);
--------------------------------------------------------------------------------

/*Podemos comprobar la creación de este índice mediante la siguiente consulta al
catálogo.*/
SELECT * FROM user_indexes WHERE index_name='INDICE_PROVEEDORES';
DROP INDEX indice_proveedores;
--------------------------------------------------------------------------------
/*Ejemplo de indice por clave invertida.*/
CREATE INDEX idx_inv ON proveedor(ciudad) REVERSE;
DROP INDEX idx_inv;
--------------------------------------------------------------------------------

/*Ejemplo de uso BITMAP:*/
CREATE TABLE Prueba_Bit(color VARCHAR2(10));

BEGIN
    FOR i IN 1..50000 LOOP
        INSERT INTO Prueba_bit (
        SELECT decode(round(dbms_random.value(1,4)),1,'Rojo',2,'Verde',
        3,'Amarillo',4,'Azul') FROM dual);
    END LOOP;
END;

CREATE INDEX Prueba_IDX ON Prueba_Bit(color);

SELECT COUNT(*) FROM Prueba_Bit 
WHERE color='Amarillo' OR color='Azul';

DROP INDEX Prueba_IDX;

CREATE BITMAP INDEX Prueba_BITMAP_IDX ON Prueba_Bit(color);

SELECT COUNT(*) FROM Prueba_Bit 
WHERE color='Amarillo' OR color='Azul';

DROP TABLE Prueba_Bit;
--------------------------------------------------------------------------------

/*Ejemplo IOT:*/
CREATE TABLE Prueba_IOT (id NUMBER PRIMARY KEY) ORGANIZATION INDEX;

SELECT id FROM Prueba_IOT;

DROP TABLE Prueba_IOT;
--------------------------------------------------------------------------------

/*Para crear el “cluster” mostrado en la figura 6.1, usamos la siguiente 
sentencia.*/
CREATE CLUSTER cluster_codpro(codpro VARCHAR2(3));

CREATE TABLE proveedor2(
    codpro VARCHAR2(3) PRIMARY KEY,
    nompro VARCHAR2(30) NOT NULL,
    status NUMBER(2) CHECK(status>=1 AND status<=10),
    ciudad VARCHAR2(15))
    CLUSTER cluster_codpro(codpro);
    
CREATE TABLE ventas2(
    codpro VARCHAR2(3) REFERENCES proveedor2(codpro),
    codpie REFERENCES pieza(codpie),
    codpj REFERENCES proyecto(codpj),
    cantidad NUMBER(4),
    fecha DATE,
    PRIMARY KEY (codpro,codpie,codpj))
    CLUSTER cluster_codpro(codpro);
    
    
CREATE INDEX indice_cluster ON CLUSTER cluster_codpro;
--------------------------------------------------------------------------------


/*6_1: Rellena las tablas proveedor2 y ventas2 con los datos de sus homólogas
originales.*/
INSERT INTO proveedor2 (SELECT * FROM proveedor);
INSERT INTO ventas2 (SELECT * FROM ventas);
--------------------------------------------------------------------------------

/*6_2: Realiza alguna consulta a los datos contenidos en el “cluster” 
cluster_codpro.*/
SELECT DISTINCT codpro
FROM ventas2 NATURAL JOIN proveedor2;
--------------------------------------------------------------------------------

/*6_3:Consulta en el catálogo los objetos recién creados.*/
SELECT * FROM user_clusters;
--------------------------------------------------------------------------------


/*Crear el “cluster” mostrado en la figura 6.1, suponiendo que vamos a tener unos
50 proveedores y una media de 30 ventas por proveedor. ?
En este ejemplo, nuevamente la clave del “cluster” sería codpro y, puesto que va a tener unos
50 valores distintos, HASHKEYS sería 50. El valor de SIZE lo estimaríamos, en función del
tamaño de las tuplas de la tabla proveedor y de la tabla ventas de la figura 6.1 y de la cantidad
media de ventas por valor de la clave (30), de esta manera1:

Tamaño de tupla de proveedor: codpro=3 bytes, nompro=30 bytes, status=2 bytes y
ciudad=15 bytes. Total=50 bytes.

Tamaño de tupla de ventas; codpie=3 bytes, codpj=3 bytes, cantidad=3 bytes y fecha=7
bytes. Total=16 bytes.

Como por cada proveedor (tamaño 50 bytes) tendremos una media de 30 ventas (tamaño 16
bytes por tupla). Entonces el valor de SIZE sería: 50+30*16=530 bytes a lo que añadimos
un 15%, quedando: SIZE 610*/

CREATE CLUSTER cluster_codpro_hash(codpro VARCHAR2(3) codpro) 
SIZE 610 
HASHKEYS 50;
--------------------------------------------------------------------------------


/*Crear la estructura necesaria para que la tabla proveedor sea accedida mediante
“hashing” a través del campo codpro, suponiendo que va a haber un máximo de 100 
proveedores.*/
CREATE CLUSTER cluster_codpro_single_hash(codpro VARCHAR2(3)) 
SIZE 50 
SINGLE TABLE
HASHKEYS 100;

CREATE TABLE proveedor_hash(
    codpro VARCHAR2(3) PRIMARY KEY,
    nompro VARCHAR2(30) NOT NULL,
    status NUMBER(2) CHECK(status>=1 AND status<=10),
    ciudad VARCHAR2(15))
    CLUSTER cluster_codpro_single_hash(codpro);

--Borrar cluster:
DROP CLUSTER cluster_codpro_single_hash INCLUDING TABLES CASCADE CONSTRAINTS;
--------------------------------------------------------------------------------


COMMIT;