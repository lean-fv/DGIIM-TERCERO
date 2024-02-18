//Nombre y Apellidos: Leandro Jorge Fernández Vega      DNI: 77556130S

#include <iostream>
#include <iomanip>
#include <cassert>
#include <random>
#include <thread>
#include "scd.h"

using namespace std ;
using namespace scd ;

constexpr int
   num_items = 15 ,   // número de items a producir/consumir
   num_hebras_prod = 5 ,       // numero de consumidores
   num_hebras_cons = 5,       // numero de productores

   p = num_items/num_hebras_prod, //numero de items que produce cada productor
   c = num_items/num_hebras_cons, //numero de items que consume cada consumidor

   multiplo = 5; //Multiplo sobre el que actua hebra impresora.

   
constexpr int               
   min_ms    = 5,     // tiempo minimo de espera en sleep_for
   max_ms    = 20 ;   // tiempo máximo de espera en sleep_for


mutex
   mtx ;                 // mutex de escritura en pantalla
unsigned
   cont_prod[num_items] = {0}, // contadores de verificación: producidos
   cont_cons[num_items] = {0}, // contadores de verificación: consumidos

   cont_hebras_prod[num_hebras_prod]={0}, // para cada hebra productora, numero de veces que ha producido
   cont_hebras_cons[num_hebras_cons]={0}; // para cada hebra consumidora, numero de veces que ha consumido

//**********************************************************************
// funciones comunes a las dos soluciones (fifo y lifo)
//----------------------------------------------------------------------

int producir_dato( int n_hebra  )
{
   
   this_thread::sleep_for( chrono::milliseconds( aleatorio<min_ms,max_ms>() ));
   
   const int valor_producido = n_hebra*p + cont_hebras_prod[n_hebra] ;

   cont_hebras_prod[n_hebra]++;

   mtx.lock();

   cout << "hebra productora, produce " << valor_producido << endl << flush ;
   cout << "hebra productora " << n_hebra << " ha producido " << cont_hebras_prod[n_hebra] << " valores." << endl << flush ;

   mtx.unlock();

   cont_prod[valor_producido]++ ;
   return valor_producido ;
}
//----------------------------------------------------------------------

void consumir_dato( unsigned valor_consumir , int n_hebra)
{
   if ( num_items <= valor_consumir )
   {
      cout << " valor a consumir === " << valor_consumir << ", num_items == " << num_items << endl ;
      assert( valor_consumir < num_items );
   }
   cont_cons[valor_consumir] ++ ;
   cont_hebras_cons[n_hebra]++;
   this_thread::sleep_for( chrono::milliseconds( aleatorio<min_ms,max_ms>() ));

   mtx.lock();

   cout << "                  hebra consumidora, consume: " << valor_consumir << endl ;
   cout << "hebra consumidora " << n_hebra << " ha consumido " << cont_hebras_cons[n_hebra] << " valores." << endl << flush ;

   mtx.unlock();
}
//----------------------------------------------------------------------

void test_contadores()
{
   bool ok = true ;
   cout << "comprobando contadores ...." << endl ;

   for( unsigned i = 0 ; i < num_items ; i++ )
   {
      if ( cont_prod[i] != 1 )
      {
         cout << "error: valor " << i << " producido " << cont_prod[i] << " veces." << endl ;
         ok = false ;
      }
      if ( cont_cons[i] != 1 )
      {
         cout << "error: valor " << i << " consumido " << cont_cons[i] << " veces" << endl ;
         ok = false ;
      }
   }
   if (ok)
      cout << endl << flush << "solución (aparentemente) correcta." << endl << flush ;
}

// *****************************************************************************
// clase para monitor buffer, version FIFO, semántica SC, multiples prod/cons

class Impresora : public HoareMonitor
{
 private:
 static const int           // constantes ('static' ya que no dependen de la instancia)
   num_celdas_total = 10,   //   núm. de entradas del buffer
   total_multiplos = num_items/multiplo; //Total de multiplos de 'multiplo' que hay en num_items
 int                        // variables permanentes
   buffer[num_celdas_total],//   buffer de tamaño fijo, con los datos
   primera_libre ,          //   indice de celda de la próxima inserción
   primera_ocupada ,        //   indice de celda de la próxima extraccion
   ocupacion ,               //   numero de celdas verdaderamente ocupadas
   cont_multiplos,          //Numero de multiplos de 'multiplo' que se han producido en algun instante
   aux;                     //Ayuda a contabilizar cuantos multiplos se insertan entre llamadas a metodo_impresora

 CondVar                    // colas condicion:
   ocupadas,                //  cola donde espera el consumidor (n>0)
   libres ,                 //  cola donde espera el productor  (n<num_celdas_total)
   puede_imprimir;          //  nos indica cuando la hebra impresora puede empezar a imprimir.

