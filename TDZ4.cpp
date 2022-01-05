// TDZ - Text engine of Dimouse & Zubik for IF games, december 2007 by Dimouse
// august 2013 - work continued

#include "stdafx.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <fstream.h>

#include "TDZ4.h"
#include "Verb.h"
#include "Obj.h"
#include "Action.h"
#include "Scene.h"
#include "Parser.h"

using namespace std;

int game_score;
bool perenos;
int tab;
scene s1[SN];
int SN_real;
int inv[100];
int lastitem;
verb v[30];
bool view;
int echo_on;
int curscene;
int cur_verb;
int debug;
char dunno[200];
bool dunno_f;
int verbs;
int cmp_mode;
int winsize;

WINDOW *win_verbs;

int strcmp2(const char* str1,const char* str2){
//str2 - input word
//str1 - actual verb
if(cmp_mode==0)return strcmp(str1,str2);
else if (cmp_mode >= strlen(str2)) return strncmp(str1,str2,cmp_mode);
else return strncmp(str1,str2,strlen(str2));
}

int find_next_space(char *str, int pos){
for(int q=pos+1;q<strlen(str);q++)
if(str[q]==32) return q-pos;
return -1; //abnormal termination
}

void mvwaddstr2(WINDOW *win, int x, int y, char *str){
//mvwaddstr(win, x, y, str);
if(perenos==0) mvwaddstr(win, x, y, str);
if(perenos==1){
	int q2=y;
	for(int q=0;q<strlen(str);q++){
		if(str[q]==32){
			int eol=q2%80+find_next_space(str,q);
			if(eol>79){
				q2+=79-q2%80;
			}
		}
		mvwaddch(win,x+q2/80,q2%80,unsigned char(str[q]));
		q2++;
	}
}
}

/*class hero{
public:
char name[30];
action ac[100];
};
*/

//--------------------------------------------------
int WaitForUser(void);
int SubWinTest(WINDOW *);
int BouncingBalls(WINDOW *);
void trap(int);
//int to_rus(int);
int save_game();
int load_game();

//--------------------------------------------------


void draw_verbs(){
//char verbs[22][20]={"выход","помощь","осмотреться","инвентарь","осмотреть","поговорить","взять","съесть",
//"совместить","обнюхать","надеть","снять","ощупать","открыть","закрыть","отдать","прочитать","идти","нажать"};

int pos=0;
wattrset(win_verbs, COLOR_PAIR(1));
//if(cur_verb==i) wattrset(win_com, COLOR_PAIR(1));
for(int i=0;i<verbs;i++){
if(cur_verb==i) wattrset(win_verbs, COLOR_PAIR(3));
if(cur_verb+1==i) wattrset(win_verbs, COLOR_PAIR(1));
if(80-pos%80 < strlen(v[i].name)) pos+=80-pos%80;
mvwaddstr2(win_verbs, pos/80, pos%80, v[i].name);
pos+=strlen(v[i].name)+1;
}
wrefresh(win_verbs);
}

int WaitForUser(void)
{
	chtype ch;

	nodelay(stdscr, TRUE);
	halfdelay(50);

	ch = getch();

	nodelay(stdscr, FALSE);
	nocbreak();		/* Reset the halfdelay() value */
	cbreak();

	return (ch == '\033') ? ch : 0;
}

/* Trap interrupt */

void trap(int sig)
{
	if (sig == SIGINT)
	{
		endwin();

		exit(0);
	}
}

//MAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAIN
int _tmain(int argc, char **argv)
{
WINDOW *win;

int seed, width, height;
int j,i;
int end_all=0;
int line=3;

perenos=0;
dunno_f=0;
view=0;
curscene=1;
cmp_mode=3;
echo_on=2;
tab=2;
debug=0;
winsize=20;

#ifdef XCURSES
	Xinitscr(argc, argv);
#else
	initscr();
#endif
	seed = time((time_t *)0);
    srand(seed);

	start_color();
# if defined(NCURSES_VERSION) || (defined(PDC_BUILD) && PDC_BUILD > 3000)
	use_default_colors();
# endif
	cbreak();
	noecho();

	curs_set(0);

#if !defined(__TURBOC__) && !defined(OS2)
	signal(SIGINT, trap);
#endif
	noecho();

	/* refresh stdscr so that reading from it will not cause it to 
	   overwrite the other windows that are being created */

	refresh();

	/* Create a drawing window */

	width  = 80;
	height = 25;
	cur_verb=0;
	game_score=0;
//	cmp_mode=3;

	std::fstream config;
	char tmp[100];
	int red,green,blue;
	int red2,green2,blue2;


	config.open("tdz.cfg",ios::in);
	if(config.is_open()){
		config>>tmp;
		config>>red>>green>>blue;
		init_color(10, red, green, blue);
		config>>red2>>green2>>blue2;
		init_color(11, red2, green2, blue2);
		init_pair(1, 10, 11);

		config>>red>>green>>blue;
		init_color(12, red, green, blue);
		config>>red2>>green2>>blue2;
		init_color(13, red2, green2, blue2);
		init_pair(2, 12, 13);

		config>>red>>green>>blue;
		init_color(14, red, green, blue);
		config>>red2>>green2>>blue2;
		init_color(15, red2, green2, blue2);
		init_pair(3, 14, 15);
	
		config>>tmp>>tmp; //perenos
		if(strcmp(tmp,"On")==0)perenos=1;
		else perenos=0;

		config>>tmp; //answer for dunno
		config>>tmp;

		strcpy(dunno,"");
		while(strcmp(tmp,"/")!=0){
			strcat(dunno,tmp);
			strcat(dunno," ");
			config>>tmp;
		}

		if(strcmp(dunno,"Off ")==0)
			dunno_f=0;
		else dunno_f=1;

		config>>tmp; //interface type, classic or casual
		config>>tmp;
		if(strcmp(tmp,"Casual")==0)
			view=1;
		else view=0;
		config.close();
		if(debug) printf("configuration loaded ok\n");
	}
	else{
		view=1;
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
		init_pair(2, COLOR_RED, COLOR_BLUE);
//	wattrset(win, COLOR_PAIR(2));
		init_pair(3, COLOR_GREEN, COLOR_BLUE);
//	wattrset(win, COLOR_PAIR(3));
	}

	WINDOW *win1,/**win_verbs,*/*win_com;
	
	if(view==1){ //casual mode on!
		win1 = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2); //full windows
		//win1=newwin(0,0,0,0);
		win = newwin(22, 80, 0, 0); //windows for game
		win_verbs = newwin(2, 80, 23, 0); //windows for verbs
		win_com = newwin(1, 80, 22, 0); //window for command line

		if (win1 == NULL||win==NULL||win_verbs==NULL||win_com==NULL)
		{
			endwin();
			return 1;
		}
		wclear(win1);
		wclear(win_verbs);
		wclear(win_com);
		overlay(win,win1);
		overlay(win_verbs,win1);
		overwrite(win_com,win1);
		//wbkgd(win, COLOR_PAIR(1));
		wbkgd(win_verbs, COLOR_PAIR(1));
		wbkgd(win_com, COLOR_PAIR(3));
		wrefresh(win1);
		wrefresh(win_verbs);
		wrefresh(win_com);

	}
	else{
		win = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
		
		if (win == NULL)
		{
			endwin();
			return 1;
		}
	}

	winsize = win->_bmarg - 1;
