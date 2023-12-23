#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Alba_v5


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
     static Player* factory () {
     return new PLAYER_NAME;
     }

     /**
      * Types and attributes for your player can be defined here.
      */
     
     map<int, bool> just_went_up;
     map<int, bool> just_went_down;
     map<int, stack<int>> last_direction;         //keeps track of the last positions of all the pioneers
     map<int, int> round_used;

     //checks to see if there are any gems around and returns the direction it is
     int gems_around(Pos pos){
          bool found;
          for(int p = 0; p < 8; ++p){
               Pos new_p = pos;
               new_p += Dir(p);
               if(cell(new_p).gem == true) return p;
          }
          return -1;
     }

     int gems_around_2(Pos pos){
          for(int j = pos.j; j <= pos.j+2; ++j){
               Pos n_pos(pos.i-2, j, 1);
               if(cell(n_pos).gem == true){return RT; break; }
          }
          for(int i = pos.i-1; i <= pos.i+2; ++i){
               Pos n_pos(i, pos.j+2, 1);
               if(cell(n_pos).gem == true){ return Right; break;}
          }
          for(int j = pos.j+2; j >= pos.j; --j){
               Pos n_pos(pos.i+2, j, 1);
               if(cell(n_pos).gem == true){return BR; break;} 
          }
          return -1;
     }

     int gems_around_3(Pos pos){
          for(int i = pos.i-2; i <= pos.i+2; ++i){
               Pos n_pos(i, pos.j+3, 1);
               if(cell(n_pos).gem == true){ return Right; break;}
          }
          return -1;
     }

     void find_gems(int id, Pos pos){
          int fg = gems_around(pos);
          int fg2 = gems_around_2(pos);
          int fg3 = gems_around_3(pos);
          if(fg != -1)command(id, Dir(fg));            //esto significa que hay una gema justo a nuestro lado
          else if(fg2 != -1) command(id, Dir(fg2));              //nos toca buscar más allá
          else if(fg3 != -1) command(id, Dir(fg3));              //nos toca buscar aun mas alla
     }
     
     int elevator_around(Pos pos, int id){
          for(int p = 0; p < 8; ++p){
               Pos new_p = pos;
               new_p += Dir(p);
               if(cell(new_p).type == Elevator) return p;   //comprobamos que no sea el ascensor que acabamos de usar
          }
          return -1;
     }

     int elevator_around_r(Pos pos){
          for(int p = 0; p < 5; ++p){
               Pos new_p = pos;
               new_p += Dir(p);
               if(cell(new_p).type == Elevator) return p;
          }
          return -1;
     }

     int units_upstairs(){
          int ns = 0;
          vector<int> pion = pioneers(me());
          for(int i : pion){
               if(unit(i).pos.k == 1) ++ns;
          }
          return ns;
     }

     //we move to the right or up or down, this way, we do a sweep of the area and make sure we are not in the same spot for too long
    

     void backtrack(int id){
          auto it = last_direction.find(id);
          if(not (*it).second.empty()){
               int d = (*it).second.top(); (*it).second.pop();
               d = (d + 4) % 8;
               command(id, Dir(d));
          }
     }

     
     
     void move_caves_right(int id, Pos pos, bool &moved){
          int x = random(0,4);
          auto it = last_direction.find(id);
          if(it == last_direction.end()){
               //si este elemento no está inicializado, lo inicializamos
               stack<int> Q;
               last_direction.insert(make_pair(id, Q));
               it = last_direction.find(id);
          }
          for(int p = x; p < x + 5;++p){
               int p_n = p%5;
               Pos new_p = pos;
               new_p += Dir(p_n);
               int id2 = cell(new_p).id;
               //checks the cell we will move to is not a wall and there is no troop on it
               if(cell(new_p).owner != me() and cell(new_p).type != Rock and id2 == -1) {
                    command(id, Dir(p_n));
                    (*it).second.push(p_n);
                    moved = true;
                    break;
               }
          }
     }

     //moves pioneers to the left
     void move_caves_left(int id, Pos pos, bool &moved){
          auto it = last_direction.find(id);
          if(it == last_direction.end()){
               //si este elemento no está inicializado, lo inicializamos
               stack<int> Q;
               last_direction.insert(make_pair(id, Q));
               it = last_direction.find(id);
          }
          for(int p = 5; p < 8;++p){
               Pos new_p = pos;
               new_p += Dir(p);
               int id2 = cell(new_p).id;
               if(cell(new_p).owner != me() and cell(new_p).type != Rock and id2 == -1){
                    command(id, Dir(p));
                    (*it).second.push(p);
                    moved = true;
                    break;
               }
          }
     }

     //moves the unit id according to our conditions
     void move_caves(int id, int n, int front_sun){
          Pos pos = unit(id).pos;
          Pos pos_up = pos;
          pos_up.k = 1;
          //conditions to go up are met, so we look for elevator
          if(not daylight(pos_up) and n > 11 and units_upstairs() <= 4 and abs(pos.j - front_sun ) >= 14 and not just_went_down[id]){
               int de = elevator_around(pos, id);
               if(de != -1){
                    command(id, Dir(de));
               }
               else if(cell(pos).type == Elevator){
                    command(id, Dir(Up));
                    just_went_up[id] = true;
               }
               else{
                    //here, the conditions are met, but we are not near an elevator
                    if(round() < 60){
                    bool moved = false;
                    move_caves_right(id, pos, moved);
                    if(not moved)move_caves_left(id, pos, moved);
                    if(not moved) backtrack(id);
               }
               else{
                    bool moved = false;
                    move_caves_left(id, pos, moved);
                    if(not moved)move_caves_right(id, pos, moved);
                    if(not moved) backtrack(id);
               }
               }
          }
          //either the conditions are not met, or we are not near an elevator
          else{
               if(round() < 60){
                    bool moved = false;
                    move_caves_right(id, pos, moved);
                    if(not moved)move_caves_left(id, pos, moved);
                    if(not moved) backtrack(id);
               }
               else{
                    bool moved = false;
                    move_caves_left(id, pos, moved);
                    if(not moved)move_caves_right(id, pos, moved);
                    if(not moved) backtrack(id);
               }
          }
     }


     void move_pioneers(){
          vector<int> my_pion = pioneers(me());
          int n = my_pion.size();
          int front_sun = (round()*2 -1)%80;
          for(int id : my_pion){
               Pos pos = unit(id).pos;       //returns the position of the pioneer
               //im on the surface   
               if(pos.k == 1){
                    find_gems(id, pos);

                    int de_r = elevator_around_r(pos);         
                    if(de_r != -1) command(id, Dir(de_r));

                    else if(cell(pos).type == Elevator and not just_went_up[id]){
                         command(id, Dir(Down));
                         just_went_down[id] = true;
                         round_used[id] = round();
                    }
                    else command(id, Dir(random(1,3)));
                    just_went_up[id] = false;
               }

               //im in the caves
               else{
                    //if sun is not too close, look for an elevator
                    move_caves(id, n, front_sun);
               }
               just_went_down[id] = false;
          }
     }
     
     void check_enemy(Pos p, int id, bool& enemy){
          for (int k = 0; not enemy and k < 8; ++k) {
               if (pos_ok(p)) {
                    int id2 = cell(p).id;         //checks the id of a unit
                    if (id2 != -1 and unit(id2).player != me()) { // if we are next to an enemy, we attack.
                         enemy = true;
                         command(id, Dir(k));
                    }
               }
          }
     }

     void move_furyans(){
          vector<int> my_fur = furyans(me());
               int n = my_fur.size();
               for (int id : my_fur) {
                    bool enemy = false;
                    check_enemy(unit(id).pos, id, enemy);
                    if (not enemy) {
                         command(id, Dir(random(0,8)));
                    }
               }
     } 
     
     /**
      * Play method, invoked once per each round.
      */
     virtual void play () {
          move_furyans();
          move_pioneers();
     }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);