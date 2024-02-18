
UPDATE plantilla
    SET estadocivil='divorciado'
    WHERE nombre='Juan';
    
    
//2_2

UPDATE plantilla
    SET nombre='Luis'
    WHERE dni='12345678';
    
SELECT * FROM plantilla;

COMMIT;