//	printf("%d\n", winsize);

	wbkgd(win, COLOR_PAIR(1));

	werase(win);
	wrefresh(win);
    wattrset(win, 0);
	wtimeout(win, 200);
	keypad(win, TRUE);
if(view==1){
	wtimeout(win_com,200);
	keypad(win_com, TRUE);
}
	PDC_save_key_modifiers(TRUE);
	PDC_return_key_modifiers(TRUE);
	curs_set(0);		/* turn cursor off */
	scrollok(win, TRUE);

//SAVE GAME check - outdated!
	/*int loadgame=0;
	fstream savegame;
	char savephrase[20];
	savegame.open("save.dat",ios::in);
	if(savegame.is_open()){
		savegame>>savephrase;
		if(strcmp(savephrase,"sordid_is_here")==0) loadgame=1;
		savegame.close();
	}
	*/

	parser par;
	for(int q=0;q<100;q++)inv[q]=0;

	curscene=1; //first scene
//	inv[0]=105; //book
	lastitem=0; //items

	//load verbs
	int decl_st=1;
	int j2=0;
	while(decl_st==1){
		v[j2].num=j2;
		decl_st=v[j2].decl();
		if(debug) printf("%d: %s, %d, %d, %d\n",v[j2].num,v[j2].name,v[j2].what,v[j2].words,v[j2].padeg);
		j2++;
	}
	if(debug)printf("verbs loaded ok\n");
	verbs=j2;
	//if(decl_st==0) return 0;

	//load scenes
	decl_st=1;
	j2=0;
	while(decl_st==1){
		s1[j2].num=j2;
		decl_st=s1[j2].decl();
		j2++;
	}
	if(debug)printf("scenes loaded ok\n");
	SN_real=j2;
	if(decl_st==0) return 0;
	if(SN_real==0) return 0;

	s1[curscene].show_discr(&line, win);
	
	//s1[0].show_discr(&line, win);

	wattrset(win, COLOR_PAIR(3));
//	mvwaddstr2(win, 0, 22, "Dimouse & Zubik corporation presents:");
//	mvwaddstr2(win, 1, 22, "Simon the Sorcerer 1.5 Textmode Demo.");
	mvwaddstr2(win, 0, 40-strlen(s1[0].name)/2, s1[0].name);
	mvwaddstr2(win, 1, 40-strlen(s1[0].discr)/2, s1[0].discr);

	/*if(loadgame==1){
		mvwaddstr2(win, 2, 22, "Loading saved game... Successful!");
	}*/

//main loop
	while(!end_all){
		wrefresh(win);
		if(view==0)if(par.read(line,win))line++;
		if(view==1){
			par.read(0,win_com); //read user input
			wrefresh(win);
			wclear(win_com);
			wrefresh(win_com);
		}

		//mvwaddstr(win, 20, 1, par.buffer[0]);
		wattrset(win, COLOR_PAIR(1));
		if(!par.analyze(&line,win)) end_all=1; //analyze, if return 0 - game over
/*
		for(int q=0;q<s1[curscene].actnum;q++) //game win, check some state???
			if(s1[1].a[q].num==8)
				if(s1[1].a[q].state==1){ //demo finished
					wattrset(win, COLOR_PAIR(3));
					mvwaddstr2(win, line, 0, "This demo is finished.");
					end_all=1;
				}
*/
		//scroll window if needed
		if(line>winsize){
			for(int q=0;q<line-winsize;q++)scroll(win);
			line=winsize;
		}						
		
		//clear stuff
		for(j=0;j<10;j++)
		for(i=0;i<80;i++) par.buffer[j][i]=0;
		par.word=0;
	}

	nodelay(stdscr, FALSE);
	wattrset(win, COLOR_PAIR(3));
	mvwaddstr2(win, line, 0, "Пока!");
	end_all=0;
	while (!end_all) //press any key
	{
		char c = wgetch(win);
		if (c != ERR) end_all=1;
	}
	
	//wgetch(win);
	//napms(5000);
	endwin();

	return 0;
}

/* End of NEWDEMO.C */
