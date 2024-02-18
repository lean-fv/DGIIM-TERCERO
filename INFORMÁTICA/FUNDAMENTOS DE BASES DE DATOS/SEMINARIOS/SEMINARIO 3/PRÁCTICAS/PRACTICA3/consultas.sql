
//Mostrar ciudades donde hay un proyecto.
SELECT ciudad FROM proyecto;
-----------------------------------------

/*3_1:
SELECT ciudad FROM proyecto;
Comprueba el resultado de la proyección. ¿Es este conforme a lo que se obtiene
en el AR?*/

//Para que no tome los repetidos usamos DISTINCT.
SELECT DISTINCT ciudad FROM proyecto;
------------------------------------------------

//Para seleccionar todos los atributos usamos *.
SELECT * FROM proveedor;
---------------------------------------


//3_2: Mostrar codigos de los componentes de una venta. ¿Necesario usar distinct?

//No es necesario usar distinct porque el trio es clave primaria de ventas y es unico.
SELECT codpro,codpj,codpie FROM ventas;
-------------------------------------------------------------------


//Codigos de proveedores que suministran al proyecto 'J1'.
SELECT codpro 
FROM ventas
WHERE codpj='J1';
--------------------------------------------------------


//3_3: Mostrar piezas de Madrid que son grises y rojas.
SELECT *
FROM pieza
WHERE ( (ciudad='Madrid') AND (color='Rojo' OR color='Gris') );
----------------------------------------------------------------------


//3_4: Suministros cuya cantidad esta entre 200 y 300, ambos inclusive.
SELECT *
FROM ventas
WHERE (cantidad BETWEEN 200 AND 300);
---------------------------------------------------


//Mostrar nombres de proveedores cuyo nombre de ciudad empieza por 'L'.
SELECT nompro, ciudad
FROM proveedor
WHERE ciudad LIKE 'L%';
-------------------------------------------------


//3_5: Mostrar piezas que empiezan por caracter seguido de 'ornillo'.
SELECT *
FROM pieza
WHERE nompie LIKE '_ornillo';
-----------------------------------------------------------


//Describe la cantidad de cada venta expresada en docenas, docenas redondeadas y 
//truncadas al tercer decimal y aproximadas por el entero inferior y por el entero
//superior, solo de las ventas cuyo número de piezas es mayor de diez docenas.
SELECT cantidad/12, ROUND (cantidad/12,3), TRUNC(cantidad/12,3), FLOOR(cantidad/12), CEIL(cantidad/12)
FROM ventas
WHERE (cantidad/12)>10;
-----------------------------------------------------------------------------


//Encontrar los proveedores que tienen su status registrado en la base de datos.
SELECT codpro
FROM proveedor
WHERE status IS NOT NULL;
---------------------------------------------------------------


//Mostrar la información de todas las tablas denominadas ventas a las que tienes acceso.
SELECT table_name
FROM ALL_TABLES
WHERE table_name LIKE '%ventas';
---------------------------------------------------------------

/*3_6:Comprueba que no devuelve ninguna. Pero SÍ que hay!!!. Prueba a usar la función 
upper() comparando con ’VENTAS’ o la función lower() comparando con ’ventas’.*/
//lower() y upper() pasan a minusculas y mayusculas, respectivamente.
SELECT table_name
FROM ALL_TABLES
WHERE LOWER(table_name) LIKE '%ventas';

SELECT table_name
FROM ALL_TABLES
WHERE UPPER(table_name) LIKE '%VENTAS';
-------------------------------------------------------------------


//Ciudades donde viven proveedores con status mayor de 2 en las que no se fabrica la pieza ’P1’.
//Los DISTINCT no son necesarios, pues las operaciones de conjuntos estan implementadas sin repeticion, salvo UNION ALL.
(SELECT DISTINCT ciudad
FROM proveedor
WHERE status>2)

MINUS 

(SELECT DISTINCT ciudad
FROM pieza
WHERE codpie='P1');
-------------------------------------------------------------------------

//3_7:Resolver la consulta del ejemplo 3.8 utilizando el operador de interseccion.
//Ciudades donde viven proveedores con status mayor de 2 en las que no se fabrica la pieza ’P1’.

