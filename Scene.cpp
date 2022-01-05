#include "stdafx.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <fstream.h>
#include <ios>
#include "Verb.h"
#include "Obj.h"
#include "Action.h"
#include "Scene.h"
#include "TDZ4.h"

int scene::decl(){
//the description of file format is as follows:
//&
//name of location/
//discription of location (result of "look around") with * for marking active words/
//4 numbers for exits -1 - inactive, 1 - active
//objects in the scene, until zero (0) as follows:
//ID_num name name_v name_d name_t discr /
//actions in the scene, until zero (0) as follows:
//command word obj scene cond_num cond_scene_num cond_state res_num res_scene_num res_state inv_to_add inv_to_add_scene state score num answer

std::fstream f;
char t[255];
int nnum=0, exit=0;
f.open("scenes.dat",std::ios::in);
if(!f.is_open()) return 0;

while(!f.eof()&&!exit){
	f>>t;
	if(strcmp(t,"&")==0) nnum++;
	if(nnum==scene::num+1) exit=1;
}

f>>t;
strcpy(scene::name,"");
while(strcmp(t,"/")!=0){
	strcat(scene::name,t);
	strcat(scene::name," ");
	f>>t;
}
f>>t;
strcpy(scene::discr,"");
while(strcmp(t,"/")!=0){
	strcat(scene::discr,t);
	strcat(scene::discr," ");
	f>>t;
}
if(debug)printf("%s: %s\n",scene::name, scene::discr);

f>>scene::exits[0];
f>>scene::exits[1];
f>>scene::exits[2];
f>>scene::exits[3];

scene::cd=0;

//objects declaration
int curnum=0,onum=1;

//f>>onum;
while(onum!=0&&curnum<100){
	f>>onum;
	scene::o[curnum].num=onum;
	f>>scene::o[curnum].name;
	f>>scene::o[curnum].name_t;
	f>>scene::o[curnum].name_d;
	f>>scene::o[curnum].name_v;
	scene::o[curnum].cd=0;
	f>>t;
	if(scene::num==0){ //objects in 0 scene - initial inventory
		if(onum!=0){
			inv[lastitem]=onum;
			lastitem++;
		}
	}
	strcpy(scene::o[curnum].discr,"");
	while(strcmp(t,"/")!=0){ //read obj descriptions
		strcat(scene::o[curnum].discr,t);
		strcat(scene::o[curnum].discr," ");
		f>>t;
	}
	if(onum!=0) curnum++;
}
scene::objnum=curnum;

//and now for actions declaration:
onum=1;
int anum=0;
curnum=0;
int curnum2=0;
while(onum!=0&&curnum<100){
	f>>onum; //verb id
	//scene::a[curnum].what=onum;
/*	if(onum==1) strcpy(scene::a[curnum].w[0],"поговорить");
	if(onum==2) strcpy(scene::a[curnum].w[0],"взять");
	if(onum==3) strcpy(scene::a[curnum].w[0],"съесть");
	if(onum==4) strcpy(scene::a[curnum].w[0],"совместить");
	if(onum==5) strcpy(scene::a[curnum].w[0],"обнюхать");
	if(onum==6) strcpy(scene::a[curnum].w[0],"надеть");
	if(onum==7) strcpy(scene::a[curnum].w[0],"снять");
	if(onum==8) strcpy(scene::a[curnum].w[0],"ощупать");
	if(onum==9) strcpy(scene::a[curnum].w[0],"открыть");
	if(onum==10)strcpy(scene::a[curnum].w[0],"закрыть");
	if(onum==11)strcpy(scene::a[curnum].w[0],"отдать");
	if(onum==12)strcpy(scene::a[curnum].w[0],"прочитать");
	if(onum==13)strcpy(scene::a[curnum].w[0],"нажать");
*/
	if(onum<=verbs)strcpy(scene::a[curnum].w[0],v[onum].name);

	f>>scene::a[curnum].word; //number of words
	f>>scene::a[curnum].what; //obj id
//	if(onum==4||onum==11) f>>scene::a[curnum].what2; //obj id 2 for certain verbs (merge items and give item to person)
	if(v[onum].what==2) f>>scene::a[curnum].what2;
	else scene::a[curnum].what2=0;
	
	f>>scene::a[curnum].where; //scene id
if(debug) printf("%d %d %d %d\n",onum,scene::a[curnum].word,scene::a[curnum].what,scene::a[curnum].where);
	f>>t; // /
	anum=1;
	curnum2=0;
	//actions
	while(anum!=0){
		f>>anum;
		scene::a[curnum].conditions[curnum2][0]=anum; //action id
		f>>scene::a[curnum].conditions[curnum2][1]; //scene id
		f>>scene::a[curnum].conditions[curnum2][2]; //state
if(debug) printf("condition: %d %d %d\n",scene::a[curnum].conditions[curnum2][0],scene::a[curnum].conditions[curnum2][1],scene::a[curnum].conditions[curnum2][2]);
		curnum2++;
	}
	anum=1;
	curnum2=0;
	f>>t; // /
	while(anum!=0){
		f>>anum;
		scene::a[curnum].results[curnum2][0]=anum; //result action id
		f>>scene::a[curnum].results[curnum2][1]; //result scene id
		f>>scene::a[curnum].results[curnum2][2]; //result state
		curnum2++;
	}
	f>>t; // /
	anum=1;
	curnum2=0;
	//inventory change
	while(anum!=0){
		f>>anum;
		scene::a[curnum].inv_change[curnum2]=anum;
	//	if(anum!=0)
			f>>scene::a[curnum].inv_change_s[curnum2];
		curnum2++;
	}
	f>>scene::a[curnum].state;
	f>>scene::a[curnum].score;
	f>>scene::a[curnum].num;

if(debug) printf("action: %d, state/score/num: %d %d %d\n",curnum,scene::a[curnum].state,scene::a[curnum].score,scene::a[curnum].num);

	f>>t;
	strcpy(scene::a[curnum].answer,"");
	while(strcmp(t,"/")!=0){
		strcat(scene::a[curnum].answer,t);
		strcat(scene::a[curnum].answer," ");
		f>>t;
	}
if(debug) printf("%s\n",scene::a[curnum].answer);
	curnum++;
}
scene::actnum=curnum;

if(debug)printf("Finish init scene\n");

f>>t;
if(strcmp(t,"&")!=0){ //next scene identifier
f.close();
return 2; //end of scene.dat
}

f.close();
return 1;
}

