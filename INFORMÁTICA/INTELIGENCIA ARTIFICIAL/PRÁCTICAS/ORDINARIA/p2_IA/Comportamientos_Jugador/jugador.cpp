#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>


//Devuelve si una ubicación en el mapa es transitable para el agente
bool CasillaTransitable(const ubicacion &x, const vector<vector<unsigned char>> &mapa){
	return (mapa[x.f][x.c] != 'P' and mapa[x.f][x.c] != 'M' );
}

// Devuelve la ubicación siguiente según el avance del agente
ubicacion NextCasilla(const ubicacion &pos){
	ubicacion next = pos;
	switch (pos.brujula)
	{
	case norte:
		next.f = pos.f - 1;
		break;
	case noreste:
		next.f = pos.f - 1;
		next.c = pos.c + 1;
		break;
	case este:
		next.c = pos.c + 1;
		break;
	case sureste:
		next.f = pos.f + 1;
		next.c = pos.c + 1;
		break;
	case sur:
		next.f = pos.f + 1;
		break;
	case suroeste:
		next.f = pos.f + 1;
		next.c = pos.c - 1;
		break;
	case oeste:
		next.c = pos.c - 1;
		break;
	case noroeste:
		next.f = pos.f - 1;
		next.c = pos.c - 1;
		break;
	}

	return next;
}

//Nivel 0

/** Devuelve el estado que se genera si el agente puede avanzar.
 * Si no puede avanzar, devuelve como salida el mismo estado de entrada
*/
stateN0 apply(const Action &a, const stateN0 &st, const vector<vector<unsigned char>>& mapa){
	stateN0 st_result = st;
	ubicacion sig_ubicacion, sig_ubicacion2;
	switch (a)
	{
	case actWALK: //si prox casilla es transitable y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){
				st_result.jugador = sig_ubicacion;
			}
		break;
	
	case actRUN: //si prox 2 casillas son transitables y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){
				sig_ubicacion2 = NextCasilla(sig_ubicacion);
				if (CasillaTransitable(sig_ubicacion2, mapa) and 
					!(sig_ubicacion2.f == st.colaborador.f and sig_ubicacion2.c == st.colaborador.c)){
						st_result.jugador = sig_ubicacion2;
				}
			}
		break;

	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+6)%8);
		break;

	case actTURN_SR:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+1)%8);
		break;
	}
	return st_result;
}


//Encuentra si el elemento item está en la lista
bool Find(const stateN0 &item, const list<stateN0> &lista){
	auto it = lista.begin();
	while (it != lista.end() and !((*it) == item))
		it++;

	return (!(it == lista.end()));
}


//Sobrecarga de la función Find para buscar en lista de nodeN0
bool Find(const stateN0 &item, const list<nodeN0> &lista){
	auto it = lista.begin();
	while (it != lista.end() and !(it->st == item))
		it++;

	return (!(it == lista.end()));
}

//Primera aproximacion de la busqueda en anchura: Nivel 0
bool AnchuraSoloJugador(const stateN0 &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa)
{
	stateN0 current_state = inicio;
	list<stateN0> frontier;
	list<stateN0> explored;
	frontier.push_back(current_state);
	bool SolutionFound = (current_state.jugador.f == final.f and
						current_state.jugador.c == final.c);

	while (!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.push_back(current_state);

		// Generar hijo actWALK
		stateN0 child_walk = apply(actWALK, current_state, mapa);
		if (child_walk.jugador.f == final.f and child_walk.jugador.c == final.c){
			current_state = child_walk;
			SolutionFound = true;
		}
		else if (!Find(child_walk, frontier) and !Find(child_walk, explored)){
			frontier.push_back(child_walk);
		}

		if (!SolutionFound){
			// Generar hijo actRUN
			stateN0 child_run = apply(actRUN, current_state, mapa);
			if (child_run.jugador.f == final.f and child_run.jugador.c == final.c){
				current_state = child_run;
				SolutionFound = true;
			}
			else if (!Find(child_run, frontier) and !Find(child_run, explored)){
				frontier.push_back(child_run);
			}
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			stateN0 child_turnl = apply(actTURN_L, current_state, mapa);
			if (!Find(child_turnl, frontier) and !Find(child_turnl, explored)){
				frontier.push_back(child_turnl);
			}		
			// Generar hijo actTURN_SR
			stateN0 child_turnsr = apply(actTURN_SR, current_state, mapa);
			if (!Find(child_turnsr, frontier) and !Find(child_turnsr, explored)){
				frontier.push_back(child_turnsr);
			}		
		}

		if (!SolutionFound and !frontier.empty())
			current_state = frontier.front();
	}
	
	return SolutionFound;
}