--Propuesta incorrecta
(SELECT ciudad
FROM proveedor
WHERE status>2)

INTERSECT

(SELECT ciudad
FROM pieza
WHERE codpie != 'P1');


-- propuesta correcta:
(SELECT ciudad
FROM proveedor
WHERE status>2)

INTERSECT

(SELECT p1.ciudad
FROM proveedor p1, pieza p2
WHERE codpie='P1' AND p1.ciudad!=p2.ciudad);
---------------------------------------------------------------------

//3_8:Encontrar los códigos de aquellos proyectos a los que solo abastece ’S1’.
SELECT DISTINCT codpj
FROM ventas
WHERE (codpro='S1')

MINUS 

SELECT codpj
FROM ventas
WHERE (codpro!='S1');
---------------------------------------------------------------------------


//3_9:Mostrar todas las ciudades de la base de datos. Utilizar UNION.
(SELECT ciudad FROM proveedor)
UNION
(SELECT ciudad FROM pieza)
UNION
(SELECT ciudad FROM proyecto);
--------------------------------------------------------------------


//3_10:Mostrar todas las ciudades de la base de datos. Utilizar UNION ALL.
SELECT DISTINCT ciudad
FROM(
    SELECT ciudad FROM proveedor
    UNION ALL
    SELECT ciudad FROM pieza
    UNION ALL
    SELECT ciudad FROM proyecto);
-------------------------------------------------------------------------


//3_11:Comprueba cuántas tuplas resultan del producto cartesiano aplicado a ventas y proveedor.
SELECT *
FROM ventas,proveedor;
//Son 217 tuplas.
------------------------------------------------------------


//Muestra las posibles ternas (codpro,codpie,codpj) tales que todos los implicados sean de la misma ciudad.
SELECT codpro, codpie, codpj
FROM proveedor,proyecto,pieza
WHERE proveedor.ciudad=proyecto.ciudad AND proveedor.ciudad=pieza.ciudad;
-----------------------------------------------------------------------


//Mostrar las ternas (codpro,codpie,codpj) tales que todos los implicados son de Londres.
SELECT codpro,codpie,codpj
FROM
    (SELECT * FROM proveedor WHERE ciudad='Londres'),
    (SELECT * FROM pieza WHERE ciudad='Londres'),
    (SELECT * FROM proyecto WHERE ciudad='Londres');
    
    
SELECT codpro,codpie,codpj
FROM proveedor,pro
yecto,pieza
WHERE proveedor.ciudad='Londres' AND pieza.ciudad='Londres' AND proyecto.ciudad='Londres';
----------------------------------------------------------------------


//3_12:Mostrar las ternas que son de la misma ciudad pero que hayan realizado alguna venta.

(SELECT codpro,codpie,codpj FROM ventas)

INTERSECT

(SELECT codpro,codpie,codpj
FROM proveedor,pieza,proyecto
WHERE proveedor.ciudad=pieza.ciudad AND pieza.ciudad=proyecto.ciudad);
----------------------------------------------------------------------------


//Muestra las posibles ternas (codpro,codpie,codpj) tal que todos los implicados sean de la misma ciudad.
SELECT codpro,codpie,codpj
FROM proveedor s,proyecto j,pieza p
WHERE s.ciudad=j.ciudad AND j.ciudad=p.ciudad;
-------------------------------------------------------------------------

//3_13:Encontrar parejas de proveedores que no viven en la misma ciudad.
SELECT s1.codpro, s2.codpro
FROM proveedor s1, proveedor s2
WHERE s1.ciudad<s2.ciudad;
----------------------------------------------------------------


//3_14: Encuentra las piezas con máximo peso.
(SELECT codpie FROM pieza)

MINUS

(SELECT p1.codpie
FROM pieza p1, pieza p2
WHERE p1.peso < p2.peso);
----------------------------------------------------------------


//Mostrar los nombres de proveedores y la cantidad de aquellas ventas de cantidad superior a 800 unidades.
SELECT nompro, cantidad
FROM proveedor NATURAL JOIN (SELECT * FROM ventas
                            WHERE cantidad>800);
                            
