#include "Player.hh"

#include <cmath>

using namespace std;



/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME FreshPrincessTT



struct PLAYER_NAME : public Player {


    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }


    /** 
     * Attributes for your player can be defined here.
     */     
    vector<Dir> dirs;


    /**
     * Play method.
     * 
     * This method will be invoked once per each round.
     * You have to read the board here to place your actions
     * for this round.
     *
     * In this example, each robot selects a random direction
     * and follows it as long as it does not hit a wall.
     */    
int temps_power; 
vector<vector<int> > dist;
queue<Pos> q;
vector<vector<Pos> > padre;
struct priority {
	Pos posicio_escollida;
	int puntuacio;
};
bool superpac;
Pos Fant;
bool altre;
bool porta;
//bool 


virtual void play () {
	// move pacman
	if (round() == 0) {temps_power = 0;}
	superpac = false;
	Pos p = pacman(me()).pos;
	Dir x = BFS_pacman(p);
	move_my_pacman(x);
	if (round() > 19){
		for (int i=0; i < nb_ghosts(); ++i) {
			if(ghost(me(),i).alive){
				porta = false;
				superpac = false;
				altre = false;
				Pos q = ghost(me(), i).pos;
				Dir y = BFS_ghost(q);
				move_my_ghost(i, y);		
			}
		}
	}
	--temps_power;
}   
Dir calcul_dir(priority Chosen, Pos A) {
	if(dest(A,Top) == Chosen.posicio_escollida)    return Top;
    else if(dest(A,Bottom) == Chosen.posicio_escollida) return Bottom;
    else if(dest(A,Right) == Chosen.posicio_escollida)  return Right;
    else if(dest(A,Left) == Chosen.posicio_escollida)   return Left;
	return None;
}
Dir direccio_escollida(priority& Chosen, Pos A) {
	bool gtfo = false;
	priority Aux;
	if (Chosen.puntuacio != -1) {
		while (dist[Chosen.posicio_escollida.i][Chosen.posicio_escollida.j] != 1 and not gtfo){
			Aux.posicio_escollida = padre[Chosen.posicio_escollida.i][Chosen.posicio_escollida.j];
			if (Aux.posicio_escollida.i == -2){
				gtfo = true;
			}
			else Chosen.posicio_escollida = Aux.posicio_escollida;
		}
		return calcul_dir(Chosen, A);
	}
	return None;
}

