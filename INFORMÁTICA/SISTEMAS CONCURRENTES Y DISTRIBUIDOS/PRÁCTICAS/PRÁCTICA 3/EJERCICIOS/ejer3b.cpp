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
    id_portero=num_filo_ten+1,
    num_procesos  = num_filo_ten + 2,   // número de procesos total

    //etiquetas
    etiq_sentarse = 0,
    etiq_levantarse = 1,
    etiq_A=2,
    etiq_B=3,
    etiq_salir=4;


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
        sleep_for( milliseconds( aleatorio<200,400>() ) );


        cout <<"Filósofo " <<id <<" suelta ten. izq. " <<id_ten_izq <<endl;

        MPI_Ssend(&msg,1,MPI_INT,id_ten_izq,0,MPI_COMM_WORLD);

        cout<< "Filósofo " <<id <<" suelta ten. der. " <<id_ten_der <<endl;
        
        MPI_Ssend(&msg,1,MPI_INT,id_ten_der,0,MPI_COMM_WORLD);


        //Levantarse
        cout <<"Filósofo " <<id << " solicita levantarse." <<endl;
        MPI_Ssend(&msg,1,MPI_INT,id_camarero,etiq_levantarse,MPI_COMM_WORLD);

        //Ir al banio

        int banio=aleatorio<1,3>(),
        etiq_aceptable;

        if (banio==etiq_A || banio==etiq_B){

            cout << "Filosofo " << id << "solicita entrar al banio " << banio << endl;
            
            if (banio==etiq_A) etiq_aceptable=etiq_A;
            else etiq_aceptable=etiq_B;
            
            MPI_Ssend(&banio,1,MPI_INT,id_portero,etiq_aceptable,MPI_COMM_WORLD);

            sleep_for( milliseconds( aleatorio<200,400>() ) );

            MPI_Send(&banio,1,MPI_INT,id_portero,etiq_salir,MPI_COMM_WORLD);


        }
       
        
        //Pensar
        cout << "Filosofo " << id << " comienza a pensar" << endl;
        sleep_for( milliseconds( aleatorio<200,400>() ) );
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

void funcion_portero(){

    MPI_Status estado;
    int msg;
    bool 
        banio_A_ocupado=false,
        banio_B_ocupado=false;

    bool mensaje_aceptable=true;
    

    while(true){

        int hay_mensaje;

        MPI_Iprobe(MPI_ANY_SOURCE,etiq_salir,MPI_COMM_WORLD,&hay_mensaje,&estado);

        if(hay_mensaje>0){

            MPI_Recv(&msg,1,MPI_INT,estado.MPI_SOURCE,etiq_salir,MPI_COMM_WORLD,&estado);
            
            int banio;
            string el_banio;

            if (banio==etiq_A){

                el_banio="A";
                banio_A_ocupado=false;

            } 
            else {
                
                el_banio="B";
                banio_B_ocupado=false;
            }

            cout << "Filosofo " << estado.MPI_SOURCE/2 << " sale del banio" << el_banio << endl;
            
        }

        else{

            if(!banio_A_ocupado){

                MPI_Iprobe(MPI_ANY_SOURCE,etiq_A,MPI_COMM_WORLD,&hay_mensaje,&estado);

                if(hay_mensaje>0){

                    MPI_Recv(&msg,1,MPI_INT,estado.MPI_SOURCE,etiq_A,MPI_COMM_WORLD,&estado);
                    banio_A_ocupado=true;
                    cout << "Filosofo " << estado.MPI_SOURCE/2 << " entra al banio A." << endl;

                }
            }

            else{

                if(!banio_B_ocupado){

                    MPI_Iprobe(MPI_ANY_SOURCE,etiq_B,MPI_COMM_WORLD,&hay_mensaje,&estado);

                    if(hay_mensaje>0){

                        MPI_Recv(&msg,1,MPI_INT,estado.MPI_SOURCE,etiq_B,MPI_COMM_WORLD,&estado);
                        banio_B_ocupado=true;
                        cout << "Filosofo " << estado.MPI_SOURCE/2 << " entra al banio B." << endl;

                    }

                   
                }

                else {
                        
                    mensaje_aceptable=false;
                    sleep_for( milliseconds( aleatorio<200,400>() ) );

                }
                
            }

        }


        if (!mensaje_aceptable) cout << "Portero: no hay ningun mensaje aceptable." << endl;
        if (hay_mensaje==0){

            cout << "Portero: no hay ningun mensaje aceptable." << endl;
            MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&estado);

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

        else if(id_propio==id_portero)
            funcion_portero();

        else if ( id_propio % 2 == 0 )          // si es par
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
