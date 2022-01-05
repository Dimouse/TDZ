#define	SN 50 //max scenes num

#include "Verb.h"
#include "Scene.h"

#ifndef tdz_cpp
#define tdz_cpp

int find_next_space(char *str, int pos);
int strcmp2(const char* str1,const char* str2);
void mvwaddstr2(WINDOW *win, int x, int y, char *str);

extern int game_score;
extern verb v[30];
extern scene s1[SN]; //global and game levels

extern int verbs; //actual verbs num
extern int lastitem;
extern bool perenos;
extern char dunno[200];
extern bool dunno_f;
extern bool view;
extern int cur_verb;
extern int curscene;
extern int cmp_mode; //compare mode: if 0, than compare by full words, if not, compare by first max(cmp_mode,len(input)) chars of dest word
extern int SN_real; //actual scenes num
extern int tab; //tabulation in output
extern int debug;
extern int echo_on; //echo input line to output, 1 - simple, 2 - advanced
extern int winsize; //size of scrolling

extern int inv[100]; //inventory - IDs of objects that you currently possess

//extern WINDOW *win_verbs;

#endif