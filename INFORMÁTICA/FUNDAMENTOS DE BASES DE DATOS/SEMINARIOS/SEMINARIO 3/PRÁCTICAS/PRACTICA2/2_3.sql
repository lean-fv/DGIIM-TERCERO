
DELETE FROM prueba2;
SELECT * FROM prueba2;


//2_3

//La clave primaria no puede quedar nula, por lo que se viola restriccion y no se puede eliminar.
//DELETE FROM plantilla; 

//Permite pues la clave primaria esta formada por referencias a otras claves.
DELETE FROM serjefe;
SELECT * FROM serjefe;

COMMIT;