//Segunda aproximacion de la busqueda en anchura: Nivel 0
list<Action> AnchuraSoloJugador_V2(const stateN0 &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa)
{
	nodeN0 current_node;
	list<nodeN0> frontier;
	list<nodeN0> explored;
	list<Action> plan;
	current_node.st= inicio;
	frontier.push_back(current_node);

	bool SolutionFound = (current_node.st.jugador.f == final.f and
							current_node.st.jugador.c == final.c);

	while (!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.push_back(current_node);

		// Generar hijo actWALK
		nodeN0 child_walk = current_node;
		child_walk.st = apply(actWALK, current_node.st, mapa);
		child_walk.secuencia.push_back(actWALK);

		if (child_walk.st.jugador.f == final.f and child_walk.st.jugador.c == final.c){
			current_node = child_walk;
			SolutionFound = true;
		}
		else if (!Find(child_walk.st, frontier) and !Find(child_walk.st, explored)){
			frontier.push_back(child_walk);
		}

		if (!SolutionFound){
			// Generar hijo actRUN
			nodeN0 child_run = current_node;
			child_run.st = apply(actRUN, current_node.st, mapa);
			child_run.secuencia.push_back(actRUN);

			if (child_run.st.jugador.f == final.f and child_run.st.jugador.c == final.c){
				current_node = child_run;
				SolutionFound = true;
			}
			else if (!Find(child_run.st, frontier) and !Find(child_run.st, explored)){
				frontier.push_back(child_run);
			}
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN0 child_turnl = current_node;
			child_turnl.st = apply(actTURN_L, current_node.st, mapa);
			child_turnl.secuencia.push_back(actTURN_L);

			if (!Find(child_turnl.st, frontier) and !Find(child_turnl.st, explored)){
				frontier.push_back(child_turnl);
			}		
			// Generar hijo actTURN_SR
			nodeN0 child_turnsr = current_node;
			child_turnsr.st = apply(actTURN_SR, current_node.st, mapa);
			child_turnsr.secuencia.push_back(actTURN_SR);

			if (!Find(child_turnsr.st, frontier) and !Find(child_turnsr.st, explored)){
				frontier.push_back(child_turnsr);
			}		
		}

		if (!SolutionFound and !frontier.empty())
			current_node = frontier.front();
	}

	if(SolutionFound) plan = current_node.secuencia;
	
	return plan;
}

//Tercera aproximacion de la busqueda en anchura: Nivel 0
list<Action> AnchuraSoloJugador_V3(const stateN0 &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa)
{
	nodeN0 current_node;
	list<nodeN0> frontier;
	set<nodeN0> explored;
	list<Action> plan;
	current_node.st= inicio;
	frontier.push_back(current_node);

	bool SolutionFound = (current_node.st.jugador.f == final.f and
							current_node.st.jugador.c == final.c);

	while (!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);

		// Generar hijo actWALK
		nodeN0 child_walk = current_node;
		child_walk.st = apply(actWALK, current_node.st, mapa);
		child_walk.secuencia.push_back(actWALK);

		if (child_walk.st.jugador.f == final.f and child_walk.st.jugador.c == final.c){
			current_node = child_walk;
			SolutionFound = true;
		}
		else if (explored.find(child_walk) == explored.end()){
			frontier.push_back(child_walk);
		}

		if (!SolutionFound){
			// Generar hijo actRUN
			nodeN0 child_run = current_node;
			child_run.st = apply(actRUN, current_node.st, mapa);
			child_run.secuencia.push_back(actRUN);

			if (child_run.st.jugador.f == final.f and child_run.st.jugador.c == final.c){
				current_node = child_run;
				SolutionFound = true;
			}
			else if (explored.find(child_run) == explored.end()){
				frontier.push_back(child_run);
			}
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN0 child_turnl = current_node;
			child_turnl.st = apply(actTURN_L, current_node.st, mapa);
			child_turnl.secuencia.push_back(actTURN_L);

			if (explored.find(child_turnl) == explored.end()){
				frontier.push_back(child_turnl);
			}		
			// Generar hijo actTURN_SR
			nodeN0 child_turnsr = current_node;
			child_turnsr.st = apply(actTURN_SR, current_node.st, mapa);
			child_turnsr.secuencia.push_back(actTURN_SR);

			if (explored.find(child_turnsr) == explored.end()){
				frontier.push_back(child_turnsr);
			}		
		}

		if (!SolutionFound and !frontier.empty()){
			current_node = frontier.front();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if(!frontier.empty()) 
					current_node=frontier.front();
			}
		}
	}
	
	if(SolutionFound) plan = current_node.secuencia;
	
	return plan;
}



//////////////////////////////////////////////////////////////////////////////////////////////////

//Nivel 1


