CREATE TABLE serjefe(

    dnijefe REFERENCES plantilla(dni),
    dnitrabajador REFERENCES plantilla(dni),
    PRIMARY KEY (dnitrabajador));
    
DESCRIBE serjefe;
    
COMMIT;