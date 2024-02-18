//Nombre y Apellidos: Leandro Jorge Fernández Vega      DNI: 77556130S

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "scd.h"

using namespace std ;
using namespace scd ;

const int num_fumadores = 3 ; // numero de fumadores 

Semaphore mostr_vacio=1; // 1 si esta vacio, 0 si esta ocupado

Semaphore ingr_disp[num_fumadores]={0,0,0}; // 1 si el ingrediente i esta disponible en mostrador, 0 si no.

unsigned ingrediente; // aquel que produce el estanquero y que necesita cada fumador.


Semaphore bar_vacio=1; //Entrar al bar
Semaphore cerveza_disp=0; //Numero de cervezas en barra.


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

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(  )
{

    while (true){

        ingrediente=producir_ingrediente();

        sem_wait(mostr_vacio);

        cout << "Producido ingrediente " << ingrediente << endl;

        sem_signal(ingr_disp[ingrediente]);

    }

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


//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {

    sem_wait(ingr_disp[num_fumador]);

    cout << "Retirando ingrediente " << num_fumador << endl;

    sem_signal(mostr_vacio);

    fumar(num_fumador);

    sem_wait(bar_vacio);

    cout << "Fumador: " << num_fumador << ". Voy a por mi cerveza." << endl;
    
    sem_wait(cerveza_disp);

    cout << "Fumador: " << num_fumador << ". Ya tengo mi cerveza, me voy del bar a bebermela fuera." << endl;

    sem_signal(bar_vacio);

    Beber(num_fumador);

   }
}

void funcion_hebra_camarero(){

    while(true){


        const int num_cervezas = aleatorio<5,10>() ;

        cout << "Poniendo cervezas." << endl;
        
        for (int i=0; i<num_cervezas; i++) sem_signal(cerveza_disp);
        

        // espera bloqueada un tiempo igual a ''duracion_produ' milisegundos
        chrono::milliseconds duracion_produ( aleatorio<1000,1500>() );
        this_thread::sleep_for( duracion_produ );
    }
}

//----------------------------------------------------------------------

int main()
{
   cout << "-----------------------------------------------------------------" << endl
        << "Problema de los fumadores." << endl
        << "------------------------------------------------------------------" << endl
        << flush ;
    
    thread hebra_estanquero ( funcion_hebra_estanquero );
    thread hebra_fumador[num_fumadores];
    thread hebra_camarero (funcion_hebra_camarero);

    for (int i=0; i<num_fumadores; i++) hebra_fumador[i]= thread( funcion_hebra_fumador, i );


    hebra_estanquero.join() ;
    for (int i=0; i<num_fumadores; i++) hebra_fumador[i].join() ;
    hebra_camarero.join();
}
