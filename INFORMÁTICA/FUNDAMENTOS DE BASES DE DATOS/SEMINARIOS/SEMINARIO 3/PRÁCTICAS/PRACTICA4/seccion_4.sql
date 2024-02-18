/*Elaborar una vista con el conjunto de suministros realizados solo con 
integrantes procedentes de Paris.*/
CREATE VIEW VentasParis(codpro,codpie,codpj,cantidad,fecha) AS
    
    SELECT codpro, codpie,codpj,cantidad,fecha
    FROM ventas
    WHERE (codpro,codpie,codpj) IN
        (SELECT codpro,codpie,codpj
        FROM proveedor,proyecto,pieza
        WHERE proveedor.ciudad= 'Paris' AND proyecto.ciudad ='Paris' 
            AND pieza.ciudad='Paris');
--------------------------------------------------------------------------------

/*Consultar la relación VentasParis y de ella mostrar los códigos de proveedores 
que suministran al proyecto J4.*/
SELECT DISTINCT codpro FROM VentasParis WHERE codpj='J4';
--------------------------------------------------------------------------------

/*Elaborar una vista con el conjunto de piezas procedentes de Londres,
prescindiendo del atributo ciudad de la tabla original.*/
CREATE VIEW PiezasLondres AS
    SELECT codpie,nompie,color,peso
    FROM pieza
    WHERE pieza.ciudad='Londres';
    
INSERT INTO PiezasLondres VALUES('P9','Pieza 9', 'rojo',90);
SELECT * FROM PiezasLondres;
--------------------------------------------------------------------------------

/*4_1:Crear una vista con los proveedores de Londres. ¿Qué sucede si insertamos
en dicha vista la tupla (’S7’,’Jose Suarez’,3,’Granada’)?. (Buscar en [3] la 
cláusula WITH CHECK OPTION ).*/
CREATE VIEW ProveedoresLondres (codpro,nompro,status) AS
    SELECT codpro,nompro,status
    FROM proveedor
    WHERE ciudad='Londres';

--Error: demasiados Valores:
INSERT INTO ProveedoresLondres VALUES ('S7','Jose Suarez',3,'Granada');

CREATE OR REPLACE VIEW ProveedoresLondres (codpro,nompro,status,ciudad) AS
    SELECT codpro,nompro,status,ciudad
    FROM proveedor
    WHERE ciudad='Londres';
    
INSERT INTO ProveedoresLondres VALUES ('S8','Jose Suarez',3,'Granada');
/*Se inserta en proveedor, pero no en la vista pues la condición de que la 
ciudad sea Londres no se cumple.*/
SELECT * FROM ProveedoresLondres;
SELECT * FROM proveedor;
--------------------------------------------------------------------------------

/*4_2:Crear una vista con los nombres de los proveedores y sus ciudades. Inserta
sobre ella una tupla y explica cuál es el problema que se plantea. ¿Habría 
problemas de actualización?*/
CREATE VIEW ProvCiu (codpro,ciudad) AS
    SELECT codpro,ciudad
    FROM proveedor;
    
INSERT INTO ProvCiu VALUES('S9','Granada');
--Error pues nompro no puede ser nulo.

CREATE OR REPLACE VIEW  ProvCiu (codpro,nompro,ciudad) AS
    SELECT codpro,nompro,ciudad
    FROM proveedor;
    
INSERT INTO ProvCiu VALUES('S9','Leandro','Granada');
--OK
--------------------------------------------------------------------------------

/*4_3:Crear una vista donde aparezcan el código de proveedor, el nombre de 
proveedor y el código del proyecto tales que la pieza sumistrada sea gris. Sobre 
esta vista realiza alguna consulta y enumera todos los motivos por los que sería 
imposible realizar una inserción.*/

CREATE VIEW mezcla(codpro,nompro,codpj) AS
    SELECT codpro,nompro,codpj
    FROM ventas NATURAL JOIN proveedor NATURAL JOIN pieza
    WHERE color='Gris';
    
--Codpie toma un valor nulo.
--No se pueden usar inserciones si hay reuniones naturales.
--------------------------------------------------------------------------------

COMMIT;