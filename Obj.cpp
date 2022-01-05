#include "stdafx.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <fstream.h>
#include <ios>
#include "Obj.h"
#include "TDZ4.h"

//show object desription after look at command
void obj::show_discr(int *line, WINDOW *win, int tab, bool perenos){
	int bold=0;
	int q2=tab;
	int nsob=0;
	int schar=0;
	for(int q=0;q<int(strlen(obj::discr));q++){
		if(nsob==obj::cd){
			schar=q;
			break;
		}
		if(obj::discr[q]=='@')nsob++;
	}

	for(int q=schar;q<int(strlen(obj::discr));q++){
		if(obj::discr[q]=='@') break;
		if(obj::discr[q]=='*'&&bold==0){
			wattrset(win, COLOR_PAIR(2));
			bold=1;
		}
		else if(obj::discr[q]=='*'&&bold==1){
			wattrset(win, COLOR_PAIR(1));
			bold=0;
		}
		else{
			if(unsigned char(obj::discr[q])==32&&perenos==1){ //space
				int eol=q2%80+find_next_space(obj::discr,q);
				if(eol>79){
					q2+=79-q2%80;
					*line+=1;
					while(*line>winsize){
						scroll(win);
						*line-=1;
					}
				}
			}
			mvwaddch(win, *line/*+q2/80*/, q2%80, unsigned char(obj::discr[q]));
			q2++;
			if(q2%80==0&&perenos==0){
			*line+=1;
			while(*line>winsize){
				scroll(win);
				*line-=1;
			}
		}
		}
	}
	//*line+=/*strlen(obj::discr)*/q2/80+1;
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
}