//show scene description after look around command
void scene::show_discr(int *line, WINDOW *win){
	int bold=0;
	int q2=tab;
	int nsob=0;
	int schar=0;
	for(int q=0;q<int(strlen(scene::discr));q++){
		if(nsob==scene::cd){
			schar=q;
			break;
		}
		if(scene::discr[q]=='@')nsob++;
	}
	wattrset(win, COLOR_PAIR(1));

	for(int q=schar;q<int(strlen(scene::discr));q++){
		if(scene::discr[q]=='@') break;
		if(scene::discr[q]=='*'&&bold==0){
			wattrset(win, COLOR_PAIR(2));
			bold=1;
		}
		else if(scene::discr[q]=='*'&&bold==1){
			wattrset(win, COLOR_PAIR(1));
			bold=0;
		}
		else{
			if(unsigned char(scene::discr[q])==32&&perenos==1){ //space
				int eol=q2%80+find_next_space(scene::discr,q);
				if(eol>79){
					q2+=79-q2%80;
					*line+=1;
					while(*line>winsize){
						scroll(win);
						*line-=1;
					}
				}
			}
			mvwaddch(win, *line/*+q2/80*/, q2%80, unsigned char(scene::discr[q]));
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
	*line+=1;

	q2=20;
	int first=1;
	mvwaddstr(win, *line, 0, "Вы видите выходы к: "); //show exits
	if(scene::exits[0]!=-1){
		mvwaddstr(win, *line, q2, "востоку");
		q2+=7;
		first=0;
	}
	if(scene::exits[1]!=-1){
		if(!first){
			mvwaddstr(win, *line, q2, ", ");
			q2+=2;
		}
		mvwaddstr(win, *line, q2, "западу");
		q2+=6;
		first=0;
	}
	if(scene::exits[2]!=-1){
		if(!first){
			mvwaddstr(win, *line, q2, ", ");
			q2+=2;
		}
		mvwaddstr(win, *line, q2, "северу");
		q2+=6;
		first=0;
	}
	if(scene::exits[3]!=-1){
		if(!first){
			mvwaddstr(win, *line, q2, ", ");
			q2+=2;
		}
		mvwaddstr(win, *line, q2, "югу");
		q2+=3;
		first=0;
	}
	mvwaddstr(win, *line, q2, ".");
	*line+=1;

	while(*line>winsize){
				scroll(win);
				*line-=1;
	}
//	*line+=strlen(scene::discr)/80+1;
	//cout<<"You see exits to ";
	//if(scene::exits[0]!=-1) cout<<"East ";
	//if(scene::exits[1]!=-1) cout<<"West ";
	//if(scene::exits[2]!=-1) cout<<"North ";
	//if(scene::exits[3]!=-1) cout<<"South ";
	//cout<<endl;
}
