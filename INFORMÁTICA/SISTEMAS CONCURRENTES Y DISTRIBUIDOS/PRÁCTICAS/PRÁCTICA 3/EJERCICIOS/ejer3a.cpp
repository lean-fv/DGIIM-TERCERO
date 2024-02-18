//Nombre y Apellidos: Leandro Jorge Fernández Vega      DNI: 77556130S

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
    num_filosofos = 8 ,              // número de filósofos 
    num_filo_ten  = 2*num_filosofos, // número de filósofos y tenedores 
    id_camarero   = num_filo_ten,   //identificador del proceso camarero.
    num_procesos  = num_filo_ten + 1,   // número de procesos total

    //etiquetas
    etiq_sentarse = 0,
    etiq_levantarse = 1;


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

    while ( true )
    {

        //Sentarse
        cout <<"Filósofo " <<id << " solicita sentarse." <<endl;
        MPI_Ssend(&msg,1,MPI_INT,id_camarero,etiq_sentarse,MPI_COMM_WORLD);


        //Comida
        cout <<"Filósofo " <<id << " solicita ten. izq." <<id_ten_izq <<endl;
        
        MPI_Ssend(&msg,1,MPI_INT,id_ten_izq,0,MPI_COMM_WORLD);

        cout <<"Filósofo " <<id <<" solicita ten. der." <<id_ten_der <<endl;
        
        MPI_Ssend(&msg,1,MPI_INT,id_ten_der,0,MPI_COMM_WORLD);


        cout <<"Filósofo " <<id <<" comienza a comer" <<endl ;
        sleep_for( milliseconds( aleatorio<10,100>() ) );


        cout <<"Filósofo " <<id <<" suelta ten. izq. " <<id_ten_izq <<endl;

        MPI_Ssend(&msg,1,MPI_INT,id_ten_izq,0,MPI_COMM_WORLD);

        cout<< "Filósofo " <<id <<" suelta ten. der. " <<id_ten_der <<endl;
        
        MPI_Ssend(&msg,1,MPI_INT,id_ten_der,0,MPI_COMM_WORLD);


        //Levantarse
        cout <<"Filósofo " <<id << " solicita levantarse." <<endl;
        MPI_Ssend(&msg,1,MPI_INT,id_camarero,etiq_levantarse,MPI_COMM_WORLD);


        //Pensar
        cout << "Filosofo " << id << " comienza a pensar" << endl;
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

        cout <<"Ten. " <<id <<" ha sido cogido por filo. " <<id_filosofo <<endl;


        MPI_Recv(&valor,1,MPI_INT,id_filosofo,0,MPI_COMM_WORLD,&estado);
        
        cout <<"Ten. "<< id<< " ha sido liberado por filo. " <<id_filosofo <<endl ;
    }
}
// ---------------------------------------------------------------------

void funcion_camarero(){

    int fil_sentados=0,
        valor;

    MPI_Status estado;

    while (true){

        int etiq_aceptable = (fil_sentados < num_filosofos-1) ? MPI_ANY_TAG : etiq_levantarse;

        MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE,etiq_aceptable,MPI_COMM_WORLD,&estado);

        if (estado.MPI_TAG==etiq_sentarse){

            fil_sentados++;
            cout << "Se sienta filósofo " << estado.MPI_SOURCE
                 << ". Hay " << fil_sentados << " filósofos sentados."<< endl;
        } 
        
        else{

            fil_sentados--;
            cout << "Se levanta filósofo " << estado.MPI_SOURCE 
                 << ". Hay " << fil_sentados << " filósofos sentados." << endl;
        }
    }
}

int main( int argc, char** argv )
{
    int id_propio, num_procesos_actual;
   
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
    MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );


    if ( num_procesos == num_procesos_actual )
    {
        // ejecutar la función correspondiente a 'id_propio'

        if(id_propio==id_camarero)  //es el camarero.
            funcion_camarero();

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