//Alternativamente

SELECT nompro, cantidad
FROM proveedor s, (SELECT * FROM ventas WHERE cantidad>800) v
WHERE s.codpro=v.codpro;

SELECT nompro, cantidad
FROM proveedor s, ventas v
WHERE s.codpro=v.codpro AND cantidad>800;

SELECT nompro, cantidad
FROM proveedor s JOIN (SELECT * FROM ventas WHERE cantidad>800) v
                ON (s.codpro=v.codpro);
-------------------------------------------------------------------------

//3_15:Mostrar las piezas vendidas por los proveedores de Madrid.
SELECT DISTINCT codpie
FROM ventas NATURAL JOIN (SELECT * FROM proveedor
                          WHERE(ciudad='Madrid'));
------------------------------------------------------------------------

/*3_16_Encuentra la ciudad y los códigos de las piezas suministradas a cualquier
proyecto por un proveedor que está en la misma ciudad donde está el proyecto.*/
SELECT DISTINCT ciudad, codpie
FROM ventas NATURAL JOIN (SELECT * 
                          FROM proveedor NATURAL JOIN(SELECT * 
                                                      FROM proyecto));
--------------------------------------------------------------------------------


//Encontrar los nombres de proveedores ordenados alfabéticamente.
SELECT nompro 
FROM proveedor
ORDER BY nompro;
--------------------------------------------------------------------------------

//3_17:Comprobar la salida de la consulta anterior sin la cláusula ORDER BY.
SELECT nompro 
FROM proveedor;
--------------------------------------------------------------------------------


/*3_18:Listar las ventas ordenadas por cantidad. Si algunas ventas coinciden en 
la cantidad se ordenan en función de la fecha de manera descendente.*/
SELECT *

FROM ventas
ORDER BY cantidad, fecha DESC;
--------------------------------------------------------------------------------


/*Encontrar las piezas suministradas por proveedores de Londres (sin usar el
operador de reunión).*/
SELECT DISTINCT codpie
FROM ventas
WHERE codpro IN
    (SELECT codpro
    FROM proveedor
    WHERE ciudad='Londres');
--------------------------------------------------------------------------------

/*3_19:Mostrar las piezas vendidas por los proveedores de Madrid (fragmentando la
consulta con ayuda del operador IN). Compara la solución con la del ejercicio 
3.15.*/
SELECT DISTINCT codpie
FROM ventas
WHERE codpro IN 
    (SELECT codpro
    FROM proveedor
    WHERE ciudad='Madrid');
--------------------------------------------------------------------------------
    
/*3_20:Encuentra los proyectos que están en una ciudad donde se fabrica alguna
pieza.*/
SELECT DISTINCT codpj
FROM proyecto
WHERE ciudad IN
    (SELECT ciudad 
    FROM pieza);
--------------------------------------------------------------------------------
    
/*3_21:Encuentra los códigos de aquellos proyectos que no utilizan ninguna pieza
roja que esté suministrada por un proveedor de Londres. */

SELECT codpj
FROM proyecto
WHERE codpj NOT IN
    (SELECT codpj
    FROM ventas v,pieza p,proveedor s
    WHERE v.codpie=p.codpie 
        AND v.codpro=s.codpro
        AND color='Rojo'
        AND s.ciudad='Londres');
        

//Alternativamente:
(SELECT codpj FROM proyecto)
MINUS
(SELECT codpj FROM ventas NATURAL JOIN(
    SELECT * FROM pieza WHERE color='Rojo')
WHERE codpro IN
    (SELECT codpro FROM proveedor WHERE ciudad='Londres'));
--------------------------------------------------------------------------------


/*Encontrar los proveedores (código y nombre) que suministran la pieza ’P1’.*/
SELECT codpro, nompro
FROM proveedor
WHERE EXISTS(
    SELECT * 
    FROM ventas
    WHERE ventas.codpro=proveedor.codpro AND ventas.codpie='P1');
--------------------------------------------------------------------------------


/*Muestra el código de los proveedores cuyo estatus sea igual al del proveedor 
’S3’.*/
SELECT codpro
FROM proveedor
WHERE status = (
    SELECT status
    FROM proveedor
    WHERE codpro='S3');
