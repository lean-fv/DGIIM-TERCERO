CREATE TABLE plantilla(

    dni varchar2(9) PRIMARY KEY,
    nombre varchar2(15),
    estadocivil varchar2(10) CHECK(estadocivil IN ('soltero','casado','divorciado','viudo')),
    fechaalta date);

DESCRIBE plantilla;

COMMIT;