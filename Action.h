
#ifndef action_cpp
#define action_cpp

class action{
public:
char w[1][80]; //"magic" words to trigger action
int word; //number of words in w
int what; //object num
int what2;//for two-objects actions and for hero's interactions (talking etc) - then it's hero's num
int where;//scene num
int conditions[100][3]; //action num (with scene num) and action state in order for this action to be done
int results[100][3]; //action num (with scene num) and action state to be changed afterwards
int inv_change[10]; //items to be added to inventory
int inv_change_s[10]; // scene of it
int state; //action state: 0 - ready, 1 - done, -1 - cannot be done
int num; //ID of action to be able to initialize it
char answer[500]; //phrase you see on the screen afterwards
int score; //score

void show_result(int *,WINDOW *);
};
#endif