--------------------------------------------------------------------------------

    
/*Muestra el código de las piezas cuyo peso es mayor que el peso de alguna pieza
’Tornillo’.*/
SELECT codpie
FROM pieza
WHERE peso > ANY(
    SELECT peso
    FROM pieza
    WHERE nompie='Tornillo');
--------------------------------------------------------------------------------

/*3_22:Muestra el código de las piezas cuyo peso es mayor que el peso de 
cualquier ’Tornillo’.*/
SELECT codpie
FROM pieza
WHERE peso > ALL(
    SELECT peso
    FROM pieza
    WHERE nompie='Tornillo');
--------------------------------------------------------------------------------

/*3_23:Encuentra las piezas con peso máximo. Compara esta solución con la obtenida
en el ejercicio 3.14*/
SELECT codpie
FROM pieza
WHERE peso >= ALL(
    SELECT peso
    FROM pieza);
--------------------------------------------------------------------------------

/*Mostrar el código de los proveedores que suministran todas las piezas.*/

--Algebra Relacional
(SELECT DISTINCT codpro FROM ventas)

MINUS

(SELECT codpro FROM(
    (SELECT v.codpro,p.codpie
    FROM (SELECT DISTINCT codpro FROM ventas) v,
        (SELECT codpie FROM pieza) p)
    MINUS 
    (SELECT codpro,codpie FROM ventas)));
    
    
--Calculo Relacional
SELECT codpro
FROM proveedor
WHERE NOT EXISTS(
    SELECT *
    FROM pieza
    WHERE NOT EXISTS(
        SELECT *
        FROM ventas
        WHERE pieza.codpie=ventas.codpie AND proveedor.codpro=ventas.codpro));
        
--Mixto
SELECT codpro
FROM proveedor
WHERE NOT EXISTS(
    (SELECT DISTINCT codpie FROM pieza)
    MINUS
    (SELECT DISTINCT codpie
    FROM ventas
    WHERE proveedor.
    codpro=ventas.codpro));
--------------------------------------------------------------------------------

/*3_24:Encontrar los códigos de las piezas suministradas a todos los proyectos
localizados en Londres.*/

(SELECT DISTINCT codpie FROM pieza)

MINUS

(SELECT DISTINCT codpie FROM(
    (SELECT DISTINCT p.codpie, s.codpj FROM
        (SELECT DISTINCT codpie FROM pieza) p, 
        (SELECT DISTINCT codpj FROM proyecto WHERE ciudad ='Londres') s)
        
    MINUS
    
    (SELECT DISTINCT codpie,codpj FROM ventas)
));


//REVISAAARRRRRR PORQUE EN EL ANTERIOR NO SALE IGUAL
SELECT DISTINCT codpie
FROM pieza
WHERE NOT EXISTS(
    (SELECT codpJ
    FROM proyecto
    WHERE ciudad='Londres')
    
    MINUS
    
    (SELECT codpj
    FROM VENTAS
    WHERE ventas.codpie=ventas.codpie));
    
    
SELECT codpie
FROM pieza
WHERE NOT EXISTS(
    (SELECT codpj FROM proyecto WHERE ciudad='Londres')
    MINUS
    (SELECT codpj FROM ventas WHERE ventas.codpie=pieza.codpie));
    
--------------------------------------------------------------------------------

/*3_25:Encontar aquellos proveedores que envian piezas procedentes de todas las 
ciudades donde hay un proyecto.*/

SELECT codpro FROM proveedor
WHERE NOT EXISTS(
    (SELECT ciudad FROM proyecto)
    MINUS
    (SELECT ciudad FROM ventas NATURAL JOIN pieza
    WHERE proveedor.codpro=ventas.codpro));
--------------------------------------------------------------------------------

/*Mostrar el máximo, el mínimo y el total de unidades vendidas.*/
SELECT MAX(cantidad), MIN(cantidad), SUM(cantidad) FROM ventas;

