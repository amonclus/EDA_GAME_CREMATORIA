#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME alvaro

struct PLAYER_NAME : public Player
{

     /**
      * Factory: returns a new instance of this class.
      * Do not modify this function.
      */
     static Player *factory()
     {
          return new PLAYER_NAME;
     }

     /**
      * Types and attributes for your player can be defined here.
      */

     map<int, bool> just_went_up;
     map<int, bool> just_went_down;
     map<int, stack<int>> last_direction; // keeps track of the last positions of all the pioneers

     // checks to see if there are any gems around and returns the direction it is
     int gems_around(Pos pos)
     {
          bool found;
          for (int p = 0; p < 8; ++p)
          {
               Pos new_p = pos;
               new_p += Dir(p);
               if (pos_ok(new_p))
               {
                    if (cell(new_p).gem == true)
                         return p;
               }
          }
          return -1;
     }

     int gems_around_2(Pos pos)
     {
          for (int j = pos.j; j <= pos.j + 2; ++j)
          {
               Pos n_pos(pos.i - 2, j, 1);
               if (pos_ok(n_pos))
               {
                    if (cell(n_pos).gem == true)
                    {
                         return RT;
                         break;
                    }
               }
          }
          for (int i = pos.i - 1; i <= pos.i + 2; ++i)
          {
               Pos n_pos(i, pos.j + 2, 1);
               if (pos_ok(n_pos))
                    if (cell(n_pos).gem == true)
                    {
                         return Right;
                         break;
                    }
          }
          for (int j = pos.j + 2; j >= pos.j; --j)
          {
               Pos n_pos(pos.i + 2, j, 1);
               if (pos_ok(n_pos))
                    if (cell(n_pos).gem == true)
                    {
                         return BR;
                         break;
                    }
          }
          return -1;
     }

     int gems_around_3(Pos pos)
     {
          for (int i = pos.i - 2; i <= pos.i + 2; ++i)
          {
               Pos n_pos(i, pos.j + 3, 1);
               if (pos_ok(n_pos))
                    if (cell(n_pos).gem == true)
                    {
                         return Right;
                         break;
                    }
          }
          return -1;
     }

     void find_gems(int id, Pos pos, bool found)
     {
          int fg = gems_around(pos);
          int fg2 = gems_around_2(pos);
          int fg3 = gems_around_3(pos);
          if (fg != -1)
          {
               command(id, Dir(fg));
               found = true;
          } // esto significa que hay una gema justo a nuestro lado
          else if (fg2 != -1)
          {
               command(id, Dir(fg2));
               found = true;
          } // nos toca buscar más allá
          else if (fg3 != -1)
          {
               command(id, Dir(fg3));
               found = true;
          } // nos toca buscar aun mas alla
     }

     int elevator_around(Pos pos, int id)
     {
          for (int p = 0; p < 8; ++p)
          {
               Pos new_p = pos;
               new_p += Dir(p);
               if (pos_ok(new_p))
                    if (cell(new_p).type == Elevator)
                         return p;
          }
          return -1;
     }

     int elevator_around_r_2(Pos pos)
     {
          for (int j = pos.j; j <= pos.j + 2; ++j)
          {
               Pos n_pos(pos.i - 2, j, 1);
               if (pos_ok(n_pos))
               {
                    if (cell(n_pos).type == Elevator)
                    {
                         return RT;
                         break;
                    }
               }
          }
          for (int i = pos.i - 1; i <= pos.i + 2; ++i)
          {
               Pos n_pos(i, pos.j + 2, 1);
               if (pos_ok(n_pos))
                    if (cell(n_pos).type == Elevator)
                    {
                         return Right;
                         break;
                    }
          }
          for (int j = pos.j + 2; j >= pos.j; --j)
          {
               Pos n_pos(pos.i + 2, j, 1);
               if (pos_ok(n_pos))
                    if (cell(n_pos).type == Elevator)
                    {
                         return BR;
                         break;
                    }
          }
          return -1;
     }

