#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <list>


//Nivel 0

//Definicion del estado de un agente del nivel 0
struct stateN0{

  ubicacion jugador;
  ubicacion colaborador;
  Action ultimaOrdenColaborador;

  bool operator==(const stateN0& x) const{

    return (jugador==x.jugador);
  }

  bool operator<(const stateN0& x) const {

    return (jugador.f < x.jugador.f) 
          || (jugador.f == x.jugador.f && jugador.c < x.jugador.c) 
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula < x.jugador.brujula);
  }

};

//Definición del tipo nodo del nivel 0
struct nodeN0{
	stateN0 st;
	list<Action> secuencia;

	bool operator==(const nodeN0 &n) const {return (st == n.st);}
	bool operator<(const nodeN0 &n)  const {return (st<n.st);}
};

////////////////////////////////////////////////////////////////////////////////////


//Nivel 1

//Definicion del estado de un agente del nivel 1
struct stateN1{

  ubicacion jugador;
  ubicacion colaborador;
  Action ultimaOrdenColaborador;

  stateN1(){}
  stateN1(const stateN0& st){
    jugador=st.jugador;
    colaborador=st.colaborador;
  }

  bool operator==(const stateN1& x) const{

    return (jugador==x.jugador && colaborador==x.colaborador && ultimaOrdenColaborador==x.ultimaOrdenColaborador);
  }

  bool operator<(const stateN1& x) const{

    return (jugador.f < x.jugador.f) 
          || (jugador.f == x.jugador.f && jugador.c < x.jugador.c) 
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula < x.jugador.brujula)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f < x.colaborador.f)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c < x.colaborador.c)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && colaborador.brujula < x.colaborador.brujula)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && colaborador.brujula == x.colaborador.brujula && ultimaOrdenColaborador < x.ultimaOrdenColaborador);
  }
};

//Definición del tipo nodo del nivel 1
struct nodeN1{
	stateN1 st;
	list<Action> secuencia;

	bool operator==(const nodeN1 &n) const {return (st == n.st);}
	bool operator<(const nodeN1 &n)  const {return (st<n.st);}
};

////////////////////////////////////////////////////////////////////////////////////


//Definicion del estado de un agente del nivel 2
struct stateN2{

  ubicacion jugador;
  ubicacion colaborador;
  Action ultimaOrdenColaborador;
  bool bikini;
  bool zapatillas;

  stateN2(){}
  stateN2(const stateN0& st){
    jugador=st.jugador;
    colaborador=st.colaborador;
    bikini=false;
    zapatillas=false;
  }

  bool operator==(const stateN2& x) const{

    return (jugador==x.jugador && bikini==x.bikini && zapatillas==x.zapatillas);
  }

  bool operator<(const stateN2& x) const{

    return (jugador.f < x.jugador.f) 
          || (jugador.f == x.jugador.f && jugador.c < x.jugador.c) 
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula < x.jugador.brujula)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && bikini < x.bikini) 
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && bikini == x.bikini && zapatillas < x.zapatillas);
  }
};

//Definición del tipo nodo del nivel 2
struct nodeN2{
  stateN2 st;
  list<Action> secuencia;
  int bateria;

  bool operator==(const nodeN2 &n) const {return (st == n.st);}
  bool operator<(const nodeN2 &n)  const {return (st<n.st);}
  bool operator>(const nodeN2 &n)  const {return (bateria>n.bateria);}
};

class comparadorN2{
      public:
        bool operator()(const nodeN2 &n1, const nodeN2 &n2) const{
          return (n1>n2);
        }
};

////////////////////////////////////////////////////////////////////////////////////

//Definicion del estado de un agente del nivel 3
struct stateN3{

  ubicacion jugador;
  ubicacion colaborador;
  Action ultimaOrdenColaborador;
  bool bikini;
  bool zapatillas;
  bool bikini_clb;
  bool zapatillas_clb;

  stateN3(){}
  stateN3(const stateN0& st){
    jugador=st.jugador;
    colaborador=st.colaborador;
    bikini=false;
    zapatillas=false;
    bikini_clb=false;
    zapatillas_clb=false;
  }

  bool operator==(const stateN3& x) const{

    return (jugador==x.jugador && colaborador==x.colaborador && ultimaOrdenColaborador==x.ultimaOrdenColaborador && bikini==x.bikini && zapatillas==x.zapatillas && bikini_clb==x.bikini_clb && zapatillas_clb==x.zapatillas_clb);
  }

  bool operator<(const stateN3& x) const{

    return (jugador.f < x.jugador.f) 
          || (jugador.f == x.jugador.f && jugador.c < x.jugador.c) 
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula < x.jugador.brujula)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f < x.colaborador.f)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c < x.colaborador.c)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && colaborador.brujula < x.colaborador.brujula)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && colaborador.brujula == x.colaborador.brujula && ultimaOrdenColaborador < x.ultimaOrdenColaborador)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && colaborador.brujula == x.colaborador.brujula && ultimaOrdenColaborador == x.ultimaOrdenColaborador && bikini < x.bikini)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && colaborador.brujula == x.colaborador.brujula && ultimaOrdenColaborador == x.ultimaOrdenColaborador && bikini == x.bikini && zapatillas < x.zapatillas)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && colaborador.brujula == x.colaborador.brujula && ultimaOrdenColaborador == x.ultimaOrdenColaborador && bikini == x.bikini && zapatillas == x.zapatillas && bikini_clb < x.bikini_clb)
          || (jugador.f == x.jugador.f && jugador.c == x.jugador.c && jugador.brujula == x.jugador.brujula && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && colaborador.brujula == x.colaborador.brujula && ultimaOrdenColaborador == x.ultimaOrdenColaborador && bikini == x.bikini && zapatillas == x.zapatillas && bikini_clb == x.bikini_clb && zapatillas_clb < x.zapatillas_clb);
  }
};

//Definición del tipo nodo del nivel 3
struct nodeN3{
  stateN3 st;
  list<Action> secuencia;
  int bateria;
  int bateria_heuristica;

  bool operator==(const nodeN3 &n) const {return (st == n.st);}
  bool operator<(const nodeN3 &n)  const {return (st<n.st);}
  bool operator>(const nodeN3 &n)  const {return (bateria+bateria_heuristica>n.bateria+n.bateria_heuristica);}
};

class comparadorN3{
      public:
        bool operator()(const nodeN3 &n1, const nodeN3 &n2) const{
          return (n1>n2);
        }
};

////////////////////////////////////////////////////////////////////////////////////

class ComportamientoJugador : public Comportamiento {
  public:

    //Contructor nivel 4
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
    }

    //Constructor niveles 0-3
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hayPlan=false;

    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    //Permite pintar sobre el mapa del simulador el plan partiendo desde el estado st
    void VisualizaPlan(const stateN0 &st, const list<Action> &plan);


  private:
    // Declarar Variables de Estado
    list<Action> plan; //Plan en ejecucion
    bool hayPlan; //Si se esta siguiendo un plan
    ubicacion goal; //Casilla objetivo

    //Nivel 0
    stateN0 c_state; //Estado actual del agente en el nivel 0

};

#endif
