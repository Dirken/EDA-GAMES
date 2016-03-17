#include "Player.hh"

using namespace std;


/**
 * Write the name of your player and save this file
 * as AI<name>.cc
 */
#define PLAYER_NAME NewPusheen26

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

    queue<Pos> q;
    vector<vector<int> > dist;
    vector<vector<Pos> > padre;
    bool aTiro;
    bool rondaAnterior;
    struct priority {
        Pos posicio_escollida;
        int puntuacio;
    };
    Pos destinacioN1 = {25,25};

    inline Pos dest (const Pos& p, Dir d) const {
        Pos q = p + d;
        return q;
    }

    Dir calcul_dir(priority Chosen, Pos A) {

             if(dest(A,UP) == Chosen.posicio_escollida)         return UP;
        else if(dest(A,FAST_UP) == Chosen.posicio_escollida)    return FAST_UP;
        else if(dest(A,SLOW_UP) == Chosen.posicio_escollida)    return SLOW_UP;
        else if(dest(A,DOWN) == Chosen.posicio_escollida)       return DOWN;
        else if(dest(A,FAST_DOWN) == Chosen.posicio_escollida)  return FAST_DOWN;
        else if(dest(A,SLOW_DOWN) == Chosen.posicio_escollida)  return SLOW_DOWN;
        else if(dest(A,SLOW) == Chosen.posicio_escollida)       return SLOW;
        else if(dest(A,FAST) == Chosen.posicio_escollida)       return FAST;
                                                                return DEFAULT;
    }

    Dir direccio_escollida(priority& Chosen, Pos A, Starship s) {
        bool gtfo = false;
        priority Aux;
        if (Chosen.puntuacio != -1) {
            while (dist[first(Chosen.posicio_escollida)][second(Chosen.posicio_escollida)] != 1 and not gtfo){
                Aux.posicio_escollida = padre[first(Chosen.posicio_escollida)][second(Chosen.posicio_escollida)];
                if (first(Aux.posicio_escollida) == -2) gtfo = true;
                else Chosen.posicio_escollida = Aux.posicio_escollida;
            }
            return calcul_dir(Chosen, A);
        }
        if (s.nb_miss > 0 && round() > 3) aTiro = true;
        return DEFAULT;
    }

    bool missil_aprop(Pos posicio){
        Cell ahead;
        for (int i = 0; i < 4; ++i){
            ahead = cell(dest(posicio,{0,-i}));
            if (within_window(dest(posicio,{0,-i}), round()+1) && ahead.type == MISSILE){
                return true;
            } 
        }
        return false;
    }

    bool nau_aprop(Pos posicio){
        Cell ahead;
        Starship_Id meu = (cell(posicio)).sid;
        for (int i = 0; i < 3; ++i){
            if (within_window(dest(posicio,{0,-i}), round()+1)){
               ahead = cell(dest(posicio,{0,-i}));
                if(ahead.type == STARSHIP){
                    Player_Id p2 = player_of(ahead.sid);
                    if (p2 != me() && ahead.sid != meu) return true;
                } 
            }
        }
        for (int i = 0; i < 3; ++i){
            if (within_window(dest(posicio,{-i,0}), round()+1)){
                ahead = cell(dest(posicio,{-i, 0}));
                if (ahead.type == STARSHIP){
                    Player_Id p2 = player_of(ahead.sid);
                    if (p2 != me() && ahead.sid != meu) return true;
                }
            } 
        }
        for (int i = 0; i < 3; ++i){
            if (within_window(dest(posicio,{+i,0}), round()+1)){
                ahead = cell(dest(posicio,{+i, 0}));
                if (ahead.type == STARSHIP){
                    Player_Id p2 = player_of(ahead.sid);
                    if (p2 != me() && ahead.sid != meu) return true;
                }
            } 
        }

        return false;
    }
    inline bool check_posicio (Pos posicio, Dir d, Starship s){
        posicio = dest(posicio, d);
        if (s.sid != begin(me()) and posicio == destinacioN1) return false;
        if (not within_window(posicio, round()+1) or dist[first(posicio)][second(posicio)] != -1) return false;
        CType t = cell(posicio).type;
        if (t == ASTEROID or t == STARSHIP or t == MISSILE) return false; 
        if (missil_aprop(posicio) || nau_aprop(posicio)) return false;

        return true;
    }

    inline bool nau_can_move (Pos posicio, Dir d, Starship s) {
        Pos aux = posicio;
        
        if (not check_posicio(posicio,d,s)) return false;
        else {
                 if (d == DOWN)     return (check_posicio(aux, SLOW_DOWN,s) && check_posicio(aux,DEFAULT,s));
            else if (d == UP)       return (check_posicio(aux,SLOW_UP,s) && check_posicio(aux,DEFAULT,s));
            else if (d == FAST)     return (check_posicio(aux,DEFAULT,s));
            else if (d == FAST_DOWN)return (check_posicio(aux,DEFAULT,s) && check_posicio(aux, DOWN,s) 
                                            && check_posicio(aux, SLOW_DOWN,s) && check_posicio(aux, FAST,s));
            else if (d == FAST_UP)  return (check_posicio(aux,DEFAULT,s) && check_posicio(aux, UP,s) 
                                            && check_posicio(aux, SLOW_UP,s) && check_posicio(aux, FAST,s));
        }
        if (missil_aprop(posicio) || nau_aprop(posicio)) return false;
        return true;
    }
    void disparar(Pos posicio){
        Cell ahead;
        for (int i = 0; i < 8; ++i){
            ahead = cell(dest(posicio,{0,i}));
            if (ahead.type == STARSHIP){
                Player_Id p2 = player_of(ahead.sid);
                if (p2 != me()) aTiro = true;
            }
        }
    }

    priority criteris_nau (int x, int y, int d, priority P, Starship& s){
        int punt = -1;
        bool found = false; 
        Pos posicio = {x,y};
        CType t = cell(posicio).type;
        Cell ahead = cell(posicio);
        if (t == POINT_BONUS){
            punt = 2;
            if(P.puntuacio < punt) found = true;
        } 
        else if (t == MISSILE_BONUS){
            punt = 1;
            if (s.nb_miss < 2) punt = 2;
            if(P.puntuacio < punt) found = true;
        } 
        else if (t == EMPTY){
            punt = 0;
            if(P.puntuacio < punt) found = true;
        }          
        else if (t == STARSHIP){
             Player_Id p2 = player_of(ahead.sid);
            if (p2 != me()) {
                punt = 4;
                if(P.puntuacio < punt) found = true;
            }
        }
        // Esquivar-me a mi mateix.
        if (found) {
            P.puntuacio = punt; 
            P.posicio_escollida = {x,y}; 
            if (s.sid == begin(me())) destinacioN1 = {x,y};
        }


        return P;
    }
    priority actualitzar_parametres(int px, int py, int d, Pos pos, Dir dir, priority Chosen, Starship& s) {
        pos = dest(pos, dir);
        int x = first(pos); int y = second(pos);
        dist[x][y] = d+1;

        q.push(pos);
        padre[x][y]= {px, py};
        return criteris_nau (x, y, d, Chosen, s);
    }

    Dir BFS_nau (Pos A, Starship s) {
        priority Chosen; Chosen.puntuacio = -1;
        dist = vector<vector<int> >(number_rows(), vector<int> (number_window_columns()+round()+1, -1));
        padre = vector<vector<Pos> > (number_rows(), vector<Pos> (number_window_columns()+round()+1)); 
        q.push(A);  // Ponemos el nodo inicial en la cola
        padre[first(A)][second(A)]  = {-2,-2};
        dist[first(A)][second(A)] = 0; 
        while (not q.empty()){
            Pos p = q.front(); q.pop();
            int x = first(p); int y = second(p); int d = dist[x][y];
            if (nau_can_move(p,UP, s))         Chosen = actualitzar_parametres(x,y,d,p,UP, Chosen, s);
            if (nau_can_move(p,FAST_UP,s))    Chosen = actualitzar_parametres(x,y,d,p,FAST_UP, Chosen, s);
            if (nau_can_move(p,SLOW_UP,s))    Chosen = actualitzar_parametres(x,y,d,p,SLOW_UP, Chosen, s);
            if (nau_can_move(p,DOWN,s))       Chosen = actualitzar_parametres(x,y,d,p,DOWN, Chosen, s);
            if (nau_can_move(p,FAST_DOWN,s))  Chosen = actualitzar_parametres(x,y,d,p,FAST_DOWN, Chosen, s);
            if (nau_can_move(p,SLOW_DOWN,s))  Chosen = actualitzar_parametres(x,y,d,p,SLOW_DOWN, Chosen, s);
            if (nau_can_move(p,SLOW,s))       Chosen = actualitzar_parametres(x,y,d,p,SLOW, Chosen, s);
            if (nau_can_move(p,FAST,s))       Chosen = actualitzar_parametres(x,y,d,p,FAST, Chosen, s);
            if (nau_can_move(p,DEFAULT,s))    Chosen = actualitzar_parametres(x,y,d,p,DEFAULT, Chosen, s);
        }
        return direccio_escollida(Chosen, A, s);   
    }

    /**
     * Play method.
     *
     * This method will be invoked once per each round.
     * You have to read the board here to place your actions
     * for this round.
     *
     */
    bool posicio_propera(Pos x, Pos y) {
        Starship_Id meu = (cell(x)).sid;
        Starship_Id desti = (cell(y)).sid;

        if ( x == y) return true;
        else if (x -y == DOWN || y -x == DOWN) return true;
        else if (x -y == UP || y -x == UP) return true;
        else if (x -y == FAST || y -x == FAST) return true;
        else if (x -y == FAST_DOWN || y -x == FAST_DOWN) return true;
        else if (x -y == FAST_UP || y -x == FAST_UP) return true;
        else if (x -y == DEFAULT || y -x == DEFAULT) return true;
        else if (x -y == SLOW || y -x == SLOW) return true;
        else if (x -y == SLOW_UP || y -x == SLOW_UP) return true;
        else if (x -y == SLOW_DOWN || y -x == SLOW_DOWN) return true;

        return false;
    }


       
    virtual void play () {
        Pos posnau1;
        for (Starship_Id sid = begin(me()); sid != end(me()); ++sid) {
          aTiro = false;
          Starship s = starship(sid);
            if (s.alive) { // if the starship is alive
                if (round() == 0){
                    Pos p = s.pos; 
                   CType t = cell(dest(p,{0,6})).type;
                   if (t == ASTEROID) shoot(sid);
               }
                else {
                    Pos p = s.pos; 
                    Dir x = BFS_nau(p, s); 
                    if (sid == begin(me())) posnau1 = dest(p,x);
                    CType t = cell(dest(p,{0,1})).type;
                    if (t == ASTEROID) {
                        Dir x2 = BFS_nau(dest(p,{0,1}),s);
                        if (x2 != x && s.nb_miss > 1) aTiro = true;
                    }
                    if (posicio_propera(posnau1,dest(p,x)) && sid != begin(me()) && round() > 17){ //no xocar entre elles...
                        move(sid, SLOW);
                    }
                    if (x == SLOW && missil_aprop(p)){ //evitar tiros per quedar-se quiet
                        if (nau_can_move(p,UP,s))        move(sid,UP);
                        else if (nau_can_move(p,DOWN,s)) move(sid,DOWN);
                    }
                    
                    if (s.nb_miss > 1 && round() > 3)                   disparar(p);
                    if (aTiro && s.nb_miss > 1 && round() > 3)   {
                        shoot(sid);
                        rondaAnterior = true;
                    }       
                    else {
                        move(sid,x); 
                    }
                }

            }
        }
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

