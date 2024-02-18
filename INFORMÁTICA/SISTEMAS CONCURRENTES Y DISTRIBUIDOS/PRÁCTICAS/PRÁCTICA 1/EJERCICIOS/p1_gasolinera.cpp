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

const int 

    c_diesel= 5, //coches diesel
    c_gasolina= 5, //coches gasolina
    num_coches = c_diesel + c_gasolina, //numero de hebras que representan coches que repostan en un surtidor.

    s_diesel = 2, // numero de surtidores diesel, s_diesel < c_diesel
    s_gasolina = 2; // numero de surtidores gasolina, s_gasolina < c_gasolina


Semaphore 

    reposta_diesel = s_diesel,
    reposta_gasolina = s_gasolina,
    em=1; // exclusion mutua

int 
    surt_uso=0, //indica cuantos surtidores de cualquier tipo hay en uso.
    aux = surt_uso; //auxiliar para surt_uso

void repostar(int num_coche, string tipo_coche){


    cout << "Coche numero " << num_coche << " (" << tipo_coche << ") comienza a repostar." << endl;

    this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    cout << "Coche numero " << num_coche << " (" << tipo_coche << ") acaba de repostar." << endl;

}

void funcion_hebra_diesel(int num_coche){

    while(true){

        sem_wait(reposta_diesel);

        sem_wait(em);
        surt_uso++;
        cout << endl << "El numero de surtidores en uso es: " << surt_uso << endl << endl;
        sem_signal(em);


        repostar(num_coche, "diesel");

        sem_wait(em);
        surt_uso--;
        cout << endl << "El numero de surtidores en uso es: " << surt_uso << endl << endl;
        sem_signal(em);

        sem_signal(reposta_diesel);



         //retraso antes de respostar de nuevo
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
    }
    


}

void funcion_hebra_gasolina(int num_coche){


    while(true){

        sem_wait(reposta_gasolina);

        sem_wait(em);
        surt_uso++;
        cout << endl << "El numero de surtidores en uso es: " << surt_uso << endl << endl;
        sem_signal(em);

        repostar(num_coche, "gasolina");

        sem_wait(em);
        surt_uso--;
        cout << endl << "El numero de surtidores en uso es: " << surt_uso << endl << endl;
        sem_signal(em);

        sem_signal(reposta_gasolina);


        //retraso antes de respostar de nuevo
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    }


}

int main()
{
   cout << "-----------------------------------------------------------------" << endl
        << "                Problema de la gasolinera." << endl
        << "------------------------------------------------------------------" << endl
        << flush ;

    thread  coches_diesel[c_diesel],
            coches_gasolina[c_gasolina];

    for (int i=0; i<c_diesel; i++)  coches_diesel[i]= thread ( funcion_hebra_diesel,i );
    for (int i=0; i<c_gasolina; i++)  coches_gasolina[i]= thread ( funcion_hebra_gasolina,i+c_diesel );

    for (int i=0; i<c_diesel; i++) coches_diesel[i].join() ;
    for (int i=0; i<c_gasolina; i++) coches_gasolina[i].join() ;

    return 0;
}