//Funcion que indica si el jugador ve al colaborador.
bool Ve_CLB(const ubicacion& jugador, const ubicacion& colaborador){

	
	bool ve_clb=false;
	switch (jugador.brujula)
	{
		case norte:
			for(int i=0; i<3 && !ve_clb; i++)
				if(colaborador.f==jugador.f-1 && colaborador.c==jugador.c+i-1)
					ve_clb=true;

			for(int i=0; i<5 && !ve_clb; i++)
				if(colaborador.f==jugador.f-2 && colaborador.c==jugador.c+i-2)
					ve_clb=true;

			for(int i=0; i<7 && !ve_clb; i++)
				if(colaborador.f==jugador.f-3 && colaborador.c==jugador.c+i-3)
					ve_clb=true;
			break;

		case sur:

			for(int i=0; i<3 && !ve_clb; i++)
				if(colaborador.f==jugador.f+1 && colaborador.c==jugador.c+i-1)
					ve_clb=true;

			for(int i=0; i<5 && !ve_clb; i++)
				if(colaborador.f==jugador.f+2 && colaborador.c==jugador.c+i-2)
					ve_clb=true;

			for(int i=0; i<7 && !ve_clb; i++)
				if(colaborador.f==jugador.f+3 && colaborador.c==jugador.c+i-3)
					ve_clb=true;
			break;

		case este:

			for(int i=0; i<3 && !ve_clb; i++)
				if(colaborador.f==jugador.f-1+i && colaborador.c==jugador.c+1)
					ve_clb=true;

			for(int i=0; i<5 && !ve_clb; i++)
				if(colaborador.f==jugador.f-2+i && colaborador.c==jugador.c+2)
					ve_clb=true;

			for(int i=0; i<7 && !ve_clb; i++)
				if(colaborador.f==jugador.f-3+i && colaborador.c==jugador.c+3)
					ve_clb=true;
			break;

			case oeste:

			for(int i=0; i<3 && !ve_clb; i++)
				if(colaborador.f==jugador.f-1+i && colaborador.c==jugador.c-1)
					ve_clb=true;

			for(int i=0; i<5 && !ve_clb; i++)
				if(colaborador.f==jugador.f-2+i && colaborador.c==jugador.c-2)
					ve_clb=true;

			for(int i=0; i<7 && !ve_clb; i++)
				if(colaborador.f==jugador.f-3+i && colaborador.c==jugador.c-3)
					ve_clb=true;
			break;
		
		case noreste:

			for(int i=0; i<4 && !ve_clb; i++)
				for(int j=0; j<4 && !ve_clb; j++)
					if(colaborador.f==jugador.f-i && colaborador.c==jugador.c+j)
						ve_clb=true;
			break;

		case suroeste:

			for(int i=0; i<4 && !ve_clb; i++)
				for(int j=0; j<4 && !ve_clb; j++)
					if(colaborador.f==jugador.f+i && colaborador.c==jugador.c-j)
						ve_clb=true;
			break;

		case noroeste:

			for(int i=0; i<4 && !ve_clb; i++)
				for(int j=0; j<4 && !ve_clb; j++)
					if(colaborador.f==jugador.f-i && colaborador.c==jugador.c-j)
						ve_clb=true;
			break;

		case sureste:

			for(int i=0; i<4 && !ve_clb; i++)
				for(int j=0; j<4 && !ve_clb; j++)
					if(colaborador.f==jugador.f+i && colaborador.c==jugador.c+j)
						ve_clb=true;
			break;
		
		
	}
	

	return ve_clb;
}

stateN1 apply1(const Action &a, const stateN1 &st, const vector<vector<unsigned char>>& mapa){
	stateN1 st_result = st;
	ubicacion sig_ubicacion, sig_ubicacion2;
	bool mover_clb=false;

	switch (a)
	{
	case actWALK: //si prox casilla es transitable y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){

			st_result.jugador = sig_ubicacion;
			mover_clb=true;
		}
		break;
	
	case actRUN: //si prox 2 casillas son transitables y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){
			
			sig_ubicacion2 = NextCasilla(sig_ubicacion);
			if (CasillaTransitable(sig_ubicacion2, mapa) and 
				!(sig_ubicacion2.f == st.colaborador.f and sig_ubicacion2.c == st.colaborador.c)){
				
				st_result.jugador = sig_ubicacion2;
				mover_clb=true;
			}
		}
		break;

	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+6)%8);
		mover_clb=true;
		break;

	case actTURN_SR:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+1)%8);
		mover_clb=true;
		break;

	case act_CLB_WALK:
		sig_ubicacion = NextCasilla(st.colaborador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.jugador.f and sig_ubicacion.c == st.jugador.c)){
				st_result.colaborador = sig_ubicacion;
				st_result.ultimaOrdenColaborador=act_CLB_WALK;
			}
		
		break;

	case act_CLB_TURN_SR:
		st_result.colaborador.brujula = (Orientacion)((st_result.colaborador.brujula+1)%8);
		st_result.ultimaOrdenColaborador=act_CLB_TURN_SR;
		break;

	case act_CLB_STOP:
		st_result.ultimaOrdenColaborador=act_CLB_STOP;
		break;
	}

	//El CLB se mueve simultanemente.
	if(st.ultimaOrdenColaborador!=act_CLB_STOP && mover_clb){
		
		switch (st.ultimaOrdenColaborador)
		{
		case act_CLB_WALK:
			sig_ubicacion = NextCasilla(st.colaborador);
			if (CasillaTransitable(sig_ubicacion, mapa) and 
				!(sig_ubicacion.f == st_result.jugador.f and sig_ubicacion.c == st_result.jugador.c)){
				
				st_result.colaborador = sig_ubicacion;
			}
			
			else st_result.jugador = st.jugador;
			
			break;
		
		case act_CLB_TURN_SR:
			st_result.colaborador.brujula = (Orientacion)((st_result.colaborador.brujula+1)%8);
			break;
		}
	}

	return st_result;
}


