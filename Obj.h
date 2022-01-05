#ifndef obj_cpp
#define obj_cpp

class obj{
public:
char discr[1000]; //desciption (result of "look at" command)
char name[30]; //name (one word!!!)
char name_t[30]; //tvoritel'nyj padeg! (pogovorit' s kem-chem)
char name_d[30]; //datel'nyj padeg! (otdat' komu-chemu)
char name_v[30]; //vinitel'nyj padeg! (osmotret' kogo-chto)
int num; //ID
int cd; //current discription (default=0)

void show_discr(int *,WINDOW *,int,bool);
};
#endif