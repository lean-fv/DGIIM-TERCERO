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

Semaphore mostr_vacio[num_fumadores]={1,1,1}; // 1 si esta vacio, 0 si esta ocupado

Semaphore ingr_disp[num_fumadores]={0,0,0}; // 1 si el ingrediente i esta disponible en mostrador, 0 si no.

unsigned ingrediente, ingrediente2; // aquel que produce el estanquero y que necesita cada fumador.




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

        do{
            ingrediente2=producir_ingrediente();
        }while(ingrediente==ingrediente2);

        sem_wait(mostr_vacio[ingrediente]);
        sem_wait(mostr_vacio[ingrediente2]);

        cout << "Producido ingrediente 1" << ingrediente << endl;
        cout << "Producido ingrediente 2" << ingrediente2 << endl;


        sem_signal(ingr_disp[ingrediente]);
        sem_signal(ingr_disp[ingrediente2]);

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

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {

    sem_wait(ingr_disp[num_fumador]);

    cout << "Retirando ingrediente " << num_fumador << endl;

    sem_signal(mostr_vacio[num_fumador]);

    fumar(num_fumador);

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

    for (int i=0; i<num_fumadores; i++) hebra_fumador[i]= thread( funcion_hebra_fumador, i );


    hebra_estanquero.join() ;
    for (int i=0; i<num_fumadores; i++) hebra_fumador[i].join() ;
}
