
//2_5

UPDATE ventas
    SET fecha=TO_DATE(2005,'yyyy')
    WHERE codpro='S5';
    
SELECT codpro, fecha FROM ventas;

COMMIT;