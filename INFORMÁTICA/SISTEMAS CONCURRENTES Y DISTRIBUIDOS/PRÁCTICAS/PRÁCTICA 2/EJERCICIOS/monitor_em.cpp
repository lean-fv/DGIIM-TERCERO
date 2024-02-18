// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Seminario 2. Introducción a los monitores en C++11.
//
// archivo: monitor_em.cpp
// Ejemplo de monitores en C++11 sin variables condición
// (solo con encapsulamiento y exclusión mutua)
//
//  -- MContador1 : sin E.M., únicamente encapsulamiento
//  -- MContador2 : con E.M. mediante clase base 'HoareMonitor' y MRef
//
// Historial:
// Julio 2017: creado 
// Sept 2022 : se quita MContador3 antiguo y se adapta MContador2 para usar HoareMonitor
// --------------------------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include "scd.h"

using namespace std ;
using namespace std::chrono ;
using namespace scd ;

const int num_incrementos = 10000 ;

// *****************************************************************************
// clase contador, sin exclusión mutua

class MContador1
{
   private:
   int cont ;

   public:
   MContador1( int valor_ini ) ;
   void incrementa();
   int leer_valor();
} ;
// -----------------------------------------------------------------------------

MContador1::MContador1( int valor_ini )
{
   cont = valor_ini ;
}
// -----------------------------------------------------------------------------

void MContador1::incrementa()
{
   cont ++ ;
}
// -----------------------------------------------------------------------------

int MContador1::leer_valor()
{
   return cont ;
}

// *****************************************************************************
// clase contador, con exclusión mutua mediante herencia de 'HoareMonitor'

class MContador2 : public HoareMonitor
{
   private:
   int cont ;

   public:
   MContador2( int valor_ini ) ;
   void incrementa();
   int leer_valor();
} ;

// -----------------------------------------------------------------------------

MContador2::MContador2( int valor_ini )
{
   cont = valor_ini ;
}
// -----------------------------------------------------------------------------

void MContador2::incrementa()
{
   cont ++ ;  
}
// -----------------------------------------------------------------------------

int MContador2::leer_valor()
{
   return cont ;
}

// *****************************************************************************

void funcion_hebra_M1( MContador1 & monitor )
{
   for( int i = 0 ; i < num_incrementos ; i++ )
      monitor.incrementa();
}
// -----------------------------------------------------------------------------

void test_1()
{
   MContador1 monitor(0) ;

   
   time_point<steady_clock> inicio_no_atom  = steady_clock::now() ;

   thread hebra1( funcion_hebra_M1, ref(monitor) ),
          hebra2( funcion_hebra_M1, ref(monitor) );

   hebra1.join();
   hebra2.join();

   time_point<steady_clock> fin_no_atom = steady_clock::now() ;

   duration<float,milli> tiempo_no_atom = fin_no_atom - inicio_no_atom ;


   cout  << "Monitor contador (sin exclusión mutua):" << endl
         << endl
         << "  valor esperado == " << 2*num_incrementos << endl
         << "  valor obtenido == " << monitor.leer_valor() << endl
         << "  tiempo sin exclusion mutua (ms) " << tiempo_no_atom.count() << endl
         << endl;
}
// *****************************************************************************

void funcion_hebra_M2( MRef<MContador2>  monitor )
{
   for( int i = 0 ; i < num_incrementos ; i++ )
      monitor->incrementa();
}
// -----------------------------------------------------------------------------

void test_2()
{
   MRef<MContador2> monitor = Create<MContador2>(0) ;

   time_point<steady_clock> inicio_atom  = steady_clock::now() ;

   thread hebra1( funcion_hebra_M2, monitor ),
          hebra2( funcion_hebra_M2, monitor );

   hebra1.join();
   hebra2.join();

   time_point<steady_clock> fin_atom  = steady_clock::now() ;

   duration<float,milli> tiempo_atom    = fin_atom  - inicio_atom ;

   cout  << "Monitor contador (EM usando clase derivada de HoareMonitor):" << endl
         << endl
         << "  valor esperado == " << 2*num_incrementos << endl
         << "  valor obtenido == " << monitor->leer_valor() << endl
         << "  tiempo con exclusion mutua (ms) " << tiempo_atom.count() << endl
         << endl ;
}
// *****************************************************************************

int main()
{
   test_1();
   test_2();
}