SELECT MAX(DISTINCT cantidad), MIN(DISTINCT cantidad), SUM(DISTINCT cantidad) FROM ventas;
/*En esta ultima opcion, al tomar las tuplas con el mismo valor de cantidad una
unica vez, el resultado de SUM es menor.*/
--------------------------------------------------------------------------------

/*3_26:Encontrar el número de envíos con más de 1000 unidades.*/
SELECT COUNT (cantidad) -- o bien COUNT(*)
FROM ventas
WHERE cantidad>1000;

--------------------------------------------------------------------------------

/*3_27:Mostrar el máximo peso.*/
SELECT MAX(peso) FROM pieza;
--------------------------------------------------------------------------------
    

/*3_28:Mostrar el código de la pieza de máximo peso. Compara esta solución con 
las correspondientes de los ejercicios 3.14 y 3.23.*/
SELECT codpie 
FROM pieza
WHERE peso =
    (SELECT MAX(peso) FROM pieza);
--------------------------------------------------------------------------------

/*3_29 Comprueba si la siguiente sentencia resuelve el ejercicio anterior.*/
SELECT codpie, MAX(peso) FROM pieza;

/*Error pues la funcion MAX produce a lo sumo un resultado, mientras que se 
pueden mostrar varias filas de codpie. Es decir, ambas funciones no generan el
mismo numero de tuplas.*/
--------------------------------------------------------------------------------

/*3_30:Muestra los códigos de proveedores que han hecho más de 3 envíos 
diferentes.*/
SELECT codpro
FROM proveedor p
WHERE(SELECT COUNT(*) FROM ventas WHERE p.codpro=codpro) > 3;

SELECT codpro
FROM ventas
GROUP BY codpro
HAVING COUNT(*)>3;
--------------------------------------------------------------------------------

/*Para cada proveedor, mostrar la cantidad de ventas realizadas y el máximo de
unidades suministrado en una venta.*/
SELECT codpro, COUNT(*), MAX(cantidad)
FROM ventas
GROUP BY codpro;
--------------------------------------------------------------------------------

/*3_31: Mostrar la media de las cantidades vendidas por cada código de pieza 
junto con su nombre.*/
SELECT codpie, nompie, AVG(cantidad)
FROM ventas NATURAL JOIN pieza
GROUP BY codpie,nompie;
--------------------------------------------------------------------------------

/*3_32:Encontrar la cantidad media de ventas de la pieza ’P1’ realizadas por cada
proveedor.*/
SELECT codpro, AVG(cantidad)
FROM ventas
WHERE codpie='P1'
GROUP BY codpro;
--------------------------------------------------------------------------------

/*3_33:Encontrar la cantidad total de cada pieza enviada a cada proyecto.*/
SELECT codpie,codpj,SUM(cantidad)
FROM ventas
GROUP BY codpie,codpj;
--------------------------------------------------------------------------------

/*Hallar la cantidad media de ventas realizadas por cada proveedor, indicando
solamente los códigos de proveedores que han hecho más de 3 ventas.*/
SELECT codpro, AVG(cantidad)
FROM ventas
GROUP BY codpro
HAVING COUNT(*)>3;
--------------------------------------------------------------------------------

/*Mostrar la media de unidades vendidas de la pieza ’P1’ realizadas por cada
proveedor, indicando solamente la información de aquellos proveedores que han 
hecho entre 2 y 10 ventas.*/
SELECT codpro, codpie, AVG(cantidad)
FROM ventas
WHERE codpie='P1'
GROUP BY codpro,codpie
HAVING COUNT(*) BETWEEN 2 AND 10;
--------------------------------------------------------------------------------

/*Encuentra los nombres de proyectos y cantidad media de piezas que recibe por
proveedor.*/
SELECT v.codpj,j.nompj,v.codpro, AVG(v.cantidad)
FROM proyecto j, ventas v
WHERE j.codpj=v.codpj
GROUP BY v.codpj,j.nompj,v.codpro;

--Alternativamente:
SELECT codpj,nompj,codpro, AVG(cantidad)
FROM proyecto NATURAL JOIN ventas
GROUP BY codpj,nompj,codpro;
--------------------------------------------------------------------------------