     int elevator_around_r_3(Pos pos)
     {
          for (int i = pos.i - 2; i <= pos.i + 2; ++i)
          {
               Pos n_pos(i, pos.j + 3, 1);
               if (pos_ok(n_pos))
                    if (cell(n_pos).type == Elevator)
                    {
                         return Right;
                         break;
                    }
          }
          return -1;
     }

     int elevator_around_r(Pos pos)
     {
          for (int p = 0; p < 5; ++p)
          {
               Pos new_p = pos;
               new_p += Dir(p);
               if (pos_ok(new_p))
                    if (cell(new_p).type == Elevator)
                         return p;
          }
          return -1;
     }

     void find_elevator(Pos pos, int id)
     {
          int fg = elevator_around_r(pos);
          int fg2 = elevator_around_r_2(pos);
          int fg3 = elevator_around_r_3(pos);
          if (fg != -1)
               command(id, Dir(fg)); // esto significa que hay una gema justo a nuestro lado
          else if (fg2 != -1)
               command(id, Dir(fg2)); // nos toca buscar más allá
          else if (fg3 != -1)
               command(id, Dir(fg3));
     }

     // we move to the right or up or down, this way, we do a sweep of the area and make sure we are not in the same spot for too long
     void backtrack(int id, bool &able)
     {
          stack<int> &Q = last_direction[id];
          if (not Q.empty())
          {
               int d = Q.top();
               Q.pop();
               d = (d + 4) % 8;
               command(id, Dir(d));
               able = true;
               return;
          }
     }

     // add looking for elevator function
     void move_caves_random(int id, Pos pos, bool &moved, int front_sun)
     {
          bool &d = just_went_down[id];
          int de = elevator_around(pos, id);
          Pos pos_up = pos;
          pos_up.k = 1;
          if (cell(pos).type == Elevator and d == false)
          { // add daylight and front of sun condition
               command(id, Dir(Up));
               just_went_up[id] = true;
               return;
          }
          else if (de != -1 and abs(pos.j - front_sun) >= 20 and not daylight(pos_up))
          { // we check if the sun is too close to our position
               command(id, Dir(de));
               return;
          }
          else
          {
               int x = random(0, 7);
               stack<int> &Q = last_direction[id];
               for (int p = x; p < x + 8; ++p)
               {
                    int p_n = p % 8;
                    Pos new_p = pos;
                    new_p += Dir(p_n);
                    if (pos_ok(new_p))
                    {
                         Cell c = cell(new_p);
                         if (c.owner != me() and c.type != Rock and c.id == -1)
                         {
                              command(id, Dir(p_n));
                              moved = true;
                              Q.push(p_n); // añadimos la dirección que tomamos
                              return;
                         }
                    }
               }
          }
     }
     // might be an issue with this function
     // moves the unit id according to our conditions
     void move_caves(int id, int n, int front_sun)
     {
          Pos pos = unit(id).pos; // this position will always be valid since it is the position our unit is on at the moment
          Pos pos_up = pos;
          pos_up.k = 1;
          bool &up = just_went_down[id];
          // conditions to go up are met, so we look for elevator
          int de = -1;
          if (not daylight(pos_up))
               de = elevator_around(pos, id); // set conditions here and initialize de to -1
          if (de != -1)
          {
               command(id, Dir(de));
               return;
          }
          else if (cell(pos).type == Elevator)
          {
               command(id, Dir(Up));
               just_went_up[id] = true;
               return;
          }

          else
          {
               // here, the conditions are met, but we are not near an elevator
               bool moved = false;
               move_caves_random(id, pos, moved, front_sun);
               bool able = false;
               if (not moved)
                    backtrack(id, able);
               if (not able)
                    command(id, Dir(random(1, 3)));
               return;
          }

          // either the conditions are not met, or we are not near an elevator
     }

