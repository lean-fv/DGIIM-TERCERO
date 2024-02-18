// -----------------------------------------------------------------------------
//
// Sistemas concurrentes y Distribuidos.
// Práctica 3. Implementación de algoritmos distribuidos con MPI
//
// Archivo: filosofos-plantilla.cpp
// Implementación del problema de los filósofos (sin camarero).
// Plantilla para completar.
//
// Historial:
// Actualizado a C++11 en Septiembre de 2017
// -----------------------------------------------------------------------------


#include <mpi.h>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <iostream>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
    num_filosofos = 5 ,              // número de filósofos 
    num_filo_ten  = 2*num_filosofos, // número de filósofos y tenedores 
    num_procesos  = num_filo_ten;   // número de procesos total (por ahora solo hay filo y ten)

    const string
            filosofos[num_filosofos]={"Pitágoras","Sócrates","Platón","Aristóteles","Epicuro"},
            tenedores[num_filosofos]={"T0","T1","T2","T3","T4"};


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

void funcion_filosofos( int id )
{
   
    const int 
            id_ten_izq = (id+1)              % num_filo_ten, //id. tenedor izq.
            id_ten_der = (id+num_filo_ten-1) % num_filo_ten; //id. tenedor der.

    int msg;

    const int
            //Los numeros de orden pares cogen primero el derecho y luego el izquierdo y
            //los impares viceversa, para evitar interbloqueo.
            id_tenedor_1= (id/2)%2 == 0 ? id_ten_izq : id_ten_der, //1º tenedor
            id_tenedor_2= (id/2)%2 == 0 ? id_ten_der : id_ten_izq; //2º tenedor

     string 
            filosofo_actual= filosofos[id/2],
            ten_izq_actual= tenedores[id_tenedor_1/2],
            ten_der_actual= tenedores[id_tenedor_2/2];

    while ( true )
    {
        cout <<"Filósofo " <<filosofo_actual << " solicita 1er tenedor." <<ten_izq_actual <<endl;
        
        MPI_Ssend(&msg,1,MPI_INT,id_tenedor_1,0,MPI_COMM_WORLD);

        cout <<"Filósofo " <<filosofo_actual <<" solicita 2o tenedor." <<ten_der_actual <<endl;
        
        MPI_Ssend(&msg,1,MPI_INT,id_tenedor_2,0,MPI_COMM_WORLD);

        cout <<"Filósofo " <<filosofo_actual <<" comienza a comer" <<endl ;
        sleep_for( milliseconds( aleatorio<10,100>() ) );

        cout <<"Filósofo " <<filosofo_actual <<" suelta 1er tenedor. " <<ten_izq_actual <<endl;

        MPI_Ssend(&msg,1,MPI_INT,id_tenedor_1,0,MPI_COMM_WORLD);

        cout<< "Filósofo " <<filosofo_actual <<" suelta 2o tenedor." <<ten_der_actual <<endl;
        
        MPI_Ssend(&msg,1,MPI_INT,id_tenedor_2,0,MPI_COMM_WORLD);

        cout << "Filosofo " << filosofo_actual << " comienza a pensar" << endl;
        sleep_for( milliseconds( aleatorio<10,100>() ) );
    }
}
// ---------------------------------------------------------------------

void funcion_tenedores( int id )
{
    int valor, id_filosofo ;  // valor recibido, identificador del filósofo
    MPI_Status estado ;       // metadatos de las dos recepciones

    while ( true )
    {
    
        MPI_Recv(&valor,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&estado);

        id_filosofo=estado.MPI_SOURCE;

        string 
            filosofo_actual= filosofos[id_filosofo/2],
            ten_actual= tenedores[id/2];

        cout <<"Ten. " <<ten_actual <<" ha sido cogido por filo. " <<filosofo_actual <<endl;


        MPI_Recv(&valor,1,MPI_INT,id_filosofo,0,MPI_COMM_WORLD,&estado);
        
        cout <<"Ten. "<< ten_actual<< " ha sido liberado por filo. " <<filosofo_actual <<endl ;
    }
}
// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
    int id_propio, num_procesos_actual ;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
    MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );


    if ( num_procesos == num_procesos_actual )
    {
        // ejecutar la función correspondiente a 'id_propio'
        if ( id_propio % 2 == 0 )          // si es par
            funcion_filosofos( id_propio ); //   es un filósofo
        else                               // si es impar
            funcion_tenedores( id_propio ); //   es un tenedor
    }
    else
    {
        if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
        { cout << "el número de procesos esperados es:    " << num_procesos << endl
                << "el número de procesos en ejecución es: " << num_procesos_actual << endl
                << "(programa abortado)" << endl ;
        }
    }

    MPI_Finalize( );
    return 0;
}

// ---------------------------------------------------------------------