/*3_34: Comprueba si es correcta la solución anterior.*/
SELECT v.codpj,j.nompj,v.codpro, AVG(v.cantidad)
FROM proyecto j, ventas v
WHERE j.codpj=v.codpj
GROUP BY v.codpj,j.nompj,v.codpro;
//Si es correcta.
--------------------------------------------------------------------------------

/*3_35:Mostrar los nombres de proveedores tales que el total de sus ventas 
superen la cantidad de 1000 unidades.*/
SELECT nompro
FROM ventas NATURAL JOIN proveedor
GROUP BY codpro,nompro
HAVING SUM(cantidad)>1000;
--------------------------------------------------------------------------------

/*Mostrar el proveedor que más ha vendido en total.*/
SELECT codpro,SUM(cantidad)
FROM ventas
GROUP BY codpro
HAVING SUM(cantidad)=
    (SELECT MAX(SUM(v.cantidad))
    FROM ventas v
    GROUP BY v.codpro);
--------------------------------------------------------------------------------

/*3_36:Mostrar la pieza que más se ha vendido en total.*/
SELECT codpie, SUM(cantidad)
FROM ventas
GROUP BY codpie
HAVING SUM(cantidad)=(
    SELECT MAX(SUM(v.cantidad))
    FROM ventas v
    GROUP BY v.codpie);
--------------------------------------------------------------------------------


/*Lista las fechas de las ventas en un formato día, mes y año con 4 dígitos.*/
SELECT TO_CHAR(fecha, 'DD-MM-YYYY')
FROM ventas;
--------------------------------------------------------------------------------

/*Encontrar las ventas realizadas entre el 1 de enero de 2002 y el 31 de 
diciembre de 2004.*/
SELECT *
FROM ventas
WHERE fecha BETWEEN TO_DATE('01/01/02','DD/MM/YY') AND TO_DATE('31/12/04','DD/MM/YY'); 
--------------------------------------------------------------------------------

/*3_37:Comprueba que no funciona correctamente si las comparaciones de fechas se
hacen con cadenas.*/
SELECT *
FROM ventas
WHERE fecha BETWEEN '01/01/02' AND '31/12/04';
--Si funciona.

SELECT *
FROM ventas
WHERE TO_CHAR(fecha) BETWEEN '01/01/02' AND '31/12/04';
--No funciona.

--------------------------------------------------------------------------------

/*Mostrar las piezas que no han sido suministradas después del año 2001.*/
(SELECT codpie FROM ventas)
MINUS
(SELECT codpie FROM ventas WHERE fecha > TO_DATE('2001','YYYY'));

--Alternativamente
(SELECT codpie FROM ventas)
MINUS
(SELECT codpie FROM ventas WHERE TO_NUMBER(TO_CHAR(fecha,'YYYY')) > 2001);

SELECT p.codpie
FROM pieza p
WHERE NOT EXISTS (
    SELECT *
    FROM ventas v
    WHERE TO_NUMBER(TO_CHAR(v.fecha,’YYYY’)) > 2001
        AND v.codpie=p.codpie);
--------------------------------------------------------------------------------

/*Agrupar los suministros de la tabla de ventas por años y sumar las cantidades
totales anuales.*/
SELECT TO_CHAR(fecha,'YYYY'), SUM(cantidad)
FROM ventas
GROUP BY TO_CHAR(fecha,'YYYY')
ORDER BY TO_CHAR(fecha,'YYYY') DESC;
--------------------------------------------------------------------------------


/*3_38: Encontrar la cantidad media de piezas suministradas cada mes.*/
SELECT TO_CHAR(fecha,'MM/YYYY'), AVG(cantidad)
FROM ventas
GROUP BY TO_CHAR(fecha,'MM/YYYY');
--------------------------------------------------------------------------------


/*Mostrar la información de todos los usuarios del sistema; la vista que nos
interesa es ALL_USERS.*/
SELECT * FROM all_users;

DESCRIBE all_users;
--------------------------------------------------------------------------------

/*Queremos saber qué índices tenemos definidos sobre nuestras tablas, pero en
esta ocasión vamos a consultar al propio catálogo para que nos muestre algunas 
de las vistas que contiene.*/
DESCRIBE DICTIONARY;

