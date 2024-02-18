//Nombre y Apellidos: Leandro Jorge Fernández Vega      DNI: 77556130S

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "scd.h"

using namespace std ;
using namespace scd ;

//**********************************************************************
// Variables globales

const unsigned 
    num_items = 20 ,   // número de items
    tam_vec   = 10 ,   // tamaño del buffer

    // Las siguientes deben ser divisores de num_items.
    num_hebras_prod = 5 , //numero de hebras productoras
    num_hebras_cons = 10, //numero de hebras consumidoras
    //-------------------------------------------------------------------

    p = num_items/num_hebras_prod, //numero de items que produce cada productor
    c = num_items/num_hebras_cons; //numero de items que consume cada consumidor



unsigned  
   cont_prod[num_items] = {0}, // contadores de verificación: para cada dato, número de veces que se ha producido.
   cont_cons[num_items] = {0}, // contadores de verificación: para cada dato, número de veces que se ha consumido.
   siguiente_dato[num_hebras_prod] = {0},  // siguiente dato a producir en 'producir_dato' (solo se usa ahí)

   cont_hebras_prod[num_hebras_prod]={0}, // para cada hebra productora, numero de veces que ha producido
   cont_hebras_cons[num_hebras_cons]={0}; // para cada hebra consumidora, numero de veces que ha consumido


Semaphore 
   ocupadas = 0,  // controla que haya datos a leer en el buffer
   libres = tam_vec, // controla que haya espacio para insertar en el buffer
   em_prods = 1, // exclusion mutua para productores
   em_cons = 1;  // exclusion mutua para consumidores


int
   primera_libre=0, //primera posicion libre del buffer.
   primera_ocupada=0; //primera posicion ocupada del buffer.


int buffer[tam_vec]={}; //buffer compartido entre productor y consumidor.

//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------

unsigned producir_dato(int num_hebra)
{
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
   
   const unsigned dato_producido = num_hebra*p + cont_hebras_prod[num_hebra];

   cont_prod[dato_producido] ++ ;
   cont_hebras_prod[num_hebra]++;

   cout << "producido: " << dato_producido << endl << flush ;
   cout << "Hebra " << num_hebra << " ha producido " << cont_hebras_prod[num_hebra] << " valores."<< endl << flush ;

   return dato_producido ;
}
//----------------------------------------------------------------------

void consumir_dato( unsigned dato, int num_hebra )
{
   assert( dato < num_items );
   cont_cons[dato] ++ ;
   cont_hebras_cons[num_hebra] ++;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "                  consumido: " << dato << endl ;
   cout << "Hebra " << num_hebra << " ha consumido: " << cont_hebras_cons[num_hebra] << " veces " << endl ;

}


//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." ;
   for( unsigned i = 0 ; i < num_items ; i++ )
   {  if ( cont_prod[i] != 1 )
      {  cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {  cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

//----------------------------------------------------------------------

void  funcion_hebra_productora(int num_hebra)
{

   for( unsigned i = 0 ; i < p ; i++ )
   {
      int dato = producir_dato(num_hebra) ;
      sem_wait(libres);

      sem_wait(em_prods);

      buffer[primera_libre] = dato;
      primera_libre= (primera_libre+1)%tam_vec;

      sem_signal(em_prods);

      sem_signal(ocupadas);
   }
}

//----------------------------------------------------------------------

void funcion_hebra_consumidora(int num_hebra)
{

   for( unsigned i = 0 ; i < c ; i++ )
   {
      int dato ;
      sem_wait(ocupadas);

      sem_wait(em_cons);

      dato = buffer[primera_ocupada];
      primera_ocupada= (primera_ocupada+1) % tam_vec;

      sem_signal(em_cons);

      sem_signal(libres);

      consumir_dato( dato,num_hebra ) ;
    }
}
//----------------------------------------------------------------------

int main()
{
   cout << "-----------------------------------------------------------------" << endl
        << "Problema de los productores-consumidores multiples (solución FIFO)." << endl
        << "------------------------------------------------------------------" << endl
        << flush ;

   thread  hebras_prod[num_hebras_prod],
            hebras_cons[num_hebras_cons];

    for (int i=0; i<num_hebras_prod; i++)  hebras_prod[i]= thread ( funcion_hebra_productora,i );
    for (int i=0; i<num_hebras_cons; i++)  hebras_cons[i]= thread ( funcion_hebra_consumidora,i );

    for (int i=0; i<num_hebras_prod; i++) hebras_prod[i].join() ;
    for (int i=0; i<num_hebras_cons; i++) hebras_cons[i].join() ;

   test_contadores();

   return 0;
}
