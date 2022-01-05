#include "Obj.h"
#include "Action.h"

#ifndef scene_cpp
#define scene_cpp

class scene{
public:
char discr[2000]; //discription
char name[30]; //name (multiply words, only for internal use)
obj o[100]; //objects
int objnum; //number of objects
action a[100]; //actions in this scene
int actnum; //number of actions
int num; //ID
int exits[4]; //exits
int cd; //current discr

void show_discr(int *,WINDOW *);
int decl();
};
#endif