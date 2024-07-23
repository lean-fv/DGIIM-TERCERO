#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

Action ComportamientoJugador::think(Sensores sensores)
{

	Action accion = actIDLE;

	// Mostrar el valor de los sensores
	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC;
	switch (sensores.sentido)
	{
		case norte:	  cout << " Norte\n";	break;
		case noreste: cout << " Noreste\n";	break;
		case este:    cout << " Este\n";	break;
		case sureste: cout << " Sureste\n";	break;
		case sur:     cout << " Sur\n";	break;
		case suroeste:cout << " Suroeste\n";	break;
		case oeste:   cout << " Oeste\n";	break;
		case noroeste:cout << " Noroeste\n";	break;
	}
	cout << "Terreno: ";
	for (int i=0; i<sensores.terreno.size(); i++)
		cout << sensores.terreno[i];

	cout << "  Agentes: ";
	for (int i=0; i<sensores.agentes.size(); i++)
		cout << sensores.agentes[i];

	cout << "\nColision: " << sensores.colision;
	cout << "  Reset: " << sensores.reset;
	cout << "  Vida: " << sensores.vida << endl<< endl;



	//Primera parte: actualizacion del mundo

	//Ponemos los bordes
	if(!bordes){
		PonerBordes();
		bordes=true;
	}

	
	//Actualizacion de poisicion y orientacion
	switch (last_action){
		
		case actWALK:

			if(!sensores.colision){

				n_giros=0;
				switch (current_state.brujula){
					case norte:
						current_state.fil--; break;
					case sur:
						current_state.fil++; break;
					case este:
						current_state.col++; break;
					case oeste:
						current_state.col--; break;
					case noreste:
						current_state.fil--; current_state.col++; break;
					case noroeste:
						current_state.fil--; current_state.col--; break;
					case sureste:
						current_state.fil++; current_state.col++; break;
					case suroeste:
						current_state.fil++; current_state.col--; break;
				}
			}

			break;

		case actRUN:

			if(!sensores.colision){

				n_giros=0;
				switch (current_state.brujula){
					case norte:
						current_state.fil-=2; break;
					case sur:
						current_state.fil+=2; break;
					case este:
						current_state.col+=2; break;
					case oeste:
						current_state.col-=2; break;
					case noreste:
						current_state.fil-=2; current_state.col+=2; break;
					case noroeste:
						current_state.fil-=2; current_state.col-=2; break;
					case sureste:
						current_state.fil+=2; current_state.col+=2; break;
					case suroeste:
						current_state.fil+=2; current_state.col-=2; break;
				}
			}

			break;


		case actTURN_SR:
			n_giros++;
			current_state.brujula=(Orientacion)((current_state.brujula+1)%8);
			break;
		
		case actTURN_L:
			n_giros++;
			current_state.brujula=(Orientacion)((current_state.brujula+6)%8);
			break;
		
	}

	//Si se resetea el agente.
	if(sensores.reset){

		bien_situado=false;
		current_state.fil=current_state.col=mapaResultado.size();
		current_state.brujula=norte;
		n_giros=0;

		mapaNoOrientado=vector<vector<unsigned char>>
        	(2*mapaResultado.size(),vector<unsigned char>(2*mapaResultado.size(), '?'));

		mapaAuxFrecuencias=vector<vector<int>>
			(2*mapaResultado.size(),vector<int>(2*mapaResultado.size(),0));

		objetivo =false;
		bien_situado=false;
		bikini=false;
		zapatillas=false;
		hubo_colision=false;
		clear(cola_acciones);


		last_action=actIDLE;
	}
	////////////////////////////////////////////////////////////////////////////////////////
	


	//Alcanzar casilla de posicionamiento
	if ((sensores.terreno[0]=='G' && !bien_situado) || sensores.nivel==0){
		if(sensores.nivel!=0) Volcar(sensores);
		current_state.fil = sensores.posF;
		current_state.col= sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
	}
	////////////////////////////////////////////////////////////////////////////////////////

	//Coger bikini
	if (sensores.terreno[0]=='K') bikini=true;
	////////////////////////////////////////////////////////////////////////////////////////
	
	//Coger zapatillas
	if (sensores.terreno[0]=='D') zapatillas=true;
	////////////////////////////////////////////////////////////////////////////////////////

	//Si la bateria cae por debajo de un umbral se puede recargar
	if(sensores.bateria < UMBRAL_BATERIA) puede_recargar=true;
	

	//Registrar lo recorrido
	if (bien_situado) {
		PonerTerrenoEnMatriz(sensores, current_state, mapaResultado);
		mapaFrecuencias[current_state.fil][current_state.col]++;
	}
	else {
		
		PonerTerrenoEnMatriz(sensores, current_state, mapaNoOrientado);
		mapaAuxFrecuencias[current_state.fil][current_state.col]++;
	}
	////////////////////////////////////////////////////////////////////////////////////////



	//Segunda parte: eleccion de la accion a tomar	

	//Quedarse recargando
	if (sensores.terreno[0]=='X' && sensores.bateria < FIN_RECARGA) {
		
		last_action=actIDLE;
		return last_action;
	}
	////////////////////////////////////////////////////////////////////////////////////////

	//Anda por todos sitios, pero por bosque y agua solo cuando tiene objetos. En caso contrario gira random.
	if((sensores.terreno[2]=='T' || sensores.terreno[2]=='S' || (sensores.terreno[2]== 'A') 
		|| (sensores.terreno[2]== 'B') || sensores.terreno[2]=='K' 
		|| sensores.terreno[2]=='D' || sensores.terreno[2]=='X' || sensores.terreno[2]=='G') 
		&& sensores.agentes[2]=='_'){

		accion=actWALK;
	}

	else if (!girar_derecha){
		accion=actTURN_L;
		girar_derecha=(rand()%2 == 0);
	}

	else{
		accion=actTURN_SR;
		girar_derecha=(rand()%2 == 0);
	} 
	////////////////////////////////////////////////////////////////////////////////////////

	/*Cada 50 instantes busca la casilla por la que menos veces ha pasado, 
	siempre que no este en proceso de alcanzar una casilla especial al mismo tiempo.*/
	if(sensores.vida%N_INSTANTES==0 && !objetivo) Traza_Ruta(getCasillaMenosFrecuente(sensores.terreno));
	////////////////////////////////////////////////////////////////////////////////////////

	//Si hay una casilla espacial a la vista va a por ella.
	for (int i=0; i<FIN_ESPECIALES && !objetivo; i++) Traza_Ruta(casillas[i],sensores.terreno);
	////////////////////////////////////////////////////////////////////////////////////////

	
	//Escapar de muros. Es mas prioritario que alcanzar casilla especial.
	if(sensores.colision){
		hubo_colision=true;

		//Si hay una colision debemos cancelar el objetivo de alcanzar casilla especial o de escape.
		objetivo=false;
		clear(cola_acciones);

		cola_acciones.push(actTURN_SR);
	}


	else if (sensores.terreno[3]!='M' && sensores.terreno[1]!= 'M' && hubo_colision){
		cola_acciones.push(actTURN_SR);
		cola_acciones.push(actTURN_L);
		
		hubo_colision=false;
	}
	
	
	////////////////////////////////////////////////////////////////////////////////////////

	

	//Explotar acciones de la cola de acciones.
	if(!cola_acciones.empty()){

		accion=cola_acciones.front();
		cola_acciones.pop();
	} 

	else objetivo=false;
	
	////////////////////////////////////////////////////////////////////////////////////////
	

	//Evitar que se formen bucles de giros infinitos.
	if(n_giros>=MAX_GIROS) {
		
		clear(cola_acciones);
		if(!(sensores.terreno[2]=='M'||sensores.terreno[2]=='P')){
			n_giros=0;
			
			accion=actWALK;
		}
		else {
			
			if(girar_derecha)accion=actTURN_SR;
			else accion=actTURN_L;
			
		}
	}
	
	last_action=accion;
	return accion;
}

