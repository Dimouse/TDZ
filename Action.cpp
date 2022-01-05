#include "stdafx.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <fstream.h>
#include <ios>
#include "TDZ4.h"
#include "Action.h"

//show action.answer
void action::show_result(int *line, WINDOW *win){
	int bold=0;
	int q2=tab;
	for(int q=0;q<int(strlen(action::answer));q++){
		if(action::answer[q]=='*'&&bold==0){
			wattrset(win, COLOR_PAIR(2));
			bold=1;
		}
		else if(action::answer[q]=='*'&&bold==1){
			wattrset(win, COLOR_PAIR(1));
			bold=0;
		}
		else{ //scroll with 'perenos' option
			if(unsigned char(action::answer[q])==32&&perenos==1){ //space
				int eol=q2%80+find_next_space(action::answer,q);
				if(eol>79){
					q2+=79-q2%80;
					*line+=1;
					while(*line>winsize){
						scroll(win);
						*line-=1;
					}
				}
			}
			mvwaddch(win, *line/*+q2/80*/, q2%80, unsigned char(action::answer[q]));
			q2++;
			if(q2%80==0&&perenos==0){ //scroll without 'perenos' option
			*line+=1;
			while(*line>winsize){
				scroll(win);
				*line-=1;
			}
			}
		}
	}
	//*line+=/*strlen(action::answer)*/q2/80+1;
	if(action::score>0&&action::state!=3){
	wattrset(win, COLOR_PAIR(3));
	*line+=1;
	while(*line>winsize){
				scroll(win);
				*line-=1;
	}
			char tmpstr[10],tmpstr2[10];
			strcpy(tmpstr2,"счет +");
			itoa(action::score,tmpstr,10);
			strcat(tmpstr2,tmpstr);
			mvwaddstr2(win, *line, 0, tmpstr2);
	/**line+=1;
	if(*line>20){
				scroll(win);
				*line-=1;
	}*/
	//scroll(win);
	//scroll(win);
	game_score+=action::score;
	}	
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
if(debug) printf("action shown\n");
}