 public:                    // constructor y métodos públicos
   Impresora() ;             // constructor
   int  leer();                // extraer un valor (sentencia L) (consumidor)
   void escribir( int valor ); // insertar un valor (sentencia E) (productor)
   bool metodo_impresora();
} ;
// -----------------------------------------------------------------------------

Impresora::Impresora(  )
{
   primera_libre = 0 ;
   primera_ocupada = 0 ;
   ocupacion = 0 ;
   cont_multiplos=0;

   ocupadas      = newCondVar();
   libres        = newCondVar();
   puede_imprimir= newCondVar();
}
// -----------------------------------------------------------------------------
// función llamada por el consumidor para extraer un dato

int Impresora::leer(  )
{
   // esperar bloqueado hasta que 0 < primera_libre
   if ( ocupacion == 0 )
      ocupadas.wait();

   //cout << "leer: ocup == " << primera_libre << ", total == " << num_celdas_total << endl ;
   assert( 0 < ocupacion  );

   // hacer la operación de lectura, actualizando estado del monitor
   const int valor = buffer[primera_ocupada] ;

   primera_ocupada = (primera_ocupada + 1)%num_celdas_total;
   ocupacion--;
   
   // señalar al productor que hay un hueco libre, por si está esperando
   libres.signal();

   // devolver valor
   return valor ;
}
// -----------------------------------------------------------------------------

void Impresora::escribir( int valor )
{
   // esperar bloqueado hasta que primera_libre < num_celdas_total
   if ( ocupacion == num_celdas_total )
      libres.wait();

   //cout << "escribir: ocup == " << primera_libre << ", total == " << num_celdas_total << endl ;
   assert( ocupacion < num_celdas_total );

   // hacer la operación de inserción, actualizando estado del monitor
   buffer[primera_libre] = valor ;

   primera_libre = (primera_libre + 1)%num_celdas_total ;
   ocupacion++;

   if(valor % multiplo==0) {

      cont_multiplos++;
      puede_imprimir.signal();
      
   }

   // señalar al consumidor que ya hay una celda ocupada (por si esta esperando)
   ocupadas.signal();
}

bool Impresora::metodo_impresora(){

   
   bool multiplos_no_producidos=true;
   
   if(cont_multiplos==total_multiplos) multiplos_no_producidos=false;

   else {

      if (aux-cont_multiplos==0) puede_imprimir.wait();
   

      cout << endl << "Se han insertado " << cont_multiplos-aux << " múltiplos de " << multiplo 
           << " nuevo(s)."<<endl << endl;

      aux=cont_multiplos;
   }

   return multiplos_no_producidos;
}

// *****************************************************************************
// funciones de hebras

void funcion_hebra_productora( MRef<Impresora> monitor, int n_hebra )
{
   for( unsigned i = 0 ; i < p ; i++ )
   {
      int valor = producir_dato( n_hebra  ) ;
      monitor->escribir( valor );
   }
}
// -----------------------------------------------------------------------------

void funcion_hebra_consumidora( MRef<Impresora>  monitor, int n_hebra )
{

   for( unsigned i = 0 ; i < c ; i++ )
   {
      int valor = monitor->leer();
      consumir_dato( valor, n_hebra ) ;
   }
}


void funcion_hebra_impresora( MRef<Impresora>  monitor){

    while(monitor->metodo_impresora()){}

    cout << endl << "Se han insertado todos los multiplos de " << multiplo << endl<< endl;
}
// -----------------------------------------------------------------------------

int main()
{
   cout << "--------------------------------------------------------------------" << endl
        << "Problema de la impresora (Monitor SU). " << endl
        << "--------------------------------------------------------------------" << endl
        << flush ;

   // crear monitor  ('monitor' es una referencia al mismo, de tipo MRef<...>)
   MRef<Impresora> monitor = Create<Impresora>() ;

   // crear y lanzar las hebras
   // crear y lanzar las hebras
   thread hebras_prod[num_hebras_prod],
          hebras_cons[num_hebras_cons],
          hebra_impr= thread(funcion_hebra_impresora,monitor);


    for (int i=0; i<num_hebras_prod; i++) hebras_prod[i]= thread (funcion_hebra_productora, monitor,i);
    for (int i=0; i<num_hebras_cons; i++) hebras_cons[i]= thread (funcion_hebra_consumidora, monitor,i);

   // esperar a que terminen las hebras
    for (int i=0; i<num_hebras_prod; i++) hebras_prod[i].join();
    for (int i=0; i<num_hebras_cons; i++) hebras_cons[i].join();
    hebra_impr.join();


   test_contadores() ;
   return 0;
}