SELECT * FROM DICTIONARY WHERE table_name LIKE '%INDEX%';
--------------------------------------------------------------------------------

/*3_39: ¿ Cuál es el nombre de la vista que tienes que consultar y qué campos te
pueden interesar?*/
SELECT * FROM user_indexes;
SELECT * FROM all_users;
--------------------------------------------------------------------------------

/*3.40 Muestra las tablas ventas a las que tienes acceso de consulta junto con 
el nombre del propietario y su número de identificación en el sistema.*/
SELECT i.table_name, u.username, u.user_id
FROM user_indexes i, all_users u
WHERE (i.table_owner=u.user_id) AND (LOWER(i.table_name) LIKE '%ventas%');
--No ejecuta correctamente.
--------------------------------------------------------------------------------

/*3_41:Muestra todos tus objetos creados en el sistema. ¿Hay algo más que 
tablas?*/
SELECT * FROM DICTIONARY;
--------------------------------------------------------------------------------

/*3_42:Mostrar los códigos de aquellos proveedores que hayan superado las ventas
totales realizadas por el proveedor ’S1’.*/

SELECT codpro
FROM ventas
WHERE cantidad > (
    SELECT SUM(v.cantidad)
    FROM ventas v
    WHERE v.codpro='S1'
    GROUP BY codpro);
--------------------------------------------------------------------------------

/*3_43: Mostrar los mejores proveedores, entendiéndose como los que tienen 
mayores cantidades totales.*/

SELECT codpro, SUM(cantidad)
FROM ventas
GROUP BY codpro
HAVING SUM(cantidad) = (
    SELECT MAX(SUM(v.cantidad))
    FROM ventas v
    GROUP BY v.codpro);
--------------------------------------------------------------------------------

/*3_44: Mostrar los proveedores que venden piezas a todas las ciudades de los
proyectos a los que suministra ’S3’, sin incluirlo.*/
(SELECT codpro FROM proveedor WHERE codpro!='S3')

MINUS

(SELECT codpro
FROM(
    (SELECT s.codpro,p.ciudad 
    FROM proveedor s, proyecto p
    WHERE p.codpj IN
        (SELECT codpro FROM proveedor WHERE codpro='S3'))
    MINUS
    (SELECT codpro, ciudad FROM ventas NATURAL JOIN proyecto)
)
);
--------------------------------------------------------------------------------

/*3.45 Encontrar aquellos proveedores que hayan hecho al menos diez pedidos.*/
SELECT codpro, COUNT(*)
FROM ventas
GROUP BY codpro
HAVING COUNT(*) >= 10;
--------------------------------------------------------------------------------

/*3.46 Encontrar aquellos proveedores que venden todas las piezas suministradas
por S1.*/
SELECT codpro 
FROM proveedor
WHERE NOT EXISTS (
    (SELECT codpie FROM ventas WHERE codpro='S1')
    MINUS
    (SELECT codpie FROM ventas
    WHERE ventas.codpro=proveedor.codpro) 

);
--------------------------------------------------------------------------------

/*3_47:Encontrar la cantidad total de piezas que ha vendido cada proveedor que
cumple la condición de vender todas las piezas suministradas por S1.*/
SELECT codpro, SUM(cantidad)
FROM ventas
WHERE codpro IN(

    SELECT p.codpro 
    FROM proveedor p
    WHERE NOT EXISTS (
        (SELECT codpie FROM pieza)
        MINUS
        (SELECT codpie FROM ventas v
        WHERE v.codpro=p.codpro AND v.codpro= 'S1')
    )
)
GROUP BY codpro;
--------------------------------------------------------------------------------

/*3_48: Encontrar qué proyectos están suministrados por todos lo proveedores que
suministran la pieza P3.*/
SELECT DISTINCT codpj
FROM ventas v
WHERE NOT EXISTS(
    (SELECT DISTINCT codpro FROM ventas WHERE codpie='P3' )
    MINUS
    (SELECT DISTINCT codpro FROM ventas 
    WHERE ventas.codpj=v.codpj)
);
--------------------------------------------------------------------------------

