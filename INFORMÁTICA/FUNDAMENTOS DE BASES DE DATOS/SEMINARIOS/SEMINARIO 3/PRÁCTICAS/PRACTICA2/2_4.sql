
UPDATE plantilla
    SET fechaalta=fechaalta+1
    WHERE nombre='Juan';
    
SELECT * FROM plantilla;

INSERT INTO plantilla VALUES ('11223355','Miguel','casado',TO_DATE('22/10/2005','dd/mm/yyyy'),null);


SELECT TO_CHAR(fechaalta,'dd-mon-yyyy') FROM plantilla;
SELECT fechaalta FROM plantilla;



SELECT * FROM proveedor;
SELECT * FROM pieza;
SELECT * FROM proyecto;
SELECT * FROM ventas;
SELECT * FROM medina.ventas;

//INSERT INTO ventas SELECT * FROM medina.ventas;


//2_4

//Error pues ya existe una tupla con esa clave primaria.
//INSERT INTO ventas VALUES ('S3','P1','J1',150,'24/12/05'); 

//Error pues el codigo de la pieza no puede ser nulo.
//INSERT INTO ventas (codpro,codpj) VALUES ('S4','J2');

//Error pues no existe el proyecto J6, una clave externa debe referenciar una clave existente.
//INSERT INTO ventas VALUES ('S5','P3','J6',400,TO_DATE('25/12/00'));

COMMIT;