// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: prodcons2.cpp
// Implementación del problema del productor-consumidor con
// un proceso intermedio que gestiona un buffer finito y recibe peticiones
// en orden arbitrario
// (versión con un único productor y un único consumidor)
//
// Historial:
// Actualizado a C++11 en Septiembre de 2017
// -----------------------------------------------------------------------------

#include <iostream>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <mpi.h>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int

   n_prods               = 1, //Numero de productores
   n_cons                = 4, //Numero de consumidores
   id_buffer             = n_prods, //Identificador del buffer
   etiq_prod       = 0, //etiqueta_aceptada para productores
   etiq_cons_par = 1,
   etiq_cons_impar=2,

   num_procesos_esperado = n_prods+n_cons+1 , //Total de procesos
   num_items             = 20,
   tam_vector            = 10,

   p=num_items/n_prods, //Total de valores producidos por cada productor
   c=num_items/n_cons;  //Total de valores consumidos por cada consumidor

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}
// ---------------------------------------------------------------------
// ptoducir produce los numeros en secuencia (1,2,3,....)
// y lleva espera aleatorio
int producir(int n_orden)
{
   static int contador = 0 ;
   int valor= n_orden*p + contador;

   sleep_for( milliseconds( aleatorio<10,100>()) );

   contador++ ;

   cout << "Productor ha producido valor " << valor << endl << flush;
   return valor ;
}
// ---------------------------------------------------------------------

void funcion_productor(int n_orden)
{
   for ( unsigned int i=0 ; i < p ; i++ )
   {
      // producir valor
      int valor_prod = producir(n_orden);
      // enviar valor
      cout << "Productor " << n_orden << " va a enviar valor " << valor_prod << endl << flush;
      MPI_Ssend( &valor_prod, 1, MPI_INT, id_buffer, etiq_prod, MPI_COMM_WORLD );
   }
}
// ---------------------------------------------------------------------

void consumir( int valor_cons, int n_orden )
{
   // espera bloqueada
   sleep_for( milliseconds( aleatorio<110,200>()) );
   cout << "Consumidor " << n_orden << " ha consumido valor " << valor_cons << endl << flush ;
}
// ---------------------------------------------------------------------

void funcion_consumidor(int n_orden)
{
   int         peticion,
               valor_rec,
               etiqueta_aceptada;
   MPI_Status  estado ;

   for( unsigned int i=0 ; i < c; i++ )
   {

      if(n_orden%2==0) etiqueta_aceptada= etiq_cons_par;
      else etiqueta_aceptada = etiq_cons_impar;
        
      MPI_Ssend(&peticion,1,MPI_INT, id_buffer, etiqueta_aceptada,MPI_COMM_WORLD);
      MPI_Recv ( &valor_rec, 1, MPI_INT, id_buffer, etiqueta_aceptada, MPI_COMM_WORLD,&estado );
      

      cout << "Consumidor "<< n_orden << " ha recibido valor " << valor_rec << endl << flush ;
      consumir( valor_rec,n_orden );
   }
}
// ---------------------------------------------------------------------

void funcion_buffer()
{
   int        buffer[tam_vector],      // buffer con celdas ocupadas y vacías
              valor,                   // valor recibido o enviado
              primera_libre       = 0, // índice de primera celda libre
              primera_ocupada     = 0, // índice de primera celda ocupada
              num_celdas_ocupadas = 0, // número de celdas ocupadas
              id_emisor_aceptable ,    // identificador de emisor aceptable
              etiqueta_aceptada;       //etiqueta aceptada segun situacion del buffer
   MPI_Status estado ;                 // metadatos del mensaje recibido


   for( unsigned int i=0 ; i < num_items*2 ; i++ )
   {

         // 1. determinar si puede enviar solo prod., solo cons, o todos
         if ( num_celdas_ocupadas == 0 )               // si buffer vacío
            etiqueta_aceptada = etiq_prod;                      // $~~~$ solo prod.
         else if ( num_celdas_ocupadas == tam_vector ){ // si buffer lleno
            if(buffer[primera_ocupada]%2 == 0) etiqueta_aceptada=etiq_cons_par;
            else etiqueta_aceptada=etiq_cons_impar;
         }                                             // $~~~$ solo cons.

         else{              // si no vacío ni lleno
                                 
            bool seguir=true;
            int hay_mensaje;

            while(seguir){

               MPI_Iprobe(MPI_ANY_SOURCE,etiq_prod,MPI_COMM_WORLD,&hay_mensaje,&estado);

               if (hay_mensaje>0) etiqueta_aceptada=etiq_prod;
      
               else {

                  if(buffer[primera_ocupada]%2 ==0) etiqueta_aceptada=etiq_cons_par;
                  else etiqueta_aceptada = etiq_cons_impar;

                  MPI_Iprobe(MPI_ANY_SOURCE,etiqueta_aceptada,MPI_COMM_WORLD,&hay_mensaje,&estado);
               }

               if(hay_mensaje>0) seguir=false;
            }
         }
      

      // 2. recibir un mensaje del emisor o emisores aceptables

      MPI_Recv( &valor, 1, MPI_INT, MPI_ANY_SOURCE, etiqueta_aceptada, MPI_COMM_WORLD, &estado );

      // 3. procesar el mensaje recibido

      if( estado.MPI_TAG==etiq_prod ){ // leer emisor del mensaje en metadatos
         // si ha sido el productor: insertar en buffer
         buffer[primera_libre] = valor ;
         primera_libre = (primera_libre+1) % tam_vector ;
         num_celdas_ocupadas++ ;
         cout << "Buffer ha recibido valor " << valor << endl ;
      }

      else{

         valor = buffer[primera_ocupada] ;
         primera_ocupada = (primera_ocupada+1) % tam_vector ;
         num_celdas_ocupadas-- ;
         cout << "Buffer va a enviar valor " << valor << endl ;

         if(estado.MPI_TAG == etiq_cons_par) etiqueta_aceptada=etiq_cons_par;
         else etiqueta_aceptada=etiq_cons_impar;
         
         MPI_Ssend( &valor, 1, MPI_INT, estado.MPI_SOURCE, etiqueta_aceptada, MPI_COMM_WORLD);

      }  

   }
         
}

// ---------------------------------------------------------------------

int main( int argc, char *argv[] )
{
   int id_propio, num_procesos_actual;

   // inicializar MPI, leer identif. de proceso y número de procesos
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
   MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );

   if ( num_procesos_esperado == num_procesos_actual )
   {
      // ejecutar la operación apropiada a 'id_propio'
      if ( id_propio < id_buffer )
         funcion_productor( id_propio );
      else if ( id_propio == id_buffer )
         funcion_buffer();
      else
         funcion_consumidor( id_propio-(n_prods+1) );
   }
   else
   {
      if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
      { cout << "el número de procesos esperados es:    " << num_procesos_esperado << endl
             << "el número de procesos en ejecución es: " << num_procesos_actual << endl
             << "(programa abortado)" << endl ;
      }
   }

   // al terminar el proceso, finalizar MPI
   MPI_Finalize( );
   return 0;
}
