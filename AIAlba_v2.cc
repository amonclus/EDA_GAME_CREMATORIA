#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Alba_v2


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
     map<int, Pos> last_elevator;
     map<int, int> round_used;

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

     void backtrack(int id){

     }

     void move_furyans(){

     }

     void move_pioneers(){
          Pos zero(0,0,0);

          vector<int> my_pion = pioneers(me());
          int n = my_pion.size();
          int front_sun = (round()*2 -1)%80;
          for(int id : my_pion){
               Pos pos = unit(id).pos;       //returns the position of the pioneer
               int first = true;       
               //im on the surface   

               if(pos.k == 1){
                    int dg = gems_around(pos);
                    int de_r = elevator_around_r(pos);

                    if(dg != -1) command(id, Dir(dg)); 

                    else if(de_r != -1) command(id, Dir(de_r));

                    else if(cell(pos).type == Elevator and not just_went_up[id]){
                         command(id, Dir(Down));
                         last_elevator[id] = pos;      //position of the elevator we just used
                         just_went_down[id] = true;
                         round_used[id] = round();
                    }
                    else command(id, Dir(random(1,3)));
                    just_went_up[id] = false;
               }

               //im in the caves
               else{
                    auto it = last_elevator.find(id);
                    Cell cell_pos = cell(pos);
                    //si no es el ascensor que acabo de usar, subo
                    if(cell_pos.type == Elevator and (it == last_elevator.end() or ((*it).second.j != pos.j and (*it).second.i != pos.i)) and !daylight(pos) and n > 11 and abs(pos.j - front_sun)>20){
                         command(id, Dir(Up));
                         last_elevator[id] = pos;
                         round_used[id] = round();
                    }
                    else{ 
                         //aqui hay que especificar que no sea el ascensor que acabamos de usar, para que no se quede rodeandolo infinitamente
                         auto it = last_elevator.find(id);
                         int de = elevator_around(pos);
                         Pos new_p = pos;
                         new_p += Dir(de);         //position of the new elevator
                         if(de != -1 and new_p.j != (*it).second.j and new_p.i != (*it).second.i){
                              command(id, Dir(de));
                              last_elevator[id] = new_p;
                              round_used[id] = round();
                         }
                         else{
                         //check the type of the cells around us
                              int x = random(0,8);
                              for(int p = x; p < x + 8;++p){
                                   int p_n = p%8;
                                   Pos new_p = pos;
                                   new_p += Dir(p_n);
                                   if(cell(new_p).owner != me() and cell(new_p).type != Rock and cell(new_p).type != Elevator){
                                        command(id, Dir(p_n));
                                   }
                              }
                              //if it reaches here, it means that it is stuck, so we have to get it out by backtracking until we find a non visited square
                              command(id, Dir(random(0,7)));
                         }
                    }
                    just_went_down[id] = false;
                    if(round() - round_used[id] >= 4) last_elevator[id] = zero;
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