/*3_49:Encontrar la cantidad media de piezas suministrada a aquellos proveedores
que venden la pieza P3.*/
SELECT AVG(cantidad)
FROM ventas
WHERE codpie='P3';
--------------------------------------------------------------------------------

/*3_50: Queremos saber los nombres de tus índices y sobre qué tablas están 
montados. Indica además su propietario.*/
SELECT index_name, table_name,table_owner FROM user_indexes;
--------------------------------------------------------------------------------

/*3_51:Implementar el comando DESCRIBE para tu tabla ventas a través de una
consulta a las vistas del catálogo.*/
SELECT * FROM user_indexes; --???
--------------------------------------------------------------------------------

/*3_52: Mostrar para cada proveedor la media de productos suministrados cada 
año.*/
SELECT codpro, TO_CHAR(fecha,'YYYY'),AVG(cantidad)
FROM ventas
GROUP BY codpro, TO_CHAR(fecha,'YYYY')
ORDER BY codpro, TO_CHAR(fecha,'YYYY');
--------------------------------------------------------------------------------

/*3_53: Encontrar todos los proveedores que venden una pieza roja.*/
SELECT DISTINCT codpro
FROM ventas NATURAL JOIN pieza
WHERE color='Rojo';
--------------------------------------------------------------------------------

/*3_54: Encontrar todos los proveedores que venden todas las piezas rojas.*/
SELECT codpro FROM ventas
WHERE NOT EXISTS(
    (SELECT DISTINCT codpie FROM pieza WHERE color ='Rojo')
    MINUS
    (SELECT DISTINCT codpie FROM ventas v 
    WHERE ventas.codpro=v.codpro)
);
--------------------------------------------------------------------------------

/*3_55: Encontrar todos los proveedores tales que todas las piezas que venden 
son rojas.*/
(SELECT DISTINCT codpro FROM ventas)
MINUS
(SELECT codpro FROM ventas NATURAL JOIN pieza WHERE color !='Rojo');
--------------------------------------------------------------------------------

/*3_56: Encontrar el nombre de aquellos proveedores que venden más de una pieza
roja.*/
SELECT codpro, count(*)
FROM ventas NATURAL JOIN pieza
WHERE color='Rojo'
GROUP BY codpro
HAVING COUNT(*) > 1;
--------------------------------------------------------------------------------

/*3_57: Encontrar todos los proveedores que vendiendo todas las piezas rojas 
cumplen la condición de que todas sus ventas son de más de 10 unidades.*/
SELECT codpro 
FROM proveedor
WHERE codpro NOT IN (SELECT codpro FROM ventas WHERE cantidad<=10)
    AND codpro NOT IN (SELECT codpro FROM(
                        (SELECT codpro,codpie FROM proveedor,pieza WHERE color='Rojo')
                        MINUS
                        (SELECT codpro, codpie FROM ventas)
                        ));
--------------------------------------------------------------------------------

/*3_58:Coloca el status igual a 1 a aquellos proveedores que solo suministran la
pieza P1.*/
UPDATE proveedor SET status=1
WHERE codpro IN(
    (SELECT DISTINCT codpro FROM ventas)
    MINUS
    (SELECT DISTINCT codpro FROM ventas WHERE codpie!='P1')
);
--------------------------------------------------------------------------------

/*3_59: Encuentra, de entre las piezas que no se han vendido en septiembre de 
2009, las ciudades de aquéllas que se han vendido en mayor cantidad durante 
Agosto de ese mismo año.*/

--No correcto: revisar
SELECT ciudad, SUM(cantidad)
FROM ventas NATURAL JOIN pieza
WHERE fecha != TO_DATE('Sep/2009','MON/YYYY')
GROUP BY codpie,ciudad
HAVING SUM(cantidad) = (
    SELECT MAX(SUM(cantidad)) 
    FROM ventas NATURAL JOIN (
        SELECT codpie FROM pieza WHERE fecha != TO_DATE('Sep/2009','mm/YYYY')
    )
    WHERE fecha = TO_DATE('Aug/2009','mm/YYYY')
    GROUP BY codpie,ciudad
);
--------------------------------------------------------------------------------

COMMIT;