list<Action> AnchuraJugadorColaborador(const stateN1 &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa)
{
	nodeN1 current_node;
	list<nodeN1> frontier;
	set<nodeN1> explored;
	list<Action> plan;
	bool ve_clb=false;
	current_node.st= inicio;
	frontier.push_back(current_node);

	bool SolutionFound = (current_node.st.colaborador.f == final.f and
							current_node.st.colaborador.c == final.c);

	while (!frontier.empty() and !SolutionFound){
		frontier.pop_front();
		explored.insert(current_node);

		ve_clb=Ve_CLB(current_node.st.jugador,current_node.st.colaborador);
		if(ve_clb){

			// Generar hijo act_CLB_WALK
			nodeN1 child_clb_walk = current_node;
			child_clb_walk.st = apply1(act_CLB_WALK, current_node.st, mapa);
			child_clb_walk.secuencia.push_back(act_CLB_WALK);
			

			if (child_clb_walk.st.colaborador.f == final.f and child_clb_walk.st.colaborador.c == final.c){
				current_node = child_clb_walk;
				SolutionFound = true;
			}
			else if (explored.find(child_clb_walk) == explored.end()){
				frontier.push_back(child_clb_walk);
			}

			if(!SolutionFound){

				// Generar hijo act_CLB_TURN_SR
				nodeN1 child_clb_turnsr = current_node;
				child_clb_turnsr.st = apply1(act_CLB_TURN_SR, current_node.st, mapa);
				child_clb_turnsr.secuencia.push_back(act_CLB_TURN_SR);

				if (explored.find(child_clb_turnsr) == explored.end()){
					frontier.push_back(child_clb_turnsr);
				}	
			}

			if(!SolutionFound){

				// Generar hijo act_CLB_STOP
				nodeN1 child_clb_stop = current_node;
				child_clb_stop.st = apply1(act_CLB_STOP, current_node.st, mapa);
				child_clb_stop.secuencia.push_back(act_CLB_STOP);

				if (explored.find(child_clb_stop) == explored.end()){
					frontier.push_back(child_clb_stop);
				}	
			}
		}
		
		if(!SolutionFound){
			// Generar hijo actWALK
			nodeN1 child_walk = current_node;
			child_walk.st = apply1(actWALK, current_node.st, mapa);
			child_walk.secuencia.push_back(actWALK);

			if (child_walk.st.colaborador.f == final.f and child_walk.st.colaborador.c == final.c){
				current_node = child_walk;
				SolutionFound = true;
			}

			else if (explored.find(child_walk) == explored.end()){
				frontier.push_back(child_walk);
			}
		}

		if (!SolutionFound){
			// Generar hijo actRUN
			nodeN1 child_run = current_node;
			child_run.st = apply1(actRUN, current_node.st, mapa);
			child_run.secuencia.push_back(actRUN);

			if (child_run.st.colaborador.f == final.f and child_run.st.colaborador.c == final.c){
				current_node = child_run;
				SolutionFound = true;
			}
			else if (explored.find(child_run) == explored.end()){
				frontier.push_back(child_run);
			}
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN1 child_turnl = current_node;
			child_turnl.st = apply1(actTURN_L, current_node.st, mapa);
			child_turnl.secuencia.push_back(actTURN_L);

			if(child_turnl.st.colaborador.f == final.f and child_turnl.st.colaborador.c == final.c){
				current_node = child_turnl;
				SolutionFound = true;
			}

			else if (explored.find(child_turnl) == explored.end()){
				frontier.push_back(child_turnl);
			}	

		}

		if (!SolutionFound){	
			// Generar hijo actTURN_SR
			nodeN1 child_turnsr = current_node;
			child_turnsr.st = apply1(actTURN_SR, current_node.st, mapa);
			child_turnsr.secuencia.push_back(actTURN_SR);

			if(child_turnsr.st.colaborador.f == final.f and child_turnsr.st.colaborador.c == final.c){
				current_node = child_turnsr;
				SolutionFound = true;
			}

			else if (explored.find(child_turnsr) == explored.end()){
				frontier.push_back(child_turnsr);
			}		
		}
		
		
		if (!SolutionFound and !frontier.empty()){
			current_node = frontier.front();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop_front();
				if(!frontier.empty()) 
					current_node=frontier.front();
			}
		}
	}

	if(SolutionFound) plan = current_node.secuencia;
	
	return plan;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

//Nivel 2

//Proporciona el coste de accion accion segun el terreno.
int FuncionCosteN2(const Action& a, const stateN2& st,const vector<vector<unsigned char>>& mapa){
	int coste=0;
	
	switch (a)
	{
	case actWALK:
		switch (mapa[st.jugador.f][st.jugador.c])
		{
		case 'A':
			coste=100;
			if(st.bikini) coste=10;
			break;
		case 'B':
			coste=50;
			if(st.zapatillas) coste=15;
			break;
		case 'T':
			coste=2;
			break;
		
		default:
			coste=1;
			break;
		}
		break;
	
	case actRUN:
		switch (mapa[st.jugador.f][st.jugador.c])
		{
		case 'A':
			coste=150;
			if(st.bikini) coste=15;
			break;
		case 'B':
			coste=75;
			if(st.zapatillas) coste=25;
			break;
		case 'T':
			coste=3;
			break;
		
		default:
			coste=1;
			break;
		}
		break;

	case actTURN_L:
		switch (mapa[st.jugador.f][st.jugador.c])
		{
		case 'A':
			coste=30;
			if(st.bikini) coste=5;
			break;
		case 'B':
			coste=7;
			if(st.zapatillas) coste=1;
			break;
		case 'T':
			coste=2;
			break;
		
		default:
			coste=1;
			break;
		}
		break;

	case actTURN_SR:
		switch (mapa[st.jugador.f][st.jugador.c])
		{
		case 'A':
			coste=10;
			if(st.bikini) coste=2;
			break;
		case 'B':
			coste=5;
			if(st.zapatillas) coste=1;
			break;
		case 'T':
			coste=1;
			break;
		
		default:
			coste=1;
			break;
		}
		break;
	}
	return coste;
}