inline bool pac_can_move (Pos posicio, Dir d) {
	posicio = dest(posicio, d);
	if (not pos_ok(posicio.i, posicio.j) or dist[posicio.i][posicio.j] != -1) return false;
	if (pacman(me()).type == PacMan and cell(posicio.i,posicio.j).id != -1) return false;
	CType t = cell(posicio.i,posicio.j).type;
	if (t == Wall or t == Gate) return false; 
	int r = cell(posicio.i,posicio.j).id;
	if (robot_ok(r) and pacman(me()).type == PowerPacMan){
		 if(robot(r).player == me()) return false; //X
		 if(robot(r).type == PacMan or robot(r).type == PowerPacMan) return false;//X
	} 
	return true;
}
priority actualitzar_parametres(int px, int py, int d, Pos pos, Dir dir, priority Chosen) {
	pos = dest(pos, dir);
	int x = pos.i; int y = pos.j;
	dist[x][y] = d+1;
	q.push(pos);
	padre[x][y].i = px;
	padre[x][y].j = py;
	return criteris_pacman (x, y, d, Chosen);
}
priority criteris_pacman (int x, int y, int d, priority P){//potser posar hammer.
	int punt = 0;
	bool found = false;
	bool found2 = false;
	//Actituds predeterminades per cada mapa: falta parchear terror, crises(potser),
	if (cage() == Pos(1,1)){
			if (pacman(me()).type == PowerPacMan) { 
			int r = cell(x,y).id;
			if (robot_ok(r) and robot(r).type == Ghost and pacman(me()).time > 5){
				punt = pn_ghost();
				if (pos_ok(x+1,y) and pos_ok(x-1,y) and pos_ok(x,y+1) and pos_ok(x,y-1)){
					if (cell(x+1,y).type != Gate and cell(x-1,y).type != Gate and cell(x,y+1).type != Gate and cell(x,y-1).type != Gate){
						if (P.puntuacio < punt) found = true;
					}
				}
			}
			else if (cell(x,y).type == Hammer and pacman(me()).time > 10){
				punt = pn_ghost()*9; //aprox considerarem que SEMPRE HI SÓN.
				if(P.puntuacio < punt) found = true;
			}
			else if (cell(x,y).type == Pill){
				punt = pn_ghost();
				if(P.puntuacio < punt) {found = true; found2 = true;} //hauriem de tenir algun contador del temps que tenim abans que s'acabi la pildora?
			}
			else if (cell(x,y).type == Dot){
				punt = pn_dot();
				if(P.puntuacio < punt) found = true;
			}
			else if (cell(x,y).type == Bonus){
				punt = pn_dot()*2;
				if(P.puntuacio < punt)found = true;
			}
		}
		else{
			if (cell(x,y).type == Pill){
				punt = pn_pill()*2;
				found2 = true;
				if(P.puntuacio < punt){found = true; found2 = true;} //hauriem de tenir algun contador del temps que tenim abans que s'acabi la pildora?
			}
			else if (cell(x,y).type == Bonus){
				punt = pn_dot()*2;
				if(P.puntuacio < punt) found = true;

			}
			else if (cell(x,y).type == Dot){
				punt = pn_dot();
				if(P.puntuacio < punt) found = true;
			}
		}
	}
	else if (cage() == Pos(23,26) or Pos(26,27) == cage()) {// MAPA UPC,horta
		if (pacman(me()).type == PowerPacMan) { 
			int r = cell(x,y).id;
			if (robot_ok(r) and robot(r).type == Ghost and pacman(me()).time > 5 ){
				punt = pn_ghost();
				if (pos_ok(x+1,y) and pos_ok(x-1,y) and pos_ok(x,y+1) and pos_ok(x,y-1)){
					if (cell(x+1,y).type != Gate and cell(x-1,y).type != Gate and cell(x,y+1).type != Gate and cell(x,y-1).type != Gate){
						if (P.puntuacio < punt) found = true;
					}
				}
			}
			else if (cell(x,y).type == Hammer and pacman(me()).time > 5){
				punt = pn_ghost(); //aprox considerarem que SEMPRE HI SÓN.
				if(P.puntuacio < punt) found = true;
			}
			else if (cell(x,y).type == Pill){
				punt = pn_ghost()*3;
				if(P.puntuacio < punt) {found = true; found2 = true;} //hauriem de tenir algun contador del temps que tenim abans que s'acabi la pildora?
			}
			else if (cell(x,y).type == Dot){
				punt = pn_dot();
				if(P.puntuacio < punt) found = true;
			}
			else if (cell(x,y).type == Bonus){
				punt = pn_dot()*2;
				if(P.puntuacio < punt)found = true;
			}
		}
		else{
			if (cell(x,y).type == Pill){
				punt = pn_pill()*2;
				found2 = true;
				if(P.puntuacio < punt){found = true; found2 = true;} //hauriem de tenir algun contador del temps que tenim abans que s'acabi la pildora?
			}
			else if (cell(x,y).type == Bonus){
				punt = pn_dot();
				if(P.puntuacio < punt) found = true;

			}
			else if (cell(x,y).type == Hammer){
				punt = pn_dot()*2;
				if(P.puntuacio < punt) found = true;

			}
			else if (cell(x,y).type == Dot){
				punt = pn_dot();
				if(P.puntuacio < punt) found = true;
			}
		}
	}
	else{
		if (pacman(me()).type == PowerPacMan) { //MAPA Default, Manicomi,Crises,
			int r = cell(x,y).id;
			if (robot_ok(r) and robot(r).type == Ghost and pacman(me()).time > 5 ){
				punt = pn_ghost();
				if (pos_ok(x+1,y) and pos_ok(x-1,y) and pos_ok(x,y+1) and pos_ok(x,y-1)){
					if (cell(x+1,y).type != Gate and cell(x-1,y).type != Gate and cell(x,y+1).type != Gate and cell(x,y-1).type != Gate){
						if (P.puntuacio < punt) found = true;
					}
				}
			}
			else if (cell(x,y).type == Hammer and pacman(me()).time > 0){
				punt = pn_ghost()*9; //aprox considerarem que SEMPRE HI SÓN.
				if(P.puntuacio < punt) found = true;
			}
			else if (cell(x,y).type == Pill){
				punt = pn_ghost();
				if(P.puntuacio < punt) {found = true; found2 = true;} //hauriem de tenir algun contador del temps que tenim abans que s'acabi la pildora?
			}
			/*else if (cell(x,y).type == Mushroom and temps_power > 45 and round()> 80){
				punt = (pn_ghost());
				if(P.puntuacio < punt)found = true;
			}*/
			else if (cell(x,y).type == Dot){
				punt = pn_dot();
				if(P.puntuacio < punt) found = true;
			}
			else if (cell(x,y).type == Bonus){
				punt = pn_bonus()*0.5;
				if(P.puntuacio < punt)found = true;
			}
		}
			
		else{// MAPA Default,
			if (cell(x,y).type == Pill){
				punt = pn_pill()*2;
				found2 = true;
				if(P.puntuacio < punt){found = true; found2 = true;} //hauriem de tenir algun contador del temps que tenim abans que s'acabi la pildora?
			}
			else if (cell(x,y).type == Bonus){
				punt = pn_bonus()*0.5;
				if(P.puntuacio < punt) found = true;

			}
			else if (cell(x,y).type == Hammer){
				punt = pn_dot()*2;
				if(P.puntuacio < punt) found = true;

			}
			else if (cell(x,y).type == Dot){
				punt = pn_dot();
				if(P.puntuacio < punt) found = true;
			}
		}
	}
	if (found) {
		P.puntuacio = punt; 
		P.posicio_escollida.i = x;
		P.posicio_escollida.j = y;
		if (found2) temps_power = power_time();
	}		
	return P;
}
Dir BFS_pacman (Pos A){
	priority Chosen; Chosen.puntuacio = -1;
	dist = vector<vector<int> >(rows(), vector<int> (cols(), -1));
	padre = vector<vector<Pos> > (rows(), vector<Pos> (cols()));
	q.push(A);  // Ponemos el nodo inicial en la cola
	(padre[A.i][A.j]).i = -2; (padre[A.i][A.j]).j = -2; dist[A.i][A.j] = 0; 
	while(not q.empty()){
		Pos p = q.front(); q.pop();
		int x = p.i; int y = p.j; int d = dist[x][y];
		
	// Visitar a los vecinos y ponerlos en la cola, si hace falta.
		if (pac_can_move(p,Top)) Chosen = actualitzar_parametres(x,y,d,p,Top, Chosen);
		if (pac_can_move(p,Bottom)) Chosen = actualitzar_parametres(x,y,d,p,Bottom, Chosen);
		if (pac_can_move(p,Right)) Chosen = actualitzar_parametres(x,y,d,p,Right, Chosen);
		if (pac_can_move(p,Left)) Chosen = actualitzar_parametres(x,y,d,p,Left, Chosen);
	}
	return direccio_escollida(Chosen, A);	
}
inline bool ghost_can_move (Pos posicio, Dir d) {
	posicio = dest(posicio, d);
	if (not pos_ok(posicio.i, posicio.j) or dist[posicio.i][posicio.j] != -1) return false;
	CType t = cell(posicio.i,posicio.j).type;
	if (t == Wall) return false;
	//if (r == me()) return false; //afegit ara, sino treure.
	int r = cell(posicio.i,posicio.j).id;
	if(r == -1) return true;
    if(robot(r).type == Ghost) return false;
    return true;
}
priority criteris_ghost (int x, int y, int d, priority P, Pos A){
	int punt = 0;
	bool found = false;
	bool found2 = false;
	int r = cell(x,y).id;
	if (robot_ok(r) and robot(r).type == PowerPacMan and robot(r).player != me()){
		superpac = true;
	}
	if (robot_ok(r) and robot(r).type == PacMan and robot(r).player != me()){
		punt = pn_pacman();
		if (P.puntuacio < punt) found = true;
	}
	CType t = cell(x,y).type;
	if (t == Gate and not altre){
		Fant.i = x;
		Fant.j = y;
		altre = true;
	}	
	if (found) {
		P.puntuacio = punt; 
		P.posicio_escollida.i = x;
		P.posicio_escollida.j = y;
	}	
	return P;
	//ENLLOC DE .ID FICAR .PLAYER
}
 bool pacmans_aprop(Pos A){
	int r = cell(A.i+1,A.j).id;
	if (robot_ok(r) and robot(r).type == PacMan and robot(r).player != me()) return true;
	r = cell(A.i-1,A.j).id;
	if (robot_ok(r) and robot(r).type == PacMan and robot(r).player != me()) return true;
	r = cell(A.i,A.j+1).id;
	if (robot_ok(r) and robot(r).type == PacMan and robot(r).player != me()) return true;
	r = cell(A.i,A.j-1).id;
	if (robot_ok(r) and robot(r).type == PacMan and robot(r).player != me()) return true;
	return false;
}
priority actualitzar_parametres2(int px, int py, int d, Pos pos, Dir dir, priority Chosen, Pos A){
	pos = dest(pos, dir);
	int x = pos.i; int y = pos.j;
	dist[x][y] = d+1;
	q.push(pos);
	padre[x][y].i = px;
	padre[x][y].j = py;
	return criteris_ghost (x, y, d, Chosen, A);
}
Dir BFS_ghost (Pos A){
	priority Chosen; Chosen.puntuacio = -1;
	dist = vector<vector<int> >(rows(), vector<int> (cols(), -1));
	padre = vector<vector<Pos> > (rows(), vector<Pos> (cols()));
	q.push(A);  // Ponemos el nodo inicial en la cola
	(padre[A.i][A.j]).i = -2; (padre[A.i][A.j]).j = -2; dist[A.i][A.j] = 0; 
	while(not q.empty()){
		Pos p = q.front(); q.pop();
		int x = p.i; int y = p.j; int d = dist[x][y];
		if (ghost_can_move(p,Top)) Chosen = actualitzar_parametres2(x,y,d,p,Top, Chosen, A);
		if (ghost_can_move(p,Bottom)) Chosen = actualitzar_parametres2(x,y,d,p,Bottom, Chosen, A);
		if (ghost_can_move(p,Right)) Chosen = actualitzar_parametres2(x,y,d,p,Right, Chosen, A);
		if (ghost_can_move(p,Left)) Chosen = actualitzar_parametres2(x,y,d,p,Left, Chosen, A);
	}
	if (porta) {
		if (superpac and cell(A.i, A.j).type != Gate and not pacmans_aprop(A)){
			 Chosen.posicio_escollida.i = Fant.i;
			 Chosen.posicio_escollida.j = Fant.j;
		}
	}
	else if (superpac and cell(A.i, A.j).type == Gate and not pacmans_aprop(A)) return None;
	return direccio_escollida(Chosen, A);
}
};




/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
