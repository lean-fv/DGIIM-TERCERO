
//2_7

DROP TABLE Faltas;
DROP TABLE Encuentros;
DROP TABLE Jugadores;
DROP TABLE Equipos;


CREATE TABLE Equipos(
    
    codE VARCHAR2(10) PRIMARY KEY,
    nombreE VARCHAR2(15) UNIQUE NOT NULL,
    localidad VARCHAR2(15) NOT NULL,
    entrenador VARCHAR2(15) NOT NULL,
    fecha_crea DATE NOT NULL);
    
    
CREATE TABLE Jugadores(

    codJ VARCHAR2(10) PRIMARY KEY,
    codE REFERENCES Equipos(codE),
    nombreJ VARCHAR2(15));


CREATE TABLE Encuentros(

    ELocal,
    EVisitante,
    fecha DATE,
    PLocal INT DEFAULT 0 CHECK (PLocal>=0) ,
    PVisitante INT DEFAULT 0 CHECK (PVisitante>=0),
    
    PRIMARY KEY(ELocal,Evisitante),
    FOREIGN KEY (ELocal) REFERENCES Equipos(codE),
    FOREIGN KEY (EVisitante) REFERENCES Equipos(codE));


CREATE TABLE Faltas(

    codJ,
    ELocal,
    EVisitante,
    numero INT DEFAULT 0 CHECK (numero BETWEEN 0 AND 5),
    
    PRIMARY KEY (codJ,ELocal,EVisitante),
    FOREIGN KEY(codJ) REFERENCES Jugadores(codJ),
    FOREIGN KEY (ELocal,EVisitante) REFERENCES Encuentros(ELocal,EVisitante));
    
    
    
//Equipos

INSERT INTO Equipos VALUES ('1111','Equipo 1','Granada','Pepelu','01/12/2023');
INSERT INTO Equipos VALUES ('2222','Equipo 2','Granada','Pepelu','01/12/2023');
INSERT INTO Equipos VALUES ('3333','Equipo 3','Granada','Pepelu','01/12/2023');
INSERT INTO Equipos VALUES ('4444','Equipo 4','Granada','Pepelu','01/12/2023');


//Jugadores de cada equipo

INSERT INTO Jugadores VALUES('11','1111','Jugador 1_1');
INSERT INTO Jugadores VALUES('12','1111','Jugador 1_2');
INSERT INTO Jugadores VALUES('13','1111','Jugador 1_3');
INSERT INTO Jugadores VALUES('14','1111','Jugador 1_4');
INSERT INTO Jugadores VALUES('15','1111','Jugador 1_5');


INSERT INTO Jugadores VALUES('21','2222','Jugador 2_1');
INSERT INTO Jugadores VALUES('22','2222','Jugador 2_2');
INSERT INTO Jugadores VALUES('23','2222','Jugador 2_3');
INSERT INTO Jugadores VALUES('24','2222','Jugador 2_4');
INSERT INTO Jugadores VALUES('25','2222','Jugador 2_5');


INSERT INTO Jugadores VALUES('31','3333','Jugador 3_1');
INSERT INTO Jugadores VALUES('32','3333','Jugador 3_2');
INSERT INTO Jugadores VALUES('33','3333','Jugador 3_3');
INSERT INTO Jugadores VALUES('34','3333','Jugador 3_4');
INSERT INTO Jugadores VALUES('35','3333','Jugador 3_5');


INSERT INTO Jugadores VALUES('41','4444','Jugador 4_1');
INSERT INTO Jugadores VALUES('42','4444','Jugador 4_2');
INSERT INTO Jugadores VALUES('43','4444','Jugador 4_3');
INSERT INTO Jugadores VALUES('44','4444','Jugador 4_4');
INSERT INTO Jugadores VALUES('45','4444','Jugador 4_5');


//Encuentros

INSERT INTO Encuentros VALUES ('1111','2222','10/12/2023','50',DEFAULT);
INSERT INTO Encuentros VALUES ('1111','3333','11/12/2023','45',DEFAULT);
INSERT INTO Encuentros VALUES ('1111','4444','12/12/2023','40',DEFAULT);
INSERT INTO Encuentros VALUES ('2222','1111','13/12/2023',DEFAULT,'50');
INSERT INTO Encuentros VALUES ('2222','3333','14/12/2023','30',DEFAULT);
INSERT INTO Encuentros VALUES ('2222','4444','15/12/2023','25',DEFAULT);
INSERT INTO Encuentros VALUES ('3333','1111','16/12/2023',DEFAULT,'20');
INSERT INTO Encuentros VALUES ('3333','2222','17/12/2023','15',DEFAULT);
INSERT INTO Encuentros VALUES ('3333','4444','18/12/2023','10',DEFAULT);
INSERT INTO Encuentros VALUES ('4444','1111','19/12/2023',DEFAULT,'45');

COMMIT;