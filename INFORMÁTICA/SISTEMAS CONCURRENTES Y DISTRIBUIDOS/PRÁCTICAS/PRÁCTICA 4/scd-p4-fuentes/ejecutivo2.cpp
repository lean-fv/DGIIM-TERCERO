// -----------------------------------------------------------------------------

/* Nombre y Apellidos: Leandro Jorge Fernández Vega
   DNI: 77556130S
   Correo: leandrofdez@correo.ugr.es
   Titulación: Doble Grado en Ingeniería Informática y Matemáticas.
*/

// Sistemas concurrentes y Distribuidos.
// Práctica 4. Implementación de sistemas de tiempo real.
//
// Archivo: ejecutivo2.cpp
// Implementación del primer ejemplo de ejecutivo cíclico:
//
//   Datos de las tareas:
//   ------------
//   Ta.  T    C
//   ------------
//   A  500   100
//   B  500   150
//   C  1000  200
//   D  2000  240
//  -------------
//
//  Planificación (con Ts == 500 ms)
//  *---------*----------*---------*--------*
//  | A B     | A B C    | A B C   | A B D  |
//  *---------*----------*---------*--------*
// 
/*
      1.- ¿Cuál es el menor tiempo de espera que queda al final de las iteraciones 
         del ciclo secundario en tu solucion?

         El menor tiempo de espera es el de la cuarta etapa, 500-(100+150+240)=10 ms

      2.- ¿Sería planificable si la tarea D tuviese un tiempo de cómputo de 250 ms?

         Sí, ya que la etapa donde se ejecuta sería de 100+150+250 = 500 ms, el tiempo 
      que dura un ciclo secundario.

*/
//
//
// Historial:
// Creado en Diciembre de 2017
// -----------------------------------------------------------------------------

#include <string>
#include <iostream> // cout, cerr
#include <thread>
#include <chrono>   // utilidades de tiempo
#include <ratio>    // std::ratio_divide

using namespace std ;
using namespace std::chrono ;
using namespace std::this_thread ;

// tipo para duraciones en segundos y milisegundos, en coma flotante:
//typedef duration<float,ratio<1,1>>    seconds_f ;
typedef duration<float,ratio<1,1000>> milliseconds_f ;

// -----------------------------------------------------------------------------
// tarea genérica: duerme durante un intervalo de tiempo (de determinada duración)

void Tarea( const std::string & nombre, milliseconds tcomputo )
{
   cout << "   Comienza tarea " << nombre << " (C == " << tcomputo.count() << " ms.) ... " ;
   sleep_for( tcomputo );
   cout << "fin." << endl ;
}

// -----------------------------------------------------------------------------
// tareas concretas del problema:

void TareaA() { Tarea( "A", milliseconds(100) );  }
void TareaB() { Tarea( "B", milliseconds(150) );  }
void TareaC() { Tarea( "C", milliseconds(200) );  }
void TareaD() { Tarea( "D", milliseconds(240) );  }

// -----------------------------------------------------------------------------
// implementación del ejecutivo cíclico:

int main( int argc, char *argv[] )
{
   // Ts = duración del ciclo secundario (en unidades de milisegundos, enteros)
   const milliseconds Ts_ms( 500 );

   // ini_sec = instante de inicio de la iteración actual del ciclo secundario
   time_point<steady_clock> ini_sec = steady_clock::now();

   while( true ) // ciclo principal
   {
      cout << endl
           << "---------------------------------------" << endl
           << "Comienza iteración del ciclo principal." << endl ;

      for( int i = 1 ; i <= 4 ; i++ ) // ciclo secundario (4 iteraciones)
      {
         cout << endl << "Comienza iteración " << i << " del ciclo secundario." << endl ;

         switch( i )
         {
            case 1 : TareaA(); TareaB();                     break ;
            case 2 : TareaA(); TareaB(); TareaC();           break ;
            case 3 : TareaA(); TareaB(); TareaC();           break ;
            case 4 : TareaA(); TareaB(); TareaD();           break ;
         }

         // calcular el siguiente instante de inicio del ciclo secundario
         ini_sec += Ts_ms ;

         // esperar hasta el inicio de la siguiente iteración del ciclo secundario
         sleep_until( ini_sec );

         // calcular retraso
         float retraso = milliseconds_f(steady_clock::now()-ini_sec).count();

         cout << "El retraso al volver de 'sleep_until' es de " << retraso << " milisegundos." << endl;   
      }
   }
}
