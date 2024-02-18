
//1_7

CREATE TABLE Equipos(
    
    codE VARCHAR2(10) PRIMARY KEY,
    nombreE VARCHAR2(15) UNIQUE NOT NULL,
    localidad VARCHAR2(15) NOT NULL,
    entrenador VARCHAR2(15) NOT NULL,
    fecha_crea DATE NOT NULL);
    
DESCRIBE Equipos;

    
    
CREATE TABLE Jugadores(

    codJ VARCHAR2(10) PRIMARY KEY,
    codE REFERENCES Equipos(codE),
    nombreJ VARCHAR2(15));
    
DESCRIBE Jugadores;



CREATE TABLE Encuentros(

    ELocal,
    EVisitante,
    fecha DATE,
    PLocal INT DEFAULT 0 CHECK (PLocal>=0) ,
    PVisitante INT DEFAULT 0 CHECK (PVisitante>=0),
    
    PRIMARY KEY(ELocal,Evisitante),
    FOREIGN KEY (ELocal) REFERENCES Equipos(codE),
    FOREIGN KEY (EVisitante) REFERENCES Equipos(codE));
    
DESCRIBE Encuentros;



CREATE TABLE Faltas(

    codJ,
    ELocal,
    EVisitante,
    numero INT DEFAULT 0 CHECK (numero BETWEEN 0 AND 5),
    
    PRIMARY KEY (codJ,ELocal,EVisitante),
    FOREIGN KEY(codJ) REFERENCES Jugadores(codJ),
    FOREIGN KEY (ELocal,EVisitante) REFERENCES Encuentros(ELocal,EVisitante));
    
DESCRIBE Faltas;



COMMIT;
    
    
    
    