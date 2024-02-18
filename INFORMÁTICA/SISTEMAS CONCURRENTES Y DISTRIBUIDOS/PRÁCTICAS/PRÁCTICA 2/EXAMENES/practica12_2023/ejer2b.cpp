//Nombre y Apellidos: Leandro Jorge Fernández Vega      DNI: 77556130S

#include <iostream>
#include <iomanip>
#include <cassert>
#include <random>
#include <thread>
#include "scd.h"

using namespace std ;
using namespace scd ;

const int num_fumadores = 3;


//-------------------------------------------------------------------------
// Función que simula la acción de producir un ingrediente, como un retardo
// aleatorio de la hebra (devuelve número de ingrediente producido)

int producir_ingrediente()
{
   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_produ( aleatorio<10,100>() );

   // informa de que comienza a producir
   cout << "Estanquero : empieza a producir ingrediente (" << duracion_produ.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_produ' milisegundos
   this_thread::sleep_for( duracion_produ );

   const int num_ingrediente = aleatorio<0,num_fumadores-1>() ;

   // informa de que ha terminado de producir
   cout << "Estanquero : termina de producir ingrediente " << num_ingrediente << endl;

   return num_ingrediente ;
}


//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra
void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

void Beber( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_beber( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a beber (" << duracion_beber.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_beber );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de beber." << endl;
}



// *****************************************************************************
// clase para monitor que regula la entrega de ingredientes y el fumar.

class Estanco : public HoareMonitor {

    private:

    unsigned ingrediente;

    CondVar
            ingr_disponible[num_fumadores],
            mostr_libre;

    public:

    Estanco (); //constructor
    void ObtenerIngrediente(int i); //fumador i obtiene su ingrediente para fumar.
    void ponerIngrediente(int i);   // estanco pone el ingrediente para el fumador i en su mostrador.
    void esperarRecogidaIngrediente(); // estanco espera a que el ingrediente puesto en el mostrador sea retirado por un fumador.
};

Estanco::Estanco (){

   ingrediente=-1;
   mostr_libre= newCondVar();

   for (int i=0; i < num_fumadores; i++) ingr_disponible[i]=newCondVar();

}

void Estanco::ObtenerIngrediente(int i){

   if (i!= ingrediente) ingr_disponible[i].wait();

   ingrediente=-1;

   cout << "Retirando ingrediente " << i << endl;

   mostr_libre.signal();

}

void Estanco::ponerIngrediente(int i){

   ingrediente=i;

   cout << "Puesto ingrediente " << i << endl;

   if (!ingr_disponible[i].empty()) ingr_disponible[i].signal();

}

void Estanco::esperarRecogidaIngrediente(){

   if (ingrediente!=-1){

      cout << "Esperando a que se recoja ingrediente." << endl;
      mostr_libre.wait();
   }

}

class Bar : public HoareMonitor {

    private:

    int total_cervezas; //Indica total de cervezas disponibles para consumir.
    bool bar_libre; //Indica posibilidad de entrar al bar.
    bool hay_cerveza; //Indica si se puede coger cerveza.

    CondVar
            bar_vacio,
            cerveza_disponible;

    public:

    Bar (); //constructor
    void cogerCerveza(int i); //fumador i obtiene su cerveza para beber.
    void ponerCervezas(); //camarero pone cervezas.
};


Bar::Bar(){

    total_cervezas=0;
    bar_libre=true;
    hay_cerveza=false;

    bar_vacio=newCondVar();
    cerveza_disponible=newCondVar();

}

void Bar::cogerCerveza(int num_fumador){

    if(!bar_libre) bar_vacio.wait();

    cout << "Num fumador: " << num_fumador << ". He llegado al bar. Hay " << total_cervezas << " cervezas disponibles," 
         << "voy a por mi cerveza." << endl;

    bar_libre=false;
    
    if(!hay_cerveza) cerveza_disponible.wait();

    total_cervezas--;
    if(total_cervezas==0) hay_cerveza=false;

    cout << "Num fumador: " << num_fumador << ". Ya tengo mi cerveza. Ahora quedan " << total_cervezas << " cervezas disponibles,"
         << "me voy del bar a bebermela fuera." << endl;

    bar_libre=true;

    if(!bar_vacio.empty()) bar_vacio.signal();

}

void Bar::ponerCervezas(){

    const int num_cervezas = aleatorio<5,10>() ;

    cout << "Poniendo cervezas." << endl;
        
    total_cervezas+=num_cervezas;
    hay_cerveza=true;

    if(!cerveza_disponible.empty()) cerveza_disponible.signal();
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero( MRef<Estanco> monitor)
{

   while (true){

      unsigned ingrediente = producir_ingrediente();
      monitor->ponerIngrediente(ingrediente);
      monitor->esperarRecogidaIngrediente();

   }

}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador(MRef<Estanco> monitor, MRef<Bar> monitor_bar, int num_fumador )
{
   while( true )
   {

    monitor->ObtenerIngrediente(num_fumador);
    fumar(num_fumador);

    monitor_bar->cogerCerveza(num_fumador);
    Beber(num_fumador);

   }
}

void funcion_hebra_camarero(MRef<Bar> monitor_bar){

    while(true){

        monitor_bar->ponerCervezas();

        // espera bloqueada un tiempo igual a ''duracion_produ' milisegundos
        chrono::milliseconds duracion_produ( aleatorio<1000,1500>() );
        this_thread::sleep_for( duracion_produ );
    }
}


//----------------------------------------------------------------------


int main()
{
   cout << "-----------------------------------------------------------------" << endl
        << "            Problema de los fumadores (monitores SU)." << endl
        << "------------------------------------------------------------------" << endl
        << flush ;
    
    MRef<Estanco> monitor = Create<Estanco>() ;
    MRef<Bar> monitor_bar = Create<Bar>() ;

    thread hebra_estanquero ( funcion_hebra_estanquero, monitor);
    thread hebra_fumador[num_fumadores];
    thread hebra_camarero(funcion_hebra_camarero,monitor_bar);

    for (int i=0; i<num_fumadores; i++) hebra_fumador[i]= thread( funcion_hebra_fumador, monitor, monitor_bar, i );


    hebra_estanquero.join() ;
    for (int i=0; i<num_fumadores; i++) hebra_fumador[i].join() ;
    hebra_camarero.join();

    return 0;
}