stateN2 apply2(const Action &a, const stateN2 &st, const vector<vector<unsigned char>>& mapa){
	stateN2 st_result = st;
	ubicacion sig_ubicacion, sig_ubicacion2;
	switch (a)
	{
	case actWALK: //si prox casilla es transitable y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){
				st_result.jugador = sig_ubicacion;
			}
		break;
	
	case actRUN: //si prox 2 casillas son transitables y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){
				sig_ubicacion2 = NextCasilla(sig_ubicacion);
				if (CasillaTransitable(sig_ubicacion2, mapa) and 
					!(sig_ubicacion2.f == st.colaborador.f and sig_ubicacion2.c == st.colaborador.c)){
						st_result.jugador = sig_ubicacion2;
				}
			}
		break;

	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+6)%8);
		break;

	case actTURN_SR:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+1)%8);
		break;
	}

	if(mapa[st_result.jugador.f][st_result.jugador.c]=='K'){
		st_result.bikini=true;
		st_result.zapatillas=false;
	}
	else if(mapa[st_result.jugador.f][st_result.jugador.c]=='D'){

		st_result.zapatillas=true;
		st_result.bikini=false;
	} 

	return st_result;
}


list<Action> DijkstraSoloJugador(const stateN2 &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa)
{
	nodeN2 current_node;
	priority_queue<nodeN2,vector<nodeN2>,comparadorN2> frontier;
	set<nodeN2> explored;
	list<Action> plan;

	current_node.st= inicio;
	current_node.bateria=0;
	if(mapa[current_node.st.jugador.f][current_node.st.jugador.c]=='K') current_node.st.bikini=true;
	else if(mapa[current_node.st.jugador.f][current_node.st.jugador.c]=='D') current_node.st.zapatillas=true;

	frontier.push(current_node);
	
	bool SolutionFound = (current_node.st.jugador.f == final.f and
							current_node.st.jugador.c == final.c);

	while (!frontier.empty() and !SolutionFound){
		frontier.pop();
		explored.insert(current_node);

		if (current_node.st.jugador.f == final.f and current_node.st.jugador.c == final.c) 
			SolutionFound = true;
	

		// Generar hijo actWALK
		nodeN2 child_walk = current_node;
		child_walk.st = apply2(actWALK, current_node.st, mapa);
		child_walk.secuencia.push_back(actWALK);
		child_walk.bateria+=FuncionCosteN2(actWALK, current_node.st, mapa);

		if (explored.find(child_walk) == explored.end()){
			frontier.push(child_walk);
		}

		if (!SolutionFound){
			// Generar hijo actRUN
			nodeN2 child_run = current_node;
			child_run.st = apply2(actRUN, current_node.st, mapa);
			child_run.secuencia.push_back(actRUN);
			child_run.bateria+=FuncionCosteN2(actRUN, current_node.st, mapa);

			if (explored.find(child_run) == explored.end()){
				frontier.push(child_run);
			}
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN2 child_turnl = current_node;
			child_turnl.st = apply2(actTURN_L, current_node.st, mapa);
			child_turnl.secuencia.push_back(actTURN_L);
			child_turnl.bateria+=FuncionCosteN2(actTURN_L, current_node.st, mapa);

			if (explored.find(child_turnl) == explored.end()){
				frontier.push(child_turnl);
			}
			
			// Generar hijo actTURN_SR
			nodeN2 child_turnsr = current_node;
			child_turnsr.st = apply2(actTURN_SR, current_node.st, mapa);
			child_turnsr.secuencia.push_back(actTURN_SR);
			child_turnsr.bateria+=FuncionCosteN2(actTURN_SR, current_node.st, mapa);

			if (explored.find(child_turnsr) == explored.end()){
				frontier.push(child_turnsr);
			}		
		}

		if (!SolutionFound and !frontier.empty()){
			current_node = frontier.top();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop();
				if(!frontier.empty()) 
					current_node=frontier.top();
			}
		}
	}
	
	if(SolutionFound) plan = current_node.secuencia;
	
	return plan;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

//Nivel 3

//Funcion de coste para el nivel 3
int FuncionCosteN3(const Action& a, const stateN3& st,const vector<vector<unsigned char>>& mapa){
	int coste=0;
	
	switch (a)
	{
	case actWALK:
		switch (mapa[st.jugador.f][st.jugador.c])
		{
		case 'A':
			coste=100;
			if(st.bikini) coste=10;
			break;
		case 'B':
			coste=50;
			if(st.zapatillas) coste=15;
			break;
		case 'T':
			coste=2;
			break;
		
		default:
			coste=1;
			break;
		}
		break;
	
	case actRUN:
		switch (mapa[st.jugador.f][st.jugador.c])
		{
		case 'A':
			coste=150;
			if(st.bikini) coste=15;
			break;
		case 'B':
			coste=75;
			if(st.zapatillas) coste=25;
			break;
		case 'T':
			coste=3;
			break;
		
		default:
			coste=1;
			break;
		}
		break;

	case actTURN_L:
		switch (mapa[st.jugador.f][st.jugador.c])
		{
		case 'A':
			coste=30;
			if(st.bikini) coste=5;
			break;
		case 'B':
			coste=7;
			if(st.zapatillas) coste=1;
			break;
		case 'T':
			coste=2;
			break;
		
		default:
			coste=1;
			break;
		}
		break;

	case actTURN_SR:
		switch (mapa[st.jugador.f][st.jugador.c])
		{
		case 'A':
			coste=10;
			if(st.bikini) coste=2;
			break;
		case 'B':
			coste=5;
			if(st.zapatillas) coste=1;
			break;
		case 'T':
			coste=1;
			break;
		
		default:
			coste=1;
			break;
		}
		break;

	case act_CLB_WALK:
		switch (mapa[st.colaborador.f][st.colaborador.c])
		{
		case 'A':
			coste=100;
			if(st.bikini_clb) coste=10;
			break;
		case 'B':
			coste=50;
			if(st.zapatillas_clb) coste=15;
			break;
		case 'T':
			coste=2;
			break;
		
		default:
			coste=1;
			break;
		}
		break;

	case act_CLB_TURN_SR:
		switch (mapa[st.colaborador.f][st.colaborador.c])
		{
		case 'A':
			coste=10;
			if(st.bikini_clb) coste=2;
			break;
		case 'B':
			coste=5;
			if(st.zapatillas_clb) coste=1;
			break;
		case 'T':
			coste=1;
			break;
		
		default:
			coste=1;
			break;
		}
		break;
	}
	return coste;
}

