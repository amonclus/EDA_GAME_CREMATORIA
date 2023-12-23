#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME DFS

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
     map<Pos, vector<Pos>> adj;

     // checks to see if there are any gems around and returns the direction it is

     void find_gems(int id, Pos pos, bool &found)
     {
          for (int i = pos.i - 5; i <= pos.i + 5; ++i)
          {
               for (int j = pos.j; j <= pos.j + 5; ++j)
               {
                    Pos n_pos(i, j, 1);
                    if (cell(n_pos).gem == true)
                    {
                         find_pos(id, pos, n_pos);
                         found = true;
                         break;
                    }
               }
          }
     }

     void find_pos(int id, Pos pos, Pos dest)
     {
          int dir_i = pos.i - dest.i;
          int dir_j = pos.j - dest.j;
          if (dir_i == 0 and dir_j < 0)
               command(id, Dir(Right));
          else if (dir_i > 0 and dir_j < 0)
               command(id, Dir(RT));
          else if (dir_i > 0 and dir_j == 0)
               command(id, Dir(Top));
          else if (dir_i > 0 and dir_j > 0)
               command(id, Dir(TL));
          else if (dir_i == 0 and dir_j > 0)
               command(id, Dir(Left));
          else if (dir_i < 0 and dir_j > 0)
               command(id, Dir(LB));
          else if (dir_i < 0 and dir_j == 0)
               command(id, Dir(Bottom));
          else if (dir_i < 0 and dir_j < 0)
               command(id, Dir(BR));
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


     void find_elevator(Pos pos, int id, bool &found)
     {
          for (int i = pos.i - 5; i <= pos.i + 5; ++i)
          {
               for (int j = pos.j+1; j <= pos.j + 5; ++j)
               {
                    Pos n_pos(i, j, 1);
                    if (cell(n_pos).type == Elevator)
                    {
                         find_pos(id, pos, n_pos);
                         found = true;
                         break;
                    }
               }
          }
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

     bool necromongers_around(Pos pos){
          pos.k = 1;
          vector<int> n = necromongers();
          for(long unsigned int i = 0; i < n.size(); ++i){
               Pos pn = unit(n[i]).pos;
               if(abs(pn.i - pos.i) <= 3 and abs(pn.j - pos.j) <= 3) return true;
          }
          return false;
     }

     // add looking for elevator function
     void move_caves_random(int id, Pos pos, bool &moved, int front_sun, int n)
     {
          bool &d = just_went_down[id];
          int de = elevator_around(pos, id);
          Pos pos_up = pos;
          pos_up.k = 1;
          if (cell(pos).type == Elevator and d == false and not daylight(pos_up))
          { // add daylight and front of sun condition
               command(id, Dir(Up));
               just_went_up[id] = true;
          }
          else if (de != -1 and abs(pos.j - front_sun) >= 20 and not daylight(pos_up) and n > 11 and nb_cells(me()) > 240 and not necromongers_around(pos))
          { // we check if the sun is too close to our position
               command(id, Dir(de));
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
                         if (c.owner != me() and c.type != Rock and c.id == -1 and c.type != Elevator)
                         {
                              command(id, Dir(p_n));
                              moved = true;
                              Q.push(p_n); // añadimos la dirección que tomamos
                              break;
                         }
                    }
               }
          }
     }
     // might be an issue with this function
     // moves the unit id according to our conditions

     void go_left(int id, Pos pos, int front_sun)
     {
          int de = elevator_around(pos, id);
          bool &d = just_went_down[id];
          Pos pos_up = pos;
          pos_up.k = 1;
          if (cell(pos).type == Elevator and d == false and not daylight(pos_up))
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
               stack<int> &Q = last_direction[id];
               for (int p = 5; p < 8; ++p)
               {
                    Pos new_p = pos;
                    new_p += Dir(p);
                    Cell c = cell(new_p);
                    if (c.type != Rock and c.id == -1)
                    {
                         command(id, Dir(p));
                         Q.push(p); // añadimos la dirección que tomamos
                         return;
                    }
               }
               Pos new_p = pos;
               new_p += Dir(Top);
               Cell c = cell(new_p);
               if (c.type != Rock and c.id == -1)
               {
                    command(id, Dir(Top));
                    Q.push(Top); // añadimos la dirección que tomamos
                    return;
               }
               new_p += Dir(Bottom);
               c = cell(new_p);
               if (c.type != Rock and c.id == -1)
               {
                    command(id, Dir(Bottom));
                    Q.push(Top); // añadimos la dirección que tomamos
                    return;
               }
          }
     }

     void go_right(int id, Pos pos, int front_sun)
     {
          int de = elevator_around(pos, id);
          bool &d = just_went_down[id];
          Pos pos_up = pos;
          pos_up.k = 1;
          if (cell(pos).type == Elevator and d == false and not daylight(pos_up))
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
               stack<int> &Q = last_direction[id];
               for (int p = 1; p < 4; ++p)
               {
                    Pos new_p = pos;
                    new_p += Dir(p);
                    Cell c = cell(new_p);
                    if (c.type != Rock and c.id == -1)
                    {
                         command(id, Dir(p));
                         Q.push(p); // añadimos la dirección que tomamos
                         return;
                    }
               }
               Pos new_p = pos;
               new_p += Dir(Top);
               Cell c = cell(new_p);
               if (c.type != Rock and c.id == -1)
               {
                    command(id, Dir(Top));
                    Q.push(Top); // añadimos la dirección que tomamos
                    return;
               }
               new_p += Dir(Bottom);
               c = cell(new_p);
               if (c.type != Rock and c.id == -1)
               {
                    command(id, Dir(Bottom));
                    Q.push(Top); // añadimos la dirección que tomamos
                    return;
               }
          }
     }

     void check_hellhound(vector<int> h, Pos p, int id, bool &found, int front_sun)
     {
          for (int idh : h)
          {
               Pos ph = unit(idh).pos;
               if (abs(p.i - ph.i) <= 5 and abs(p.j - ph.j) <= 5)
               { // this means there is a hound near us
                    found = true;
                    if (ph.j >= p.j)
                    {
                         go_left(id, p, front_sun);
                    }
                    else if (ph.j < p.j)
                    {
                         go_right(id, p, front_sun);
                    }
               }
          }
     }

     void check_hellhound_fur(vector<int> h, Pos pos, int id, bool &found)
     {
          for (int idh : h)
          {
               Pos ph = unit(idh).pos;
               if (abs(pos.i - ph.i) <= 5 and abs(pos.j - ph.j) <= 5)
               { // this means there is a hound near us
                    found = true;
                    if (ph.j >= pos.j)
                    {
                         for (int p = 5; p < 7; ++p)
                         {
                              Pos new_p = pos;
                              new_p += Dir(p);
                              Cell c = cell(new_p);
                              if (c.type != Rock and c.id == -1)
                              {
                                   command(id, Dir(p));
                                   return;
                              }
                         }
                         Pos new_p = pos;
                         new_p += Dir(Top);
                         Cell c = cell(new_p);
                         if (c.type != Rock and c.id == -1)
                         {
                              command(id, Dir(Top));
                              return;
                         }
                         new_p += Dir(Bottom);
                         c = cell(new_p);
                         if (c.type != Rock and c.id == -1)
                         {
                              command(id, Dir(Bottom));
                              return;
                         }
                    }
                    else if (ph.j < pos.j)
                    {
                         for (int p = 1; p < 4; ++p)
                         {
                              Pos new_p = pos;
                              new_p += Dir(p);
                              Cell c = cell(new_p);
                              if (c.type != Rock and c.id == -1)
                              {
                                   command(id, Dir(p));
                                   return;
                              }
                         }
                         Pos new_p = pos;
                         new_p += Dir(Top);
                         Cell c = cell(new_p);
                         if (c.type != Rock and c.id == -1)
                         {
                              command(id, Dir(Top));
                              return;
                         }
                         new_p += Dir(Bottom);
                         c = cell(new_p);
                         if (c.type != Rock and c.id == -1)
                         {
                              command(id, Dir(Bottom));
                              return;
                         }                         
                    }
               }
          }
     }

     Pos check_enemy_furyans(Pos pos, int id){
          vector<int>f1 = furyans((me()+1)%4);
          vector<int>f2 = furyans((me()+2)%4);
          vector<int>f3 = furyans((me()+3)%4);
          for(long unsigned int i = 0; i < f1.size(); ++i){
               int pfi = unit(f1[i]).pos.i;
               int pfj = unit(f1[i]).pos.j;
               if(abs(pos.i - pfi) <= 3 and abs(pos.j - pfj) <= 3){
                    return Pos(pfi, pfj, 0);
               }
          }
          for(long unsigned int i = 0; i < f2.size(); ++i){
               int pfi = unit(f2[i]).pos.i;
               int pfj = unit(f2[i]).pos.j;
               if(abs(pos.i - pfi) <= 3 and abs(pos.j - pfj) <= 3){
                    return Pos(pfi, pfj, 0);
               }
          }
          for(long unsigned int i = 0; i < f3.size(); ++i){
               int pfi = unit(f3[i]).pos.i;
               int pfj = unit(f3[i]).pos.j;
               if(abs(pos.i - pfi) <= 3 and abs(pos.j - pfj) <= 3){
                    return Pos(pfi, pfj, 0);
               }
          }
          return Pos(-1,-1,-1);
     }


     void move_pioneers()
     {
          vector<int> my_pion = pioneers(me());
          int n = my_pion.size();
          int front_sun = (round() * 2 - 1) % 80;
          vector<int> h = hellhounds();
          for (int id : my_pion)
          {
               Pos pos = unit(id).pos; // returns the position of the pioneer
               // im on the surface
               if (pos.k == 1)
               {    
                    bool found = false; // determines wether we found gems or not
                    find_gems(id, pos, found);
                    if (not found)
                         find_elevator(pos, id, found);

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
                    bool found = false;
                    // here, we will first see if we are near a hellhound
                    check_hellhound(h, pos, id, found, front_sun);
                    if (not found)
                    {
                         Pos p = check_enemy_furyans(pos, id);
                         if(pos_ok(p)){
                              if(p.j >= pos.j){
                                   go_left(id, pos, front_sun);
                              }
                              else if(p.j < pos.j){
                                   go_right(id, pos, front_sun);
                              }
                         }
                         else{
                              bool moved = false;
                              bool able = false;
                              move_caves_random(id, pos, moved, front_sun, n);
                              if (not moved)
                                   backtrack(id, able);
                              if (not able)
                                   command(id, Dir(random(0, 8)));
                              just_went_down[id] = false;
                         }
                    }
               }
          }
     }
     // checks if there is an enemy near us y retorna su posición
     Pos check_enemy(Pos pos){
          queue<Pos> Q;
          map<Pos, bool>vis;
          Q.push(pos);vis[pos] = true;
          while(not Q.empty()){
               Pos p = Q.front(); Q.pop();
               if(cell(p).id!= -1 and unit(cell(p).id).player != me()) return p;

               for(int i = 0; i < 8; ++i){
                    Pos vecino = p;
                    vecino +=Dir(i);
                    if(pos_ok(vecino)){
                         bool vi = vis[vecino];
                         if(cell(vecino).type != Rock){
                              if(not vi){
                                   Q.push(vecino);
                                   vis[vecino] = true;
                              }
                         }
                    }
               }
          }
          return Pos(-1,-1,-1);
     }

     //devuelve si son adyacentes o no
     bool is_adjacent(Pos pos, Pos dest){
          for(int i = 0; i < 8; ++i){
               Pos n_p = pos;
               n_p += Dir(i);
               if(n_p == dest) return true;
          }
          return false;
     }

     // finds the pioneers around me, aqui dest será pos y pos dest de la función check_enemy
     Pos go_enemy(Pos pos, Pos dest){
          queue<Pos> Q;
          map<Pos, bool>vis;
          Q.push(pos);vis[pos] = true;
          while(! Q.empty()){
               Pos p = Q.front(); Q.pop();
               if(is_adjacent(p, dest)) return p;
               for(int i = 0; i < 8; ++i){
                    Pos vecino = p;
                    vecino += Dir(i);
                    if(pos_ok(vecino)){
                         bool &vi = vis[vecino];
                         if(cell(vecino).type != Rock){
                              if(not vi){
                                   Q.push(vecino);
                                   vis[vecino] = true;
                              }
                         }
                    }
               }
          }
          return Pos(-1,-1,-1);
     }

     //mueve el id en la dirección del camino más corto de pos a dest


     Pos find_closest_pioneer(Pos pos, int me)
     {
          vector<int> v1 = pioneers((me + 1) % 4);
          vector<int> v2 = pioneers((me + 2) % 4);
          vector<int> v3 = pioneers((me + 3) % 4);
          double min = 9999;
          Pos pos_min;
          for (long unsigned int i = 0; i < v1.size(); ++i)
          {
               Pos pp = unit(v1[i]).pos;
               int posi = pos.i - pp.i;
               int posj = pos.j - pp.j;

               int d = sqrt(posi * posi + posj * posj);
               if (d < min)
               {
                    d = min;
                    pos_min = pp;
               }
          }
          for (long unsigned int i = 0; i < v2.size(); ++i)
          {
               Pos pp = unit(v1[i]).pos;
               int posi = pos.i - pp.i;
               int posj = pos.j - pp.j;

               int d = sqrt(posi * posi + posj * posj);
               if (d < min)
               {
                    d = min;
                    pos_min = pp;
               }
          }
          for (long unsigned int i = 0; i < v3.size(); ++i)
          {
               Pos pp = unit(v1[i]).pos;
               int posi = pos.i - pp.i;
               int posj = pos.j - pp.j;

               int d = sqrt(posi * posi + posj * posj);
               if (d < min)
               {
                    d = min;
                    pos_min = pp;
               }
          }
          return pos_min;
     }


     // improve so it does not move in a simply random way, it must move away from rocks and its own units
     void move_furyans()
     {
          vector<int> my_fur = furyans(me());
          vector<int>h = hellhounds();
          int n = my_fur.size();
          for (int id : my_fur)
          {
               Pos pos = unit(id).pos;
               bool hound = false;
               check_hellhound_fur(h, pos, id, hound);
               if(not hound){
                    bool enemy = false;
                    Pos en = check_enemy(pos);
                    if (pos_ok(en))
                    {    
                         Pos go = go_enemy(en, pos);
                         find_pos(id, pos, go);
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