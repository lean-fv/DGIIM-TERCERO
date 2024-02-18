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

    c_diesel= 4, //coches diesel
    c_gasolina= 6, //coches gasolina
    num_coches = c_diesel + c_gasolina, //numero de hebras que representan coches que repostan en un surtidor.

    s_diesel = 2, // numero de surtidores diesel, s_diesel < c_diesel
    s_gasolina = 3; // numero de surtidores gasolina, s_gasolina < c_gasolina

int surt_uso=0; //Numero de surtidores en uso.

void repostar(int num_coche, string tipo_coche){


    cout << "Coche numero " << num_coche << " (" << tipo_coche << ") comienza a repostar." << endl;

    this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    cout << "Coche numero " << num_coche << " (" << tipo_coche << ") acaba de repostar." << endl;

}

class Gasolinera : public HoareMonitor {

    private:

    int
        n_diesel, //Numero de surtidores diesel en uso
        n_gasolina; //Numero de surtidores gasolina en uso

    CondVar
        diesel_libre, //Hay surtidores de diesel libres
        gasolina_libre; //Hay surtidores de gasolina libres

    public:

    Gasolinera(); //constructor
    void entra_coche_diesel(int num_coche); //Determina cuando entra a repostar un coche diesel.
    void sale_coche_diesel(int num_coche);  //Determina cuando sale de repostar un coche diesel.
    void entra_coche_gasolina(int num_coche); //Determina cuando entra a repostar un coche gasolina.
    void sale_coche_gasolina(int num_coche); //Determina cuando sale de repostar un coche gasolina.
};

Gasolinera::Gasolinera(){

    n_diesel=0;
    n_gasolina=0;
    diesel_libre=newCondVar();
    gasolina_libre=newCondVar();

}

void Gasolinera::entra_coche_diesel(int num_coche){

    if(n_diesel==s_diesel) diesel_libre.wait();

    n_diesel++;
    surt_uso= n_diesel+n_gasolina;

    cout << "Coche " << num_coche << " (diesel) entrando a gasolinera." << endl
         <<"Número de surtidores en uso:" << surt_uso << endl << endl;

}


void Gasolinera::sale_coche_diesel(int num_coche){

    n_diesel--;
    surt_uso= n_diesel+n_gasolina;

    cout << "Coche " << num_coche << " (diesel) saliendo de gasolinera." << endl
         <<"Número de surtidores en uso:" << surt_uso << endl << endl;

    if(!diesel_libre.empty()) diesel_libre.signal();
}


void Gasolinera::entra_coche_gasolina(int num_coche){

     if(n_gasolina==s_gasolina) gasolina_libre.wait();

    n_gasolina++;
    surt_uso= n_diesel+n_gasolina;

    cout << "Coche " << num_coche << " (gasolina) entrando a gasolinera." << endl
         <<"Número de surtidores en uso:" << surt_uso << endl << endl;
}


void Gasolinera::sale_coche_gasolina(int num_coche){

    n_gasolina--;
    surt_uso= n_diesel+n_gasolina;

    cout << "Coche " << num_coche << " (gasolina) saliendo de gasolinera." << endl
         <<"Número de surtidores en uso:" << surt_uso << endl << endl;

    if(!gasolina_libre.empty()) gasolina_libre.signal();
}



void funcion_hebra_diesel(MRef<Gasolinera> monitor, int num_coche){

    while(true){

       monitor->entra_coche_diesel(num_coche);

       repostar(num_coche,"diesel");

       monitor->sale_coche_diesel(num_coche);


        //retraso antes de respostar de nuevo
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
    }
}


void funcion_hebra_gasolina(MRef<Gasolinera> monitor, int num_coche){


    while(true){

        monitor->entra_coche_gasolina(num_coche);

        repostar(num_coche,"gasolina");

        monitor->sale_coche_gasolina(num_coche);


        //retraso antes de respostar de nuevo
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    }
}

int main()
{
   cout << "-----------------------------------------------------------------" << endl
        << "                Problema de la gasolinera (monitores SU)." << endl
        << "------------------------------------------------------------------" << endl
        << flush ;
    
    MRef<Gasolinera> monitor = Create<Gasolinera>() ;

    thread  coches_diesel[c_diesel],
            coches_gasolina[c_gasolina];

    for (int i=0; i<c_diesel; i++)  coches_diesel[i]= thread ( funcion_hebra_diesel,monitor,i );
    for (int i=0; i<c_gasolina; i++)  coches_gasolina[i]= thread ( funcion_hebra_gasolina,monitor, i+c_diesel );

    for (int i=0; i<c_diesel; i++) coches_diesel[i].join() ;
    for (int i=0; i<c_gasolina; i++) coches_gasolina[i].join() ;

    return 0;
}