/*5_1:Ver la descripción de la vista del catálogo USER_TABLES.*/
DESCRIBE user_tables;
--------------------------------------------------------------------------------


/*5_2: Complete la siguiente secuencia:
Crear en la cuenta una tabla cualquiera.
CREATE TABLE acceso (testigo NUMBER);
Insertar algunas tuplas de prueba.
INSERT INTO acceso VALUES(1);
INSERT INTO acceso VALUES(2);
Autorizar al usuario de tu derecha para que pueda hacer consultas sobre esa tabla.
GRANT SELECT ON acceso TO usuario_derecha;
Comprobar que se puede acceder a la tabla del usuario de la izquierda.
SELECT * FROM usuario_izquierda.acceso;
Retirar el privilegio de consulta antes concedido.
REVOKE SELECT ON acceso FROM usuario_derecha;
Autorizar ahora al usuario de la derecha para que pueda hacer consultas sobre la tabla,
pero ahora con posibilidad de que este propague ese privilegio.
GRANT SELECT ON acceso TO usuario_derecha WITH GRANT OPTION;
Propagar el privilegio concedido por el usuario de la izquierda hacia el usuario de la
derecha.
GRANT SELECT ON usuario_izquierda.acceso TO usuario_derecha;
Comprobar que se pueden acceder a las tablas del usuario de la derecha y del anterior.
SELECT * FROM usuario_izquierda.acceso;
SELECT * FROM
usuario_izquierda_del_usuario_izquierda.acceso;
Retira el privilegio antes concedido. ¿Qué ocurre con los accesos?
REVOKE SELECT ON acceso FROM usuario_derecha;
SELECT * FROM
usuario_izquierda.acceso;
SELECT * FROM usuario_izquierda_del_usuario_izquierda.acceso;*/

CREATE TABLE acceso(
    testigo NUMBER
);

INSERT INTO acceso VALUES(1);
INSERT INTO acceso VALUES(2);

GRANT SELECT ON acceso TO x7556130;

SELECT * FROM x7556130.acceso;

REVOKE SELECT ON acceso FROM x7556130;

SELECT * FROM x7556130.acceso;

GRANT SELECT ON acceso TO x7556130 WITH GRANT OPTION;

GRANT SELECT ON x7556130.acceso TO x7556130;

REVOKE SELECT ON acceso FROM x7556130;

--Los privilegios se eliminan en cascada.
--------------------------------------------------------------------------------

COMMIT;
