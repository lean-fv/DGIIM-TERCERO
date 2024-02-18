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
   num_items = 200 ,   // número de items
	tam_vec   = 10 ,   // tamaño del buffer
   multiplo = 5 ,    // multiplo para el cual imprime la impresora
   fin = num_items / multiplo; //numero de iteraciones de la hebra impresora.
unsigned  
   cont_prod[num_items] = {0}, // contadores de verificación: para cada dato, número de veces que se ha producido.
   cont_cons[num_items] = {0}, // contadores de verificación: para cada dato, número de veces que se ha consumido.
   siguiente_dato       = 0 ;  // siguiente dato a producir en 'producir_dato' (solo se usa ahí)

Semaphore 
   ocupadas = 0,
   libres = tam_vec,
   impresora=0, //Semaforo que controla la impresion cuando se inserta un multiplo de multiplo.
   em=1; //Semaforo de exclusion mutua.

int
   primera_libre=0, //primera posicion libre del buffer.
   primera_ocupada=0, //primera posicion ocupada del buffer.
   num_ocupadas=0, //numero de celdas ocupadas en el buffer
   aux = num_ocupadas; //auxiliar de num_ocupadas


int buffer[tam_vec]={}; //buffer compartido entre productor y consumidor.

//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------

unsigned producir_dato()
{
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
   const unsigned dato_producido = siguiente_dato ;
   siguiente_dato++ ;
   cont_prod[dato_producido] ++ ;
   cout << "producido: " << dato_producido << endl << flush ;
   return dato_producido ;
}
//----------------------------------------------------------------------

void consumir_dato( unsigned dato )
{
   assert( dato < num_items );
   cont_cons[dato] ++ ;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

   cout << "                  consumido: " << dato << endl ;

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

void  funcion_hebra_productora(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato = producir_dato() ;
      sem_wait(libres);

      buffer[primera_libre] = dato;
      primera_libre= (primera_libre+1)%tam_vec;

      sem_wait(em);

      num_ocupadas++;
      aux=num_ocupadas;

      sem_signal(em);

      if (dato % multiplo == 0) {
        
        sem_signal(impresora);
        sem_wait(impresora);
      }

      sem_signal(ocupadas);
   }
}

//----------------------------------------------------------------------

void funcion_hebra_consumidora(  )
{
   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      int dato ;
      sem_wait(ocupadas);

      dato = buffer[primera_ocupada];
      primera_ocupada= (primera_ocupada+1) % tam_vec;

      sem_wait(em);

      num_ocupadas--;

      sem_signal(em);
      
      sem_signal(libres);

      consumir_dato( dato ) ;
    }
}


void funcion_hebra_impresora (){
    
    for (int i = 0; i<fin; i++){

        sem_wait(impresora);

        cout << "Numero de celdas ocupadas: " << aux << endl;

        sem_signal(impresora);
    }
}
//----------------------------------------------------------------------

int main()
{
   cout << "-----------------------------------------------------------------" << endl
        << "        Problema de la impresora de casillas ocupadas." << endl
        << "------------------------------------------------------------------" << endl
        << flush ;

   thread hebra_productora ( funcion_hebra_productora ),
          hebra_consumidora( funcion_hebra_consumidora ),
          hebra_impresora ( funcion_hebra_impresora );

   hebra_productora.join() ;
   hebra_consumidora.join() ;
   hebra_impresora.join() ;

   test_contadores();
}
