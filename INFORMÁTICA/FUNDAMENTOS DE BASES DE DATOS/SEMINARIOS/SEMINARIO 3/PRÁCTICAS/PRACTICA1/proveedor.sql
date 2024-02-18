
CREATE TABLE proveedor(

    codpro VARCHAR2(3) CONSTRAINT codpro_clave_primaria PRIMARY KEY,
    nompro VARCHAR2(30) CONSTRAINT nompro_no_nulo NOT NULL,
    status NUMBER CONSTRAINT status_entre_1_y_10 CHECK (status>=1 AND status<=10),
    ciudad VARCHAR2(15));
    
DESCRIBE proveedor;

COMMIT;