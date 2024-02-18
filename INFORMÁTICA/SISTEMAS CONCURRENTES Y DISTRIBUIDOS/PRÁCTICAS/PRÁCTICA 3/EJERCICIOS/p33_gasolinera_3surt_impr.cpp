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

    num_coches = 14, //numero de hebras que representan coches que repostan en un surtidor.
    id_gasolinera=num_coches, //Identificador de la gasolinera
    id_impresora=id_gasolinera+1,
    num_procesos_esperado = num_coches+2,

    num_combustibles=3, //numero de tipos de combustible
    n_surtidores[num_combustibles] = {2,3,4}, // numero de surtidores, n_surtidores < num_coches
    
    //etiqueta
    etiq_salir=num_combustibles;

int n_surtidores_libres[num_combustibles]=
                {n_surtidores[0],n_surtidores[1],n_surtidores[2]}; // numero de surtidores libres de cada tipo.



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


void imprimir(const string& cadena){
    
    MPI_Ssend(cadena.c_str(), cadena.length(),MPI_CHAR,id_impresora,0,MPI_COMM_WORLD);
}


void repostar(int num_coche, const int tipo_combustible){


    string mensaje = "Coche " + to_string(num_coche) + " (tipo de combustible" + to_string(tipo_combustible) + ") comienza a repostar.";
    imprimir(mensaje);

    this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    mensaje = "Coche " + to_string(num_coche) + " (tipo de combustible" + to_string(tipo_combustible) + ") termina de repostar.";
    imprimir(mensaje);
}



void funcion_coche(int num_coche){

    const int tipo_combustible=aleatorio<0,num_combustibles-1>();
    int valor;

    while(true){

        string mensaje = "Coche " + to_string(num_coche) + " (tipo de combustible" + to_string(tipo_combustible) + ") solicita repostar.";
        imprimir(mensaje);

        MPI_Ssend(&tipo_combustible, 1, MPI_INT, id_gasolinera,tipo_combustible,MPI_COMM_WORLD);

        repostar(num_coche,tipo_combustible);

        MPI_Ssend(&tipo_combustible, 1, MPI_INT, id_gasolinera,etiq_salir,MPI_COMM_WORLD);

        //retraso antes de respostar de nuevo
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    }
}


void funcion_gasolinera(){

    int tipo_combustible;
    MPI_Status estado;


    while(true){

        int hay_mensaje=0;
        string mensaje;

        MPI_Iprobe(MPI_ANY_SOURCE,etiq_salir,MPI_COMM_WORLD, &hay_mensaje,&estado);

        if (hay_mensaje>0){

            MPI_Recv(&tipo_combustible, 1, MPI_INT, estado.MPI_SOURCE, etiq_salir,MPI_COMM_WORLD,&estado);
            n_surtidores_libres[tipo_combustible]++;

            mensaje = "Gasolinera: procesando fin de repostaje del coche " + to_string(estado.MPI_SOURCE)+
                             " (tipo de combustible " + to_string(tipo_combustible) + ")";
            imprimir(mensaje);
        }

        for(int tipo_comb=0; tipo_comb<num_combustibles; tipo_comb++){

            if(n_surtidores_libres[tipo_comb]>0){
                
                int hay_mensaje_tipo=0;
                MPI_Iprobe(MPI_ANY_SOURCE,tipo_comb,MPI_COMM_WORLD, &hay_mensaje_tipo,&estado);

                if(hay_mensaje_tipo>0){

                    MPI_Recv(&tipo_combustible, 1, MPI_INT, estado.MPI_SOURCE, tipo_comb,MPI_COMM_WORLD,&estado);
                    n_surtidores_libres[tipo_combustible]--;
                    hay_mensaje=1;

                    mensaje = "Gasolinera: procesando inicio de repostaje del coche " + to_string(estado.MPI_SOURCE)+
                              " (tipo de combustible " + to_string(tipo_combustible) + ")";
                    imprimir(mensaje);
                }

            }
            
        }        

        if(hay_mensaje==0)  this_thread::sleep_for( chrono::milliseconds( 20 ));
    }
        
}


void funcion_impresora(){

    MPI_Status estado;
    int long_mensaje;

    while(true){

        MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&estado);

        MPI_Get_count(&estado,MPI_CHAR,&long_mensaje);

        char* mensaje = new char[long_mensaje+1];

        MPI_Recv(mensaje,long_mensaje,MPI_CHAR,estado.MPI_SOURCE,estado.MPI_TAG,MPI_COMM_WORLD,&estado);

        mensaje[long_mensaje]=0;

        cout << mensaje << endl;

        delete[] mensaje;

    }
}


int main( int argc, char *argv[] )
{
        
    int id_propio, num_procesos_actual,tipo_combustible;

    // inicializar MPI, leer identif. de proceso y número de procesos
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
    MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );

    if ( num_procesos_esperado == num_procesos_actual )
    {
        
        if ( id_propio == 0 ) 
    
            cout << "-----------------------------------------------------------------" << endl
            << "  Problema de la gasolinera " <<  num_combustibles << " surtidores con proceso impresor (paso de mensajes MPI)." << endl
            << "------------------------------------------------------------------" << endl
            << flush ;
       

        // ejecutar la operación apropiada a 'id_propio'
        if ( id_propio == id_gasolinera )
            funcion_gasolinera();

        else if (id_propio == id_impresora)
            funcion_impresora();

        else
            funcion_coche( id_propio );
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
