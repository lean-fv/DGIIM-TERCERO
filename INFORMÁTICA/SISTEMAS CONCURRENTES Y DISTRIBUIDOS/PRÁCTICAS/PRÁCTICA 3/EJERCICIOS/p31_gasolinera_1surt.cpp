//Nombre y Apellidos: Leandro Jorge Fernández Vega      DNI: 77556130S

#include <iostream>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <mpi.h>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int 

    c_gasolina= 4, //coches gasolina
    num_coches = c_gasolina, //numero de hebras que representan coches que repostan en un surtidor.
    s_gasolina = 3, // numero de surtidores gasolina, s_gasolina < c_gasolina

    id_gasolinera=num_coches, //Identificador de la gasolinera
    num_procesos_esperado = num_coches+1,

    //etiquetas
    etiq_repostar=0,
    etiq_salir=1;


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


void repostar(int num_coche, string tipo_coche){


    cout << "Coche " << num_coche << " (" << tipo_coche << ") comienza a repostar." << endl;

    this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    cout << "Coche " << num_coche << " (" << tipo_coche << ") termina de repostar." << endl;


}



void proceso_coche_gasolina(int num_coche){

    int valor;

    while(true){

        cout << "Coche " << num_coche << " (gasolina) solicita repostar." << endl;

        MPI_Ssend(&valor, 1, MPI_INT, id_gasolinera,etiq_repostar,MPI_COMM_WORLD);

        repostar(num_coche,"gasolina");

        MPI_Ssend(&valor, 1, MPI_INT, id_gasolinera,etiq_salir,MPI_COMM_WORLD);

        //retraso antes de respostar de nuevo
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    }
}


void proceso_gasolinera(){

    int surt_uso=0, //Numero de surtidores en uso.
        valor;

    MPI_Status estado;

    while(true){

        int etiq_aceptable= (surt_uso <= s_gasolina) ? MPI_ANY_TAG : etiq_salir;

        MPI_Recv(&valor, 1, MPI_INT, MPI_ANY_SOURCE, etiq_aceptable,MPI_COMM_WORLD,&estado);
        
        if (estado.MPI_TAG==etiq_repostar) surt_uso++;
        else surt_uso--;
        
    }

}


int main( int argc, char *argv[] )
{
        
    int id_propio, num_procesos_actual;

    // inicializar MPI, leer identif. de proceso y número de procesos
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
    MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );

    if ( num_procesos_esperado == num_procesos_actual )
    {
        
        if ( id_propio == 0 ) 
    
            cout << "-----------------------------------------------------------------" << endl
            << "    Problema de la gasolinera 1 surtidor (paso de mensajes MPI)." << endl
            << "------------------------------------------------------------------" << endl
            << flush ;
       

        // ejecutar la operación apropiada a 'id_propio'
        if ( id_propio == id_gasolinera )
            proceso_gasolinera();

        else
            proceso_coche_gasolina( id_propio );
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