////////////////////////////////////////////////////////////////////////////////////////

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}



void ComportamientoJugador::PonerBordes(){

	//Superiores
	for(int i=0; i<3; i++)
		for(int j=0; j<mapaResultado.size();j++)
			mapaResultado[i][j]='P';

	//Inferiores
	for(int i=mapaResultado.size()-3; i<mapaResultado.size(); i++)
		for(int j=0; j<mapaResultado.size();j++)
			mapaResultado[i][j]='P';

	//Izquierda
	for(int i=0; i<mapaResultado.size();i++)
		for(int j=0; j<3; j++)
			mapaResultado[i][j]='P';

	//Derecha
	for(int i=0; i<mapaResultado.size();i++)
		for(int j=mapaResultado.size()-3; j<mapaResultado.size(); j++)
			mapaResultado[i][j]='P';

}


/* Extiende esta version inicial donde solo se pone la componente 0
	  en la matriz; la nueva version debe poner todas las componentes
	  de terreno en funcion de la orientacion del agente*/
void ComportamientoJugador::PonerTerrenoEnMatriz(const Sensores& sensores,
	const state& st, vector<vector<unsigned char>>& matriz) {
	
	matriz[st.fil][st.col] = sensores.terreno[0];

	switch (st.brujula){

				case norte:

					matriz[st.fil-1][st.col-1]=sensores.terreno[1];
					matriz[st.fil-1][st.col]=sensores.terreno[2];
					matriz[st.fil-1][st.col+1]=sensores.terreno[3];
					matriz[st.fil-2][st.col-2]=sensores.terreno[4];
					matriz[st.fil-2][st.col-1]=sensores.terreno[5];
					
					matriz[st.fil-2][st.col+1]=sensores.terreno[7];
					matriz[st.fil-2][st.col+2]=sensores.terreno[8];
					matriz[st.fil-3][st.col-3]=sensores.terreno[9];
					matriz[st.fil-3][st.col-2]=sensores.terreno[10];
					
					matriz[st.fil-3][st.col+2]=sensores.terreno[14];
					matriz[st.fil-3][st.col+3]=sensores.terreno[15];

					if((matriz[st.fil-2][st.col]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-2][st.col]=sensores.terreno[6];
					if((matriz[st.fil-3][st.col-1]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-3][st.col-1]=sensores.terreno[11];
					if((matriz[st.fil-3][st.col]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-3][st.col]=sensores.terreno[12];
					if((matriz[st.fil-3][st.col+1]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-3][st.col+1]=sensores.terreno[13];
				

					break;

				case sur:

					matriz[st.fil+1][st.col+1]=sensores.terreno[1];
					matriz[st.fil+1][st.col]=sensores.terreno[2];
					matriz[st.fil+1][st.col-1]=sensores.terreno[3];
					matriz[st.fil+2][st.col+2]=sensores.terreno[4];
					matriz[st.fil+2][st.col+1]=sensores.terreno[5];
					
					matriz[st.fil+2][st.col-1]=sensores.terreno[7];
					matriz[st.fil+2][st.col-2]=sensores.terreno[8];
					matriz[st.fil+3][st.col+3]=sensores.terreno[9];
					matriz[st.fil+3][st.col+2]=sensores.terreno[10];
					
					matriz[st.fil+3][st.col-2]=sensores.terreno[14];
					matriz[st.fil+3][st.col-3]=sensores.terreno[15];

					if((matriz[st.fil+2][st.col]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+2][st.col]=sensores.terreno[6];
					if((matriz[st.fil+3][st.col+1]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+3][st.col+1]=sensores.terreno[11];
					if((matriz[st.fil+3][st.col]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+3][st.col]=sensores.terreno[12];
					if((matriz[st.fil+3][st.col-1]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+3][st.col-1]=sensores.terreno[13];
					
					break;

				case este:

					matriz[st.fil-1][st.col+1]=sensores.terreno[1];
					matriz[st.fil][st.col+1]=sensores.terreno[2];
					matriz[st.fil+1][st.col+1]=sensores.terreno[3];
					matriz[st.fil-2][st.col+2]=sensores.terreno[4];
					matriz[st.fil-1][st.col+2]=sensores.terreno[5];

					matriz[st.fil+1][st.col+2]=sensores.terreno[7];
					matriz[st.fil+2][st.col+2]=sensores.terreno[8];
					matriz[st.fil-3][st.col+3]=sensores.terreno[9];
					matriz[st.fil-2][st.col+3]=sensores.terreno[10];
					
					matriz[st.fil+2][st.col+3]=sensores.terreno[14];
					matriz[st.fil+3][st.col+3]=sensores.terreno[15];

					if((matriz[st.fil][st.col+2]=='?'&& sensores.nivel==3) || sensores.nivel!=3)
						matriz[st.fil][st.col+2]=sensores.terreno[6];
					if((matriz[st.fil-1][st.col+3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-1][st.col+3]=sensores.terreno[11];
					if((matriz[st.fil][st.col+3]=='?'&& sensores.nivel==3) || sensores.nivel!=3)
						matriz[st.fil][st.col+3]=sensores.terreno[12];
					if((matriz[st.fil+1][st.col+3]=='?'&& sensores.nivel==3) || sensores.nivel!=3)
						matriz[st.fil+1][st.col+3]=sensores.terreno[13];

					break;

				case oeste:

					matriz[st.fil+1][st.col-1]=sensores.terreno[1];
					matriz[st.fil][st.col-1]=sensores.terreno[2];
					matriz[st.fil-1][st.col-1]=sensores.terreno[3];
					matriz[st.fil+2][st.col-2]=sensores.terreno[4];
					matriz[st.fil+1][st.col-2]=sensores.terreno[5];

					matriz[st.fil-1][st.col-2]=sensores.terreno[7];
					matriz[st.fil-2][st.col-2]=sensores.terreno[8];
					matriz[st.fil+3][st.col-3]=sensores.terreno[9];
					matriz[st.fil+2][st.col-3]=sensores.terreno[10];
					
					matriz[st.fil-2][st.col-3]=sensores.terreno[14];
					matriz[st.fil-3][st.col-3]=sensores.terreno[15];

					if((matriz[st.fil][st.col-2]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil][st.col-2]=sensores.terreno[6];
					if((matriz[st.fil+1][st.col-3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+1][st.col-3]=sensores.terreno[11];
					if((matriz[st.fil][st.col-3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil][st.col-3]=sensores.terreno[12];
					if((matriz[st.fil-1][st.col-3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-1][st.col-3]=sensores.terreno[13];
				
					break;

				case noreste:

					matriz[st.fil-1][st.col]=sensores.terreno[1];
					matriz[st.fil-1][st.col+1]=sensores.terreno[2];
					matriz[st.fil][st.col+1]=sensores.terreno[3];
					matriz[st.fil-2][st.col]=sensores.terreno[4];
					matriz[st.fil-2][st.col+1]=sensores.terreno[5];

					matriz[st.fil-1][st.col+2]=sensores.terreno[7];
					matriz[st.fil][st.col+2]=sensores.terreno[8];
					matriz[st.fil-3][st.col]=sensores.terreno[9];
					matriz[st.fil-3][st.col+1]=sensores.terreno[10];
					
					matriz[st.fil-1][st.col+3]=sensores.terreno[14];
					matriz[st.fil][st.col+3]=sensores.terreno[15];

					
					if((matriz[st.fil-2][st.col+2]=='?' && sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-2][st.col+2]=sensores.terreno[6];
					if((matriz[st.fil-3][st.col+2]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-3][st.col+2]=sensores.terreno[11];
					if((matriz[st.fil-3][st.col+3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-3][st.col+3]=sensores.terreno[12];
					if((matriz[st.fil-2][st.col+3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-2][st.col+3]=sensores.terreno[13];
					

					break;

				case noroeste:

					matriz[st.fil][st.col-1]=sensores.terreno[1];
					matriz[st.fil-1][st.col-1]=sensores.terreno[2];
					matriz[st.fil-1][st.col]=sensores.terreno[3];
					matriz[st.fil][st.col-2]=sensores.terreno[4];
					matriz[st.fil-1][st.col-2]=sensores.terreno[5];

					matriz[st.fil-2][st.col-1]=sensores.terreno[7];
					matriz[st.fil-2][st.col]=sensores.terreno[8];
					matriz[st.fil][st.col-3]=sensores.terreno[9];
					matriz[st.fil-1][st.col-3]=sensores.terreno[10];
					
					matriz[st.fil-3][st.col-1]=sensores.terreno[14];
					matriz[st.fil-3][st.col]=sensores.terreno[15];

					if((matriz[st.fil-2][st.col-2]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-2][st.col-2]=sensores.terreno[6];
					if((matriz[st.fil-2][st.col-3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-2][st.col-3]=sensores.terreno[11];
					if((matriz[st.fil-3][st.col-3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-3][st.col-3]=sensores.terreno[12];
					if((matriz[st.fil-3][st.col-2]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil-3][st.col-2]=sensores.terreno[13];
					

					break;

				case sureste:

					matriz[st.fil][st.col+1]=sensores.terreno[1];
					matriz[st.fil+1][st.col+1]=sensores.terreno[2];
					matriz[st.fil+1][st.col]=sensores.terreno[3];
					matriz[st.fil][st.col+2]=sensores.terreno[4];
					matriz[st.fil+1][st.col+2]=sensores.terreno[5];

					matriz[st.fil+2][st.col+1]=sensores.terreno[7];
					matriz[st.fil+2][st.col]=sensores.terreno[8];
					matriz[st.fil][st.col+3]=sensores.terreno[9];
					matriz[st.fil+1][st.col+3]=sensores.terreno[10];
					
					matriz[st.fil+3][st.col+1]=sensores.terreno[14];
					matriz[st.fil+3][st.col]=sensores.terreno[15];

					if((matriz[st.fil+2][st.col+2]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+2][st.col+2]=sensores.terreno[6];
					if((matriz[st.fil+2][st.col+3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+2][st.col+3]=sensores.terreno[11];
					if((matriz[st.fil+3][st.col+3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+3][st.col+3]=sensores.terreno[12];
					if((matriz[st.fil+3][st.col+2]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+3][st.col+2]=sensores.terreno[13];
				

					break;

				case suroeste:

					matriz[st.fil+1][st.col]=sensores.terreno[1];
					matriz[st.fil+1][st.col-1]=sensores.terreno[2];
					matriz[st.fil][st.col-1]=sensores.terreno[3];
					matriz[st.fil+2][st.col]=sensores.terreno[4];
					matriz[st.fil+2][st.col-1]=sensores.terreno[5];

					matriz[st.fil+1][st.col-2]=sensores.terreno[7];
					matriz[st.fil][st.col-2]=sensores.terreno[8];
					matriz[st.fil+3][st.col]=sensores.terreno[9];
					matriz[st.fil+3][st.col-1]=sensores.terreno[10];
					
					matriz[st.fil+1][st.col-3]=sensores.terreno[14];
					matriz[st.fil][st.col-3]=sensores.terreno[15];

					if((matriz[st.fil+2][st.col-2]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+2][st.col-2]=sensores.terreno[6];
					if((matriz[st.fil+3][st.col-2]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+3][st.col-2]=sensores.terreno[11];
					if((matriz[st.fil+3][st.col-3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+3][st.col-3]=sensores.terreno[12];
					if((matriz[st.fil+2][st.col-3]=='?'&& sensores.nivel==3) || sensores.nivel!=3) 
						matriz[st.fil+2][st.col-3]=sensores.terreno[13];
				
					break;

	}

	
}

void ComportamientoJugador::Volcar(const Sensores& sensores){

	int diferencia=(sensores.sentido - current_state.brujula + 8)%8;
	pair<int,int> origen_girado;

	switch (diferencia)
	{
		case 2: //Girar 90º a la derecha

			origen_girado.first=current_state.fil+sensores.posC;
			origen_girado.second=current_state.col-sensores.posF;

			for(int i=3; i<mapaResultado.size()-3;i++){
				for(int j=3; j<mapaResultado.size()-3;j++){
					if(mapaResultado[i][j]=='?')
						mapaResultado[i][j]=mapaNoOrientado[origen_girado.first-j][origen_girado.second+i];
					
					mapaFrecuencias[i][j]+=mapaAuxFrecuencias[origen_girado.first-j][origen_girado.second+i];
					
				}
			}
			break;
		
		case 4: //Girar 180º

			origen_girado.first=current_state.fil+sensores.posF;
			origen_girado.second=current_state.col+sensores.posC;

			for(int i=3; i<mapaResultado.size()-3;i++){
				for(int j=3; j<mapaResultado.size()-3;j++){
					if(mapaResultado[i][j]=='?')
						mapaResultado[i][j]=mapaNoOrientado[origen_girado.first-i][origen_girado.second-j];
						
					mapaFrecuencias[i][j]+=mapaAuxFrecuencias[origen_girado.first-i][origen_girado.second-j];
					
				}
			}
			break;

		case 6: //Girar 90º a la izquierda

			origen_girado.first=current_state.fil-sensores.posC;
			origen_girado.second=current_state.col+sensores.posF;

			for(int i=3; i<mapaResultado.size()-3;i++){
				for(int j=3; j<mapaResultado.size()-3;j++){
					if(mapaResultado[i][j]=='?')
						mapaResultado[i][j]=mapaNoOrientado[origen_girado.first+j][origen_girado.second-i];
					
					mapaFrecuencias[i][j]+=mapaAuxFrecuencias[origen_girado.first+j][origen_girado.second-i];
					
				}
			}
			break;
		
		default:

			origen_girado.first=current_state.fil - sensores.posF;
			origen_girado.second=current_state.col - sensores.posC;

			for (int i=3; i<mapaResultado.size()-3; i++){
				for (int j=3; j<mapaResultado.size()-3; j++){
					if(mapaResultado[i][j]=='?')
						mapaResultado[i][j]=mapaNoOrientado[origen_girado.first+i][origen_girado.second+j];
						
					mapaFrecuencias[i][j]+=mapaAuxFrecuencias[origen_girado.first+i][origen_girado.second+j];
					
				}	
			}
			break;
	}


}


//Devuelve la posicion de una casilla especial en los sensores del agente.
int ComportamientoJugador::getCasillaEspecial(char casilla,const vector<unsigned char>& terreno){

		int idx=-1;
		bool seguir = true;

		for (int i=1; i<terreno.size() && seguir;i++)

			if (terreno[i]==casilla){

				idx=i;
				seguir=false;
				objetivo=true;
			}
			
	return idx;
}
////////////////////////////////////////////////////////////////////////////////////////


//Devuelve la posicion de la casilla con menos frecuencia.
int ComportamientoJugador::getCasillaMenosFrecuente(const vector<unsigned char>& terreno){

	vector<vector<int>>* matriz;
	if(bien_situado) matriz=&mapaFrecuencias;
	else matriz=&mapaAuxFrecuencias;

	vector<int> terreno_frecuencias=getTerrenoFrecuencias(current_state,*matriz);

	int min=(int)INFINITY;
	int casilla=-1;

	for(int i=0; i<terreno.size();i++)
		if(terreno_frecuencias[i]<min && terreno[i]!='M' && terreno[i]!='P'){
			min=terreno_frecuencias[i];
			casilla=i;
		}

	return casilla;
}
////////////////////////////////////////////////////////////////////////////////////////


 //Devuelve las frecuencias asociadas al vector de terreno.
vector<int> ComportamientoJugador::getTerrenoFrecuencias(const state& st, const vector<vector<int>>& matriz){

	vector<int> terreno;

	terreno.push_back(matriz[st.fil][st.col]);

	switch (current_state.brujula){

				case norte:

					terreno.push_back(matriz[st.fil-1][st.col-1]);
					terreno.push_back(matriz[st.fil-1][st.col]);
					terreno.push_back(matriz[st.fil-1][st.col+1]);
					terreno.push_back(matriz[st.fil-2][st.col-2]);
					terreno.push_back(matriz[st.fil-2][st.col-1]);
					terreno.push_back(matriz[st.fil-2][st.col]);
					terreno.push_back(matriz[st.fil-2][st.col+1]);
					terreno.push_back(matriz[st.fil-2][st.col+2]);
					terreno.push_back(matriz[st.fil-3][st.col-3]);
					terreno.push_back(matriz[st.fil-3][st.col-2]);
					terreno.push_back(matriz[st.fil-3][st.col-1]);
					terreno.push_back(matriz[st.fil-3][st.col]);
					terreno.push_back(matriz[st.fil-3][st.col+1]);
					terreno.push_back(matriz[st.fil-3][st.col+2]);
					terreno.push_back(matriz[st.fil-3][st.col+3]);


					break;

				case sur:

					terreno.push_back(matriz[st.fil+1][st.col+1]);
					terreno.push_back(matriz[st.fil+1][st.col]);
					terreno.push_back(matriz[st.fil+1][st.col-1]);
					terreno.push_back(matriz[st.fil+2][st.col+2]);
					terreno.push_back(matriz[st.fil+2][st.col+1]);
					terreno.push_back(matriz[st.fil+2][st.col]);
					terreno.push_back(matriz[st.fil+2][st.col-1]);
					terreno.push_back(matriz[st.fil+2][st.col-2]);
					terreno.push_back(matriz[st.fil+3][st.col+3]);
					terreno.push_back(matriz[st.fil+3][st.col+2]);
					terreno.push_back(matriz[st.fil+3][st.col+1]);
					terreno.push_back(matriz[st.fil+3][st.col]);
					terreno.push_back(matriz[st.fil+3][st.col-1]);
					terreno.push_back(matriz[st.fil+3][st.col-2]);
					terreno.push_back(matriz[st.fil+3][st.col-3]);

					break;

				case este:

					terreno.push_back(matriz[st.fil-1][st.col+1]);
					terreno.push_back(matriz[st.fil][st.col+1]);
					terreno.push_back(matriz[st.fil+1][st.col+1]);
					terreno.push_back(matriz[st.fil-2][st.col+2]);
					terreno.push_back(matriz[st.fil-1][st.col+2]);
					terreno.push_back(matriz[st.fil][st.col+2]);
					terreno.push_back(matriz[st.fil+1][st.col+2]);
					terreno.push_back(matriz[st.fil+2][st.col+2]);
					terreno.push_back(matriz[st.fil-3][st.col+3]);
					terreno.push_back(matriz[st.fil-2][st.col+3]);
					terreno.push_back(matriz[st.fil-1][st.col+3]);
					terreno.push_back(matriz[st.fil][st.col+3]);
					terreno.push_back(matriz[st.fil+1][st.col+3]);
					terreno.push_back(matriz[st.fil+2][st.col+3]);
					terreno.push_back(matriz[st.fil+3][st.col+3]);

					break;

				case oeste:

					terreno.push_back(matriz[st.fil+1][st.col-1]);
					terreno.push_back(matriz[st.fil][st.col-1]);
					terreno.push_back(matriz[st.fil-1][st.col-1]);
					terreno.push_back(matriz[st.fil+2][st.col-2]);
					terreno.push_back(matriz[st.fil+1][st.col-2]);
					terreno.push_back(matriz[st.fil][st.col-2]);
					terreno.push_back(matriz[st.fil-1][st.col-2]);
					terreno.push_back(matriz[st.fil-2][st.col-2]);
					terreno.push_back(matriz[st.fil+3][st.col-3]);
					terreno.push_back(matriz[st.fil+2][st.col-3]);
					terreno.push_back(matriz[st.fil+1][st.col-3]);
					terreno.push_back(matriz[st.fil][st.col-3]);
					terreno.push_back(matriz[st.fil-1][st.col-3]);
					terreno.push_back(matriz[st.fil-2][st.col-3]);
					terreno.push_back(matriz[st.fil-3][st.col-3]);
					break;

				case noreste:

					terreno.push_back(matriz[st.fil-1][st.col]);
					terreno.push_back(matriz[st.fil-1][st.col+1]);
					terreno.push_back(matriz[st.fil][st.col+1]);
					terreno.push_back(matriz[st.fil-2][st.col]);
					terreno.push_back(matriz[st.fil-2][st.col+1]);
					terreno.push_back(matriz[st.fil-2][st.col+2]);
					terreno.push_back(matriz[st.fil-1][st.col+2]);
					terreno.push_back(matriz[st.fil][st.col+2]);
					terreno.push_back(matriz[st.fil-3][st.col]);
					terreno.push_back(matriz[st.fil-3][st.col+1]);
					terreno.push_back(matriz[st.fil-3][st.col+2]);
					terreno.push_back(matriz[st.fil-3][st.col+3]);
					terreno.push_back(matriz[st.fil-2][st.col+3]);
					terreno.push_back(matriz[st.fil-1][st.col+3]);
					terreno.push_back(matriz[st.fil][st.col+3]);

					break;

				case noroeste:

					terreno.push_back(matriz[st.fil][st.col-1]);
					terreno.push_back(matriz[st.fil-1][st.col-1]);
					terreno.push_back(matriz[st.fil-1][st.col]);
					terreno.push_back(matriz[st.fil][st.col-2]);
					terreno.push_back(matriz[st.fil-1][st.col-2]);
					terreno.push_back(matriz[st.fil-2][st.col-2]);
					terreno.push_back(matriz[st.fil-2][st.col-1]);
					terreno.push_back(matriz[st.fil-2][st.col]);
					terreno.push_back(matriz[st.fil][st.col-3]);
					terreno.push_back(matriz[st.fil-1][st.col-3]);
					terreno.push_back(matriz[st.fil-2][st.col-3]);
					terreno.push_back(matriz[st.fil-3][st.col-3]);
					terreno.push_back(matriz[st.fil-3][st.col-2]);
					terreno.push_back(matriz[st.fil-3][st.col-1]);
					terreno.push_back(matriz[st.fil-3][st.col]);

					break;

				case sureste:

					terreno.push_back(matriz[st.fil][st.col+1]);
					terreno.push_back(matriz[st.fil+1][st.col+1]);
					terreno.push_back(matriz[st.fil+1][st.col]);
					terreno.push_back(matriz[st.fil][st.col+2]);
					terreno.push_back(matriz[st.fil+1][st.col+2]);
					terreno.push_back(matriz[st.fil+2][st.col+2]);
					terreno.push_back(matriz[st.fil+2][st.col+1]);
					terreno.push_back(matriz[st.fil+2][st.col]);
					terreno.push_back(matriz[st.fil][st.col+3]);
					terreno.push_back(matriz[st.fil+1][st.col+3]);
					terreno.push_back(matriz[st.fil+2][st.col+3]);
					terreno.push_back(matriz[st.fil+3][st.col+3]);
					terreno.push_back(matriz[st.fil+3][st.col+2]);
					terreno.push_back(matriz[st.fil+3][st.col+1]);
					terreno.push_back(matriz[st.fil+3][st.col]);

					break;

				case suroeste:

					terreno.push_back(matriz[st.fil+1][st.col]);
					terreno.push_back(matriz[st.fil+1][st.col-1]);
					terreno.push_back(matriz[st.fil][st.col-1]);
					terreno.push_back(matriz[st.fil+2][st.col]);
					terreno.push_back(matriz[st.fil+2][st.col-1]);
					terreno.push_back(matriz[st.fil+2][st.col-2]);
					terreno.push_back(matriz[st.fil+1][st.col-2]);
					terreno.push_back(matriz[st.fil][st.col-2]);
					terreno.push_back(matriz[st.fil+3][st.col]);
					terreno.push_back(matriz[st.fil+3][st.col-1]);
					terreno.push_back(matriz[st.fil+3][st.col-2]);
					terreno.push_back(matriz[st.fil+3][st.col-3]);
					terreno.push_back(matriz[st.fil+2][st.col-3]);
					terreno.push_back(matriz[st.fil+1][st.col-3]);
					terreno.push_back(matriz[st.fil][st.col-3]);

					break;

	}
	
	return terreno;
}
////////////////////////////////////////////////////////////////////////////////////////


/*Construye una cola de acciones a partir de un indice del sensor de terreno del agente para llegar hasta el.
  Solo es necesario insertar en la cola movimientos hasta quedar en frente.*/
void ComportamientoJugador::Construye_Cola(int i){
	
	if(i!=-1){

		clear(cola_acciones);

		switch (i)
		{
		case 1:
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actTURN_L);
			break;
		
		case 3:
			cola_acciones.push(actTURN_SR);
			break;
		case 4:
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actTURN_L);
			cola_acciones.push(actWALK);			
			break;
		case 5:
			cola_acciones.push(actWALK);
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actTURN_L);
			break;
		case 6:
			cola_acciones.push(actWALK);
			break;
		case 7:
			cola_acciones.push(actWALK);
			cola_acciones.push(actTURN_SR);
			break;
		case 8:
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actWALK);
			break;
		case 9:
			
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actTURN_L);
			cola_acciones.push(actWALK);
			cola_acciones.push(actWALK);
			break;
		case 10:
			cola_acciones.push(actWALK);
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actTURN_L);
			cola_acciones.push(actWALK);
			break;
		case 11:
			cola_acciones.push(actWALK);
			cola_acciones.push(actWALK);
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actTURN_L);
			break;
		case 12:
			cola_acciones.push(actWALK);
			cola_acciones.push(actWALK);
			break;
		case 13:
			cola_acciones.push(actWALK);
			cola_acciones.push(actWALK);
			cola_acciones.push(actTURN_SR);
			break;
		case 14:
			cola_acciones.push(actWALK);
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actWALK);
			break;
		case 15:
			cola_acciones.push(actTURN_SR);
			cola_acciones.push(actWALK);
			cola_acciones.push(actWALK);
			break;
		}
		
	}

}
////////////////////////////////////////////////////////////////////////////////////////


//Traza una ruta a una casilla especial a la vista del agente.
void ComportamientoJugador::Traza_Ruta(char casilla, const vector<unsigned char>& terreno){

		
	if((casilla=='K' && !bikini) || (casilla=='D' && !zapatillas) ||
		(casilla=='X' && puede_recargar) || (casilla=='G' && !bien_situado))

		Construye_Cola(getCasillaEspecial(casilla,terreno));

}
////////////////////////////////////////////////////////////////////////////////////////



//Traza una ruta a una casilla a la vista del agente.
void ComportamientoJugador::Traza_Ruta(int pos){

	if(pos!=-1) Construye_Cola(pos);

}
////////////////////////////////////////////////////////////////////////////////////////


//Vacia una cola
void ComportamientoJugador::clear(queue<Action> cola_acciones){

	while(!cola_acciones.empty()) cola_acciones.pop();
}
////////////////////////////////////////////////////////////////////////////////////////




