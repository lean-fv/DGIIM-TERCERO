#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <queue>
using namespace std;

struct state{
  int fil;
  int col;
  Orientacion brujula;
};


class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado

      bordes=false;

      last_action=actIDLE;
      current_state.fil=current_state.col=size;
      current_state.brujula=norte;
      girar_derecha=false;
      n_giros=0;

      objetivo =false;
      bien_situado=false;
      bikini=false;
      zapatillas=false;
      puede_recargar=false;
      
      hubo_colision=false;

      mapaNoOrientado=vector<vector<unsigned char>>(2*size,vector<unsigned char>(2*size, '?'));

      mapaFrecuencias=vector<vector<int>>(size,vector<int>(size,0));
      mapaAuxFrecuencias=vector<vector<int>>(2*size,vector<int>(2*size,0));
     
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    //Dado que todos los mapas tienen los mismos bordes, los fijamos al inicio.
    void PonerBordes();
    ////////////////////////////////////////////////////////////////////////////////////////

    /* Extiende esta version inicial donde solo se pone la componente 0
	  en la matriz; la nueva version debe poner todas las componentes
	  de terreno en funcion de la orientacion del agente*/
    void PonerTerrenoEnMatriz(const Sensores& sensores, const state& st, vector<vector<unsigned char>>& matriz);
    ////////////////////////////////////////////////////////////////////////////////////////

    /*Realiza el volcado de informacion de las matrices auxiliares a las usadas cuando se 
    esta orientado.*/
    void Volcar(const Sensores& sensores);
    ////////////////////////////////////////////////////////////////////////////////////////


    //Devuelve la posicion de una casilla especial en los sensores del agente.
    int getCasillaEspecial(char casilla,const vector<unsigned char>& terreno);
    ////////////////////////////////////////////////////////////////////////////////////////
        
    //Devuelve la posicion de la casilla con menos frecuencia.
    int getCasillaMenosFrecuente(const vector<unsigned char>& terreno);
    ////////////////////////////////////////////////////////////////////////////////////////

    
    //Devuelve las frecuencias asociadas al vector de terreno.
    vector<int> getTerrenoFrecuencias(const state& st,const vector<vector<int>>& matriz);
    ////////////////////////////////////////////////////////////////////////////////////////


    //Construye una cola de acciones a partir de un indice del sensor de terreno del agente para llegar hasta el.
    void Construye_Cola(int i);
    ////////////////////////////////////////////////////////////////////////////////////////

    //Traza una ruta a una casilla especial a la vista del agente.
    void Traza_Ruta(char casilla, const vector<unsigned char>& terreno);
    ////////////////////////////////////////////////////////////////////////////////////////

    //Traza una ruta a una casilla a la vista del agente.
    void Traza_Ruta(int pos);
    ////////////////////////////////////////////////////////////////////////////////////////

    //Vacia una cola
    void clear(queue<Action> cola_acciones);
    ////////////////////////////////////////////////////////////////////////////////////////


  private:

  const vector<char> casillas = {'K','D','X','G','B','A','S','T'}; //Vector de casillas relevantes.

  bool bordes; //Indica si se han registrado los bordes en el mapa.

  //Variables relacionadas con estados y ultimas acciones
  state current_state; //Estado actual
  Action last_action; //Accion anterior
  bool girar_derecha; //Giro cuando no se puede avanzar
  int n_giros; //Numero de giros seguidos
  const int MAX_GIROS=9; //Numeros de giros seguidos maximos
  /////////////////////////////////////////////////////////////////////////////////////////////


  //Variables relacionadas con casillas especiales
  const int FIN_ESPECIALES=4; //Posicion del vector casillas donde acaban las especiales

  queue<Action> cola_acciones; //cola de acciones a seguir para alcanzar determinada casilla
  bool objetivo; //true si el agente ve una casilla especial
  bool bien_situado; //current_state refleja posicion correcta
  bool bikini; //El agente ha cogido el bikini
  bool zapatillas; //El agente ha cogido las zapatillas
  bool puede_recargar; // Si la bateria desciende por debajo de un umbral se pone a true

  const int UMBRAL_BATERIA=1500;
  const int FIN_RECARGA=3500;
  
  /////////////////////////////////////////////////////////////////////////////////////////////


  //Variables relacionadas con el escape de muros.
  bool hubo_colision; //True si ha habido alguna colision. Bandera para escapar de barrera de muros
  /////////////////////////////////////////////////////////////////////////////////////////////

  /*Para cuando no esta orientado, guardamos en una matriz auxiliar aquello que se visita para 
  volcarlo en mapaResultado una vez se oriente.*/
  vector<vector<unsigned char>> mapaNoOrientado;
  /////////////////////////////////////////////////////////////////////////////////////////////


  //Matriz auxiliar para guardar el numero de veces que el agente pasa por las casillas.
  vector<vector<int>> mapaFrecuencias;
  vector<vector<int>> mapaAuxFrecuencias;
  const int N_INSTANTES=15; //Busca casillas menos frecuentadas cada MAX_INSTANTES instantes.
  /////////////////////////////////////////////////////////////////////////////////////////////

};
#endif
