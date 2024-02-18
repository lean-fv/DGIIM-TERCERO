#include <iostream>
#include <iomanip>
#include <cassert>
#include <random>
#include <thread>
#include "scd.h"

using namespace std ;
using namespace scd ;

const int num_fumadores = 3;
const int num_mostradores = 2;

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

class Estanco : public HoareMonitor{
    private:

    CondVar vacio;
    CondVar ingredientes[num_fumadores];
    int ingrediente1;
    int ingrediente2;
    int cont_mostr;

    public:
    Estanco(){
            
                vacio=newCondVar();
            
            for(int i=0;i<num_fumadores;i++){
                ingredientes[i]=newCondVar();
            }
            ingrediente1=-1;
            ingrediente2=-1;
            cont_mostr=0;
    }

    void poner_ingrediente(int i, int j){

        if(cont_mostr==0){

            ingrediente1 = i;
            ingrediente2 = j;
            cout << "Se pone el ingrediente: " << i << endl;
            cout << "Se pone el ingrediente: " << j << endl;

            cont_mostr=2;
                
            if(!ingredientes[i].empty()){
                ingredientes[i].signal();
            }

            if(!ingredientes[j].empty()){
                ingredientes[j].signal();
            }

        }
          
    };


    void recoger_ingrediente(int i){
        if(ingrediente1 != i && ingrediente2!=i){
			ingredientes[i].wait();
		}

        /*
        if(ingrediente2 != j){
			ingredientes[j].wait();
		}
        */
		
        if(ingrediente1==i){

            ingrediente1 = -1;

        }
        else{

             ingrediente2 = -1;

        }
		
      
		cout << "Se retira el ingrediente: " << i << endl;
        //cout << "Se retira el ingrediente: " << j << endl;

        cont_mostr--;
        if(cont_mostr==0){

            vacio.signal();

        }
		
        //vacio[1].signal();

    }


    void esperar(){
        if(cont_mostr!=0){
            vacio.wait();
        }
	}
    
};

void funcion_hebra_estanquero(MRef<Estanco> monitor){
      int i,j;
   while(true){
      
      i = producir_ingrediente();
      do{
        j = producir_ingrediente();
      }while(i==j);

      monitor->poner_ingrediente(i,j);
      monitor->esperar();
   } 

}

void funcion_hebra_fumador(MRef<Estanco> monitor, int id_fumador){
     while( true )
   {
       monitor->recoger_ingrediente(id_fumador);
       fumar(id_fumador);
   }
}

int main()
{
    MRef<Estanco> monitor = Create<Estanco>();
   
   // declarar hebras y ponerlas en marcha
   thread estanquero(funcion_hebra_estanquero,monitor);
   thread fumadores[num_fumadores];
   
   for(int i = 0; i < num_fumadores; i++){
      fumadores[i] = thread(funcion_hebra_fumador,monitor,i);
   }
   
   estanquero.join();
   for(int i = 0; i < num_fumadores; i++){
      fumadores[i].join();
   }
   
   return 0;
}