#include "Player.hh"

//Probar parámetros frente a alba_v2

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Alba_v3


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

     //checks to see if there are any gems around and returns the direction it is
     int gems_around(Pos pos){
          for(int p = 0; p < 8; ++p){
               Pos new_p = pos;
               new_p += Dir(p);
               if(cell(new_p).gem == true) return p;
          }
          return -1;
     }
     
     int elevator_around(Pos pos){
          for(int p = 0; p < 8; ++p){
               Pos new_p = pos;
               new_p += Dir(p);
               if(cell(new_p).type == Elevator) return p;
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


     //if we cannot move to the right, we will attempt to move to the left
     //first check if we can move to the right, then top right, then bottom right, then up and then bottom
     void move_right(int id){
          
     }


     //make them constantly move to the right of the map as long as it is possible: For this to work, check if there are caves straight to the right, if so, go 
     //either up or down, if there is a wall right upstairs, go right or down and so on...
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

     void move_pioneers(){
          vector<int> my_pion = pioneers(me());
          int n = my_pion.size();
          int front_sun = (round()*2 -1)%80;
          for(int id : my_pion){
               Pos pos = unit(id).pos;       //returns the position of the pioneer
               //im on the surface   
               if(pos.k == 1){
                    int dg = gems_around(pos);
                    int de_r = elevator_around_r(pos);

                    if(dg != -1) command(id, Dir(dg)); 

                    else if(de_r != -1) command(id, Dir(de_r));

                    else if(cell(pos).type == Elevator and not just_went_up[id]){
                         command(id, Dir(Down));
                         just_went_down[id] = true;
                    }
                    else command(id, Dir(random(1,3)));
                    just_went_up[id] = false;
               }
               //im in the caves
               else{
                    Cell cell_pos = cell(pos);
                    if(cell_pos.type == Elevator and not just_went_down[id] and !daylight(pos) and n > 11 and abs(pos.j - front_sun)>20){
                         command(id, Dir(Up));
                         just_went_up[id] = true;
                    }
                    else{
                         int de = elevator_around(pos);
                         if(de != -1){
                              command(id, Dir(de));
                         }
                         else{
                         //check the type of the cells around us
                              int x = random(0,8);
                              for(int p = x; p < x + 8;++p){
                                   int p_n = p%8;
                                   Pos new_p = pos;
                                   new_p += Dir(p_n);
                                   if(cell(new_p).owner != me() and cell(new_p).type != Rock){
                                        command(id, Dir(p_n));
                                   }
                              }
                              //if it reaches here, it means that it is stuck, so we have to get it out
                              command(id, Dir(random(0,7)));
                         }
                    }
                    just_went_down[id] = false;
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