//Heuristica de Chebyshev
int Heuristica(const ubicacion& a, const ubicacion& b){
	int dx = abs(a.f - b.f);
	int dy = abs(a.c - b.c);
	return max(dx,dy);
}


stateN3 apply3(const Action &a, const stateN3 &st, const vector<vector<unsigned char>>& mapa){
	stateN3 st_result = st;
	ubicacion sig_ubicacion, sig_ubicacion2;
	bool mover_clb=false;

	switch (a)
	{
	case actWALK: //si prox casilla es transitable y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){

			st_result.jugador = sig_ubicacion;
			mover_clb=true;
		}
		break;
	
	case actRUN: //si prox 2 casillas son transitables y no está ocupada por el colaborador
		sig_ubicacion = NextCasilla(st.jugador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.colaborador.f and sig_ubicacion.c == st.colaborador.c)){
			
			sig_ubicacion2 = NextCasilla(sig_ubicacion);
			if (CasillaTransitable(sig_ubicacion2, mapa) and 
				!(sig_ubicacion2.f == st.colaborador.f and sig_ubicacion2.c == st.colaborador.c)){
				
				st_result.jugador = sig_ubicacion2;
				mover_clb=true;
			}
		}
		break;

	case actTURN_L:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+6)%8);
		mover_clb=true;
		break;

	case actTURN_SR:
		st_result.jugador.brujula = static_cast<Orientacion>((st_result.jugador.brujula+1)%8);
		mover_clb=true;
		break;

	case actIDLE:
		mover_clb=true;
		break;

	case act_CLB_WALK:
		sig_ubicacion = NextCasilla(st.colaborador);
		if (CasillaTransitable(sig_ubicacion, mapa) and 
			!(sig_ubicacion.f == st.jugador.f and sig_ubicacion.c == st.jugador.c)){
				st_result.colaborador = sig_ubicacion;
				st_result.ultimaOrdenColaborador=act_CLB_WALK;
			}
		break;

	case act_CLB_TURN_SR:
		st_result.colaborador.brujula = (Orientacion)((st_result.colaborador.brujula+1)%8);
		st_result.ultimaOrdenColaborador=act_CLB_TURN_SR;
		break;

	case act_CLB_STOP:
		st_result.ultimaOrdenColaborador=act_CLB_STOP;
		break;
	}

	//El CLB se mueve simultanemente.
	if(st.ultimaOrdenColaborador!=act_CLB_STOP && mover_clb){
		
		switch (st.ultimaOrdenColaborador)
		{
		case act_CLB_WALK:
			sig_ubicacion = NextCasilla(st.colaborador);
			if (CasillaTransitable(sig_ubicacion, mapa) and 
				!(sig_ubicacion.f == st_result.jugador.f and sig_ubicacion.c == st_result.jugador.c)){
				
				st_result.colaborador = sig_ubicacion;
			}
			else st_result.jugador = st.jugador;
			
			break;
		
		case act_CLB_TURN_SR:
			st_result.colaborador.brujula = (Orientacion)((st_result.colaborador.brujula+1)%8);
			break;
		}
		
	}


	if(mapa[st_result.jugador.f][st_result.jugador.c]=='K'){
		st_result.bikini=true;
		st_result.zapatillas=false;
	}
	else if(mapa[st_result.jugador.f][st_result.jugador.c]=='D'){

		st_result.zapatillas=true;
		st_result.bikini=false;
	}

	if(mapa[st_result.colaborador.f][st_result.colaborador.c]=='K'){
		st_result.bikini_clb=true;
		st_result.zapatillas_clb=false;
	}
	else if(mapa[st_result.colaborador.f][st_result.colaborador.c]=='D'){

		st_result.zapatillas_clb=true;
		st_result.bikini_clb=false;
	}

	return st_result;
}


