#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Alba_v1


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
  void move_furyans(){

  }

  void move_pioneers(){
     vector<int> my_pion = pioneers(me());
     for(int id : my_pion){
          Pos pos = unit(id).pos;       //returns the position of the pioneer

          
          if(cell(pos).type == Outside){
               //do outside task
          }
          else{
               //check the type of the cells around us
               for(int p = 0; p < 8;++p){
                    Pos new_p = pos;
                    new_p += Dir(p);
                    if(cell(new_p).owner != me() and cell(new_p).type != Rock){
                         command(id, Dir(p));
                        break;
                    }
               }
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
