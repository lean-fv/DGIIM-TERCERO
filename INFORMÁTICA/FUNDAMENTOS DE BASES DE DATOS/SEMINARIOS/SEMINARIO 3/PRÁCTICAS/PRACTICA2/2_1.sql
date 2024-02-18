
INSERT INTO prueba1 VALUES ('aa',1,1.1);
INSERT INTO prueba1 VALUES ('Aa',2,2.1);
INSERT INTO prueba1 VALUES ('aa',1,1.1);


CREATE TABLE prueba2(
    cad char(3),
    n int,
    x float);
    
INSERT INTO prueba2 SELECT * FROM prueba1;

SELECT * FROM prueba2;
    


INSERT INTO plantilla (dni,nombre,estadocivil,fechaalta) VALUES ('12345678','Pepe','soltero',SYSDATE);
INSERT INTO plantilla (dni,nombre,estadocivil,fechaalta) VALUES ('87654321','Juan','casado',SYSDATE);
INSERT INTO serjefe VALUES ('87654321','12345678');
INSERT INTO plantilla (dni,estadocivil) VALUES ('11223344','soltero');

SELECT table_name FROM user_tables;

//2_1

SELECT * FROM prueba1;
SELECT cad, n FROM prueba1;

SELECT * FROM plantilla;
SELECT dni, nombre,estadocivil FROM plantilla;


COMMIT;