list<Action> AEstrellaJugadorColaborador(const stateN3 &inicio, const ubicacion &final,
						const vector<vector<unsigned char>> &mapa)
{
	nodeN3 current_node;
	priority_queue<nodeN3,vector<nodeN3>,comparadorN3> frontier;
	set<nodeN3> explored;
	list<Action> plan;
	bool ve_clb=false;
	current_node.st= inicio;

	if(mapa[current_node.st.jugador.f][current_node.st.jugador.c]=='K') current_node.st.bikini=true;
	else if(mapa[current_node.st.jugador.f][current_node.st.jugador.c]=='D') current_node.st.zapatillas=true;

	if(mapa[current_node.st.colaborador.f][current_node.st.colaborador.c]=='K') current_node.st.bikini_clb=true;
	else if(mapa[current_node.st.colaborador.f][current_node.st.colaborador.c]=='D') current_node.st.zapatillas_clb=true;

	frontier.push(current_node);

	bool SolutionFound = (current_node.st.colaborador.f == final.f and
							current_node.st.colaborador.c == final.c);

	while (!frontier.empty() and !SolutionFound){
		frontier.pop();
		explored.insert(current_node);

		if(current_node.st.colaborador.f == final.f and current_node.st.colaborador.c == final.c)
			SolutionFound = true;
		

		ve_clb=Ve_CLB(current_node.st.jugador,current_node.st.colaborador);
		if(ve_clb){

			// Generar hijo act_CLB_WALK
			nodeN3 child_clb_walk = current_node;
			child_clb_walk.st = apply3(act_CLB_WALK, current_node.st, mapa);
			child_clb_walk.secuencia.push_back(act_CLB_WALK);
			child_clb_walk.bateria+=FuncionCosteN3(act_CLB_WALK, current_node.st, mapa);
			child_clb_walk.bateria_heuristica=Heuristica(child_clb_walk.st.colaborador,final);
			
			if (explored.find(child_clb_walk) == explored.end()){
				frontier.push(child_clb_walk);
			}

			if(!SolutionFound){

				// Generar hijo act_CLB_TURN_SR
				nodeN3 child_clb_turnsr = current_node;
				child_clb_turnsr.st = apply3(act_CLB_TURN_SR, current_node.st, mapa);
				child_clb_turnsr.secuencia.push_back(act_CLB_TURN_SR);
				child_clb_turnsr.bateria+=FuncionCosteN3(act_CLB_TURN_SR, current_node.st, mapa);
				child_clb_turnsr.bateria_heuristica=Heuristica(child_clb_turnsr.st.colaborador,final);

				if (explored.find(child_clb_turnsr) == explored.end()){
					frontier.push(child_clb_turnsr);
				}	
			}

			if(!SolutionFound){
				// Generar hijo act_CLB_STOP
				nodeN3 child_clb_stop = current_node;
				child_clb_stop.st = apply3(act_CLB_STOP, current_node.st, mapa);
				child_clb_stop.secuencia.push_back(act_CLB_STOP);
				child_clb_stop.bateria_heuristica=Heuristica(child_clb_stop.st.colaborador,final);

				if (explored.find(child_clb_stop) == explored.end()){
					frontier.push(child_clb_stop);
				}	
			}
		}
		
		if(!SolutionFound){
			// Generar hijo actWALK
			nodeN3 child_walk = current_node;
			child_walk.st = apply3(actWALK, current_node.st, mapa);
			child_walk.secuencia.push_back(actWALK);
			child_walk.bateria+=FuncionCosteN3(actWALK, current_node.st, mapa) + FuncionCosteN3(current_node.st.ultimaOrdenColaborador, current_node.st, mapa);
			child_walk.bateria_heuristica=Heuristica(child_walk.st.colaborador,final);

			if (explored.find(child_walk) == explored.end()){
				frontier.push(child_walk);
			}
		}

		if (!SolutionFound){
			// Generar hijo actRUN
			nodeN3 child_run = current_node;
			child_run.st = apply3(actRUN, current_node.st, mapa);
			child_run.secuencia.push_back(actRUN);
			child_run.bateria+=FuncionCosteN3(actRUN, current_node.st, mapa) + FuncionCosteN3(current_node.st.ultimaOrdenColaborador, current_node.st, mapa);
			child_run.bateria_heuristica=Heuristica(child_run.st.colaborador,final);

			if (explored.find(child_run) == explored.end()){
				frontier.push(child_run);
			}
		}

		if (!SolutionFound){
			// Generar hijo actTURN_L
			nodeN3 child_turnl = current_node;
			child_turnl.st = apply3(actTURN_L, current_node.st, mapa);
			child_turnl.secuencia.push_back(actTURN_L);
			child_turnl.bateria+=FuncionCosteN3(actTURN_L, current_node.st, mapa) + FuncionCosteN3(current_node.st.ultimaOrdenColaborador, current_node.st, mapa);
			child_turnl.bateria_heuristica=Heuristica(child_turnl.st.colaborador,final);

			if (explored.find(child_turnl) == explored.end()){
				frontier.push(child_turnl);
			}	

		}

		if (!SolutionFound){	
			// Generar hijo actTURN_SR
			nodeN3 child_turnsr = current_node;
			child_turnsr.st = apply3(actTURN_SR, current_node.st, mapa);
			child_turnsr.secuencia.push_back(actTURN_SR);
			child_turnsr.bateria+=FuncionCosteN3(actTURN_SR, current_node.st, mapa) + FuncionCosteN3(current_node.st.ultimaOrdenColaborador, current_node.st, mapa);
			child_turnsr.bateria_heuristica=Heuristica(child_turnsr.st.colaborador,final);

			if (explored.find(child_turnsr) == explored.end()){
				frontier.push(child_turnsr);
			}		
		}
		
		if(!SolutionFound){
			// Generar hijo actIDLE
			nodeN3 child_idle = current_node;
			child_idle.st = apply3(actIDLE, current_node.st, mapa);
			child_idle.secuencia.push_back(actIDLE);
			child_idle.bateria+=FuncionCosteN3(current_node.st.ultimaOrdenColaborador, current_node.st, mapa);
			child_idle.bateria_heuristica=Heuristica(child_idle.st.colaborador,final);

			if (explored.find(child_idle) == explored.end()){
				frontier.push(child_idle);
			}
		}
		
		if (!SolutionFound and !frontier.empty()){
			current_node = frontier.top();
			while(!frontier.empty() and explored.find(current_node) != explored.end()){
				frontier.pop();
				if(!frontier.empty()) 
					current_node=frontier.top();
			}
		}
	}

	if(SolutionFound) plan = current_node.secuencia;
	
	return plan;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//Nivel 4
//////////////////////////////////////////////////////////////////////////////////////////////////

//Pintado del plan

// pone a cero todos los elementos de una matriz
void AnulaMatriz(vector<vector<unsigned char> > &matriz){
	for (int i = 0; i < matriz.size(); i++)
		for (int j = 0; j < matriz[0].size(); j++)
			matriz[i][j] = 0;
}


//Permite pintar sobre el mapa del simulador el plan partiendo desde el estado st
void ComportamientoJugador::VisualizaPlan(const stateN0 &st, const list<Action> &plan)
{
	AnulaMatriz(mapaConPlan);
	stateN0 cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if ((*it != act_CLB_WALK) and (*it != act_CLB_TURN_SR) and (*it != act_CLB_STOP))
		{
			switch (cst.ultimaOrdenColaborador)
			{
			case act_CLB_WALK:
				cst.colaborador = NextCasilla(cst.colaborador);
				mapaConPlan[cst.colaborador.f][cst.colaborador.c] = 2;
				break;
			case act_CLB_TURN_SR:
				cst.colaborador.brujula = (Orientacion)((cst.colaborador.brujula + 1) % 8);
				break;
			}
		}

		switch (*it)
		{
		case actRUN:
			cst.jugador = NextCasilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 3;
			cst.jugador = NextCasilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
			break;
		case actWALK:
			cst.jugador = NextCasilla(cst.jugador);
			mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
			break;
		case actTURN_SR:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 1) % 8);
			break;
		case actTURN_L:
			cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
			break;
		case act_CLB_WALK:
			cst.colaborador = NextCasilla(cst.colaborador);
			cst.ultimaOrdenColaborador = act_CLB_WALK;
			mapaConPlan[cst.colaborador.f][cst.colaborador.c] = 2;
			break;
		case act_CLB_TURN_SR:
			cst.colaborador.brujula = (Orientacion)((cst.colaborador.brujula + 1) % 8);
			cst.ultimaOrdenColaborador = act_CLB_TURN_SR;
			break;
		case act_CLB_STOP:
			cst.ultimaOrdenColaborador = act_CLB_STOP;
			break;
		}
		it++;
	}
}