     void move_pioneers()
     {
          vector<int> my_pion = pioneers(me());
          int n = my_pion.size();
          int front_sun = (round() * 2 - 1) % 80;
          for (int id : my_pion)
          {
               Pos pos = unit(id).pos; // returns the position of the pioneer
               // im on the surface
               if (pos.k == 1)
               {
                    bool found = false; // determines wether we found gems or not
                    find_gems(id, pos, found);
                    if (not found)
                         find_elevator(pos, id);

                    if (cell(pos).type == Elevator and not just_went_up[id])
                    {
                         command(id, Dir(Down));
                         just_went_down[id] = true;
                    }
                    else if (not found)
                         command(id, Dir(random(1, 3)));
                    just_went_up[id] = false;
               }

               // im in the caves
               else
               {
                    // if sun is not too close, look for an elevator
                    bool moved = false;
                    bool able = false;
                    move_caves_random(id, pos, moved, front_sun);
                    if (not moved)
                         backtrack(id, able);
                    just_went_down[id] = false;
               }
          }
     }
     // checks if there is an enemy adjacent to us
     void check_enemy(Pos p, int id, bool &enemy)
     {
          Pos new_p;
          for (int k = 0; not enemy and k < 8; ++k)
          {
               new_p = p;
               new_p += Dir(k);
               int id2 = cell(new_p).id;
               if (id2 != -1 and id2 != me())
               {
                    command(id, Dir(k));
                    enemy = true;
               }
          }
     }
     //finds the pioneers around me
     void find_enemy_pioneers(Pos pos, int id, bool enemy)
     {
          Pos new_p;
          for(int k = pos.j -2; k <= pos.j +2; ++k){
               new_p.k = 0;
               new_p.j = k;
               new_p.i = pos.i - 2;
               int id2 = cell(new_p).id;
               if(id2 != -1 and id2 != me()){
                    if(new_p.j == pos.j -2){command(id, Dir(TL));}
                    else{
                         command(id, Dir(Top));
                    } 
                    enemy = true;
                    break;   
               }
               new_p.i = pos.i + 2;
               if(id2 != -1 and id2 != me()){
                    if(new_p.j == pos.j -2){command(id, Dir(LB));}
                    else command(id, Dir(Bottom));
                    enemy = true;
                    break;   
               }     
          }
          for(int k = pos.i -1; k <= pos.i+1; ++k){
              new_p.k = 0;
              new_p.j = pos.j + 2;
              new_p.i = k;
              Cell c = cell(new_p);
              if(c.id != -1 and c.id != me()){
                    command(id, Dir(Right));
                    enemy = true;
                    break;
               } 
               new_p.j = pos.j - 2;
               if(c.id != -1 and c.id != me()){
                    command(id, Dir(Left));
                    enemy = true;
                    break;
               } 
          }
     }
     // improve so it does not move in a simply random way, it must move away from rocks and its own units
     void move_furyans()
     {
          vector<int> my_fur = furyans(me());
          int n = my_fur.size();
          for (int id : my_fur)
          {
               Pos pos = unit(id).pos;
               bool enemy = false;
               check_enemy(pos, id, enemy);
               if (not enemy)
               {
                    find_enemy_pioneers(pos, id, enemy);

                    if(not enemy){
                         int x = random(0, 7);
                         for (int p = x; p < x + 8; ++p)
                         {
                              int p_n = p % 8;
                              Pos new_p = pos;
                              new_p += Dir(p_n);
                              if (pos_ok(new_p))
                              {
                                   Cell c = cell(new_p);
                                   if (c.type != Rock and c.id != me())
                                   {
                                        command(id, Dir(p_n));
                                        break;
                                   }
                              }
                         }
                    }
               }
               
          }
     }
     /**
      * Play method, invoked once per each round.
      */
     virtual void play()
     {
          move_furyans();
          move_pioneers();

     }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);