#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME DFSv2


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

     int gems_around_3(Pos pos){
          for(int i = pos.i-2; i <= pos.i+2; ++i){
               Pos n_pos(i, pos.j+3, 1);
               if(cell(n_pos).gem == true){ return Right; break;}
          }
          return -1;
     }

     void find_gems(int id, Pos pos){
          int fg = gems_around(pos);
          int fg3 = gems_around_3(pos);
          if(fg != -1)command(id, Dir(fg));            //esto significa que hay una gema justo a nuestro lado
          else if(fg3 != -1) command(id, Dir(fg3));              //nos toca buscar aun mas alla
     }
     
     int elevator_around(Pos pos){
          for(int p = 0; p < 8; ++p){
               Pos new_p = pos;
               new_p += Dir(p);
               if(cell(new_p).type == Elevator) return p;  
          }
          return -1;
     }


     void move_caves_random(int id, Pos pos, bool &moved) {
          int x = random(0, 7);
          stack<int> &Q = last_direction[id];
          for (int p : {x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6, x + 7}) {
               int p_n = p % 8;
               Pos new_p = pos + Dir(p_n);
               Cell c = cell(new_p);
               if (c.owner != me() and c.type != Rock and c.id == -1) {
                    command(id, Dir(p_n));
                    moved = true;
                    Q.push(p_n);
                    break;
               }
          }
    }


     //moves the unit id according to our conditions
     void move_caves(int id) {
        Pos pos = unit(id).pos;
        Pos pos_up = pos;
        pos_up.k = 1;

        if (not daylight(pos_up) and not just_went_down[id]) {
            int de = elevator_around(pos);
            if (de != -1) {
                command(id, Dir(de));
            } else if (cell(pos).type == Elevator) {
                command(id, Dir(Up));
                just_went_up[id] = true;
            } else {
                bool moved = false;
                move_caves_random(id, pos, moved);
                if (not moved) {
                    stack<int> &Q = last_direction[id];
                    if (not Q.empty()) {
                        int d = (Q.top() + 4) % 8;
                        Q.pop();
                        command(id, Dir(d));
                    }
                }
            }
        } else {
            bool moved = false;
            move_caves_random(id, pos, moved);
            if (not moved) {
                stack<int> &Q = last_direction[id];
                if (not Q.empty()) {
                    int d = (Q.top() + 4) % 8;
                    Q.pop();
                    command(id, Dir(d));
                }
            }
        }
    }

     void move_pioneers() {
        for (int id : pioneers(me())) {
            Pos pos = unit(id).pos;
            if (pos.k == 1) {
                int de_r = elevator_around(pos);
                if (de_r != -1) {
                    command(id, Dir(de_r));
                } else if (cell(pos).type == Elevator and not just_went_up[id]) {
                    command(id, Dir(Down));
                    just_went_down[id] = true;
                } else {
                    command(id, Dir(random(1, 3)));
                    just_went_up[id] = false;
                }
            } else {
                move_caves(id);
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
          cerr << status(me()) << endl;
          move_furyans();
          move_pioneers();
     }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);