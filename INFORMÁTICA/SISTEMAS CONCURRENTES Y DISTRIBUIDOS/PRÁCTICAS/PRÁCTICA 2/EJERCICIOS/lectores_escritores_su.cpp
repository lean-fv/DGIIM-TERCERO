//Nombre y Apellidos: Leandro Jorge Fernández Vega      DNI: 77556130S

#include <iostream>
#include <iomanip>
#include <cassert>
#include <random>
#include <thread>
#include "scd.h"

using namespace std ;
using namespace scd ;

const int

        num_lectores = 5,
        num_escritores = 5;


void leer(int num_lector){

    cout << "Lector " << num_lector << " leyendo." << endl;

    chrono::milliseconds duracion_produ( aleatorio<10,100>() );
    this_thread::sleep_for( duracion_produ );
}

void escribir(int num_escritor){

    cout << "Escritor " << num_escritor << " escribiendo." << endl;

    chrono::milliseconds duracion_produ( aleatorio<10,100>() );
    this_thread::sleep_for( duracion_produ );
}


class Lec_Esc : public HoareMonitor {

    private:

    bool escrib; //Indica si hay alguien escribiendo.
    int n_lec; //Indica numero de lectores leyendo.

    CondVar
            lectura,
            escritura;

    public:

    Lec_Esc (); //constructor
    void ini_lectura(); //Inicio de la lectura
    void fin_lectura(); //Fin de la lectura
    void ini_escritura(); //Inicio de la escritura 
    void fin_escritura(); //Fin de la escritura
};

Lec_Esc::Lec_Esc(){

    escrib=false;
    n_lec=0;

    lectura=newCondVar();
    escritura=newCondVar();
}



void Lec_Esc::ini_lectura(){

    if (escrib) lectura.wait();

    n_lec++;
    lectura.signal();

}



void Lec_Esc::fin_lectura(){

    n_lec--;
    if (n_lec==0) escritura.signal();

}



void Lec_Esc::ini_escritura(){

    if (n_lec > 0 || escrib) escritura.wait();
    escrib=true;

}



void Lec_Esc::fin_escritura(){

    if (!lectura.empty()) lectura.signal();
    else escritura.signal();

}



void funcion_hebra_lectora(MRef<Lec_Esc> monitor,int num_lector){

    monitor->ini_lectura();
    leer(num_lector);
    monitor->fin_lectura();

}



void funcion_hebra_escritora(MRef<Lec_Esc> monitor, int num_escritor){

    monitor->ini_escritura();
    escribir(num_escritor);
    monitor->fin_escritura();

}


int main()
{
   cout << "--------------------------------------------------------------------" << endl
        << "Problema de los Lectores-Escritores (con prioridad de escritores) . " << endl
        << "--------------------------------------------------------------------" << endl
        << flush ;

   // crear monitor  ('monitor' es una referencia al mismo, de tipo MRef<...>)
   MRef<Lec_Esc> monitor = Create<Lec_Esc>() ;

   // crear y lanzar las hebras
   // crear y lanzar las hebras
   thread hebras_lec[num_lectores],
          hebras_escr[num_escritores];

    for (int i=0; i<num_lectores; i++) hebras_lec[i]= thread (funcion_hebra_lectora, monitor,i);
    for (int i=0; i<num_escritores; i++) hebras_escr[i]= thread (funcion_hebra_escritora, monitor,i);

   // esperar a que terminen las hebras
    for (int i=0; i<num_lectores; i++) hebras_escr[i].join();
    for (int i=0; i<num_escritores; i++) hebras_lec[i].join();


   return 0;
}