void PintaPlan(const list<Action> &plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actWALK){
			cout << "W ";
		}
		else if (*it == actRUN){
			cout << "R ";
		}
		else if (*it == actTURN_SR){
			cout << "r ";
		}
		else if (*it == actTURN_L){
			cout << "L ";
		}
		else if (*it == act_CLB_WALK){
			cout << "cW ";
		}
		else if (*it == act_CLB_TURN_SR){
			cout << "cr ";
		}
		else if (*it == act_CLB_STOP){
			cout << "cS ";
		}
		else if (*it == actIDLE){
			cout << "I ";
		}
		else{
			cout << "-_ ";
		}
		it++;
	}
	cout << " (" << plan.size() << " acciones)\n";
}

//////////////////////////////////////////////////////////////////////////////////////////////////


// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	// Incluir aquí el comportamiento del agente jugador

	Action accion = actIDLE;
	if(sensores.nivel!=4){
		if(!hayPlan){

			cout << endl << "Calculamos nuevo plan:" << endl;

			c_state.jugador.f=sensores.posF;
			c_state.jugador.c=sensores.posC;
			c_state.jugador.brujula=sensores.sentido;

			c_state.colaborador.f=sensores.CLBposF;
			c_state.colaborador.c=sensores.CLBposC;
			c_state.colaborador.brujula=sensores.CLBsentido;

			goal.f=sensores.destinoF;
			goal.c=sensores.destinoC;

			switch (sensores.nivel)
			{
			case 0:
				plan = AnchuraSoloJugador_V3(c_state, goal, mapaResultado);
				break;

			case 1:
				plan = AnchuraJugadorColaborador(stateN1(c_state), goal, mapaResultado);
				break;

			case 2:
				plan = DijkstraSoloJugador(stateN2(c_state), goal, mapaResultado);
				break;
			case 3: 
				plan = AEstrellaJugadorColaborador(stateN3(c_state), goal, mapaResultado);
				break;
			}

			if(!plan.empty()){
				VisualizaPlan(c_state,plan);
				PintaPlan(plan);
				hayPlan = true;
			}
		}

		if(hayPlan && !plan.empty()){
			accion=plan.front();
			plan.pop_front();
		}

		if(plan.empty()) {
			hayPlan=false;
			cout << endl << "Se completó el plan." << endl << endl;
		}
	}
	
	else{

		//Nivel 4

	}

	return accion;
}


int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}