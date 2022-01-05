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
#include "Scene.h"
#include "Parser.h"

int to_rus(int c){
if(c==97||c==65) return 228;
if(c==98||c==66) return 168;
if(c==99||c==67) return 225;
if(c==100||c==68)return 162;
if(c==101||c==69)return 227;
if(c==102||c==70)return 160;
if(c==103||c==71)return 175;
if(c==104||c==72)return 224;
if(c==105||c==73)return 232;
if(c==106||c==74)return 174;
if(c==107||c==75)return 171;
if(c==108||c==76)return 164;
if(c==109||c==77)return 236;
if(c==110||c==78)return 226;
if(c==111||c==79)return 233;
if(c==112||c==80)return 167;
if(c==113||c==81)return 169;
if(c==114||c==82)return 170;
if(c==115||c==83)return 235;
if(c==116||c==84)return 165;
if(c==117||c==85)return 163;
if(c==118||c==86)return 172;
if(c==119||c==87)return 230;
if(c==120||c==88)return 231;
if(c==121||c==89)return 173;
if(c==122||c==90)return 239;
if(c==91)return 229;
if(c==93)return 234;
if(c==59)return 166;
if(c==39)return 237;
if(c==44)return 161;
if(c==46)return 238;
if(c>=48&&c<=57)return c;

return 0;
}

int restart_game(){
	for(int q=0;q<100;q++)inv[q]=0;
	curscene=1; //first scene
	lastitem=0; //items
	int decl_st=1;
	int j2=0;
	while(decl_st==1){
		s1[j2].num=j2;
		decl_st=s1[j2].decl();
		j2++;
	}
	if(debug)printf("scenes loaded ok\n");
	SN_real=j2;
	if(decl_st==0) return 0;
	if(SN_real==0) return 0;

	return 1;
}

int save_game(){
std::fstream sfile;
sfile.open("save.dat",std::ios::out);
if(!sfile.is_open())return 0;
sfile<<curscene<<" "<<game_score<<std::endl;
sfile<<lastitem<<" ";

for(int j2=0;j2<lastitem;j2++)
sfile<<inv[j2]<<" ";

sfile<<std::endl;
for(int j2=0;j2<SN_real;j2++){
	sfile<<s1[j2].exits[0]<<" "<<s1[j2].exits[1]<<" "<<s1[j2].exits[2]<<" "<<s1[j2].exits[3]<<std::endl;
	sfile<<s1[j2].cd<<std::endl;
	for(int k=0;k<s1[j2].actnum;k++)
		sfile<<s1[j2].a[k].state<<" ";
	sfile<<std::endl;

	if(j2==0){ //scene 0
		sfile<<s1[j2].objnum<<std::endl;
		for(int k=0;k<s1[j2].objnum;k++){
			sfile<<s1[j2].o[k].cd<<" "<<s1[j2].o[k].num<<std::endl;
		}
	}
	else{ //only descr changes
	for(int k=0;k<s1[j2].objnum;k++)
		sfile<<s1[j2].o[k].cd<<" ";
	}
	sfile<<std::endl;
}

sfile.close();

return 1;
}

int load_game(){
std::fstream sfile;
sfile.open("save.dat",std::ios::in);
if(!sfile.is_open())return 0;
sfile>>curscene>>game_score;
sfile>>lastitem;

for(int j2=0;j2<lastitem;j2++)
sfile>>inv[j2];

for(int j2=0;j2<SN_real;j2++){
	sfile>>s1[j2].exits[0]>>s1[j2].exits[1]>>s1[j2].exits[2]>>s1[j2].exits[3];
	sfile>>s1[j2].cd;
	for(int k=0;k<s1[j2].actnum;k++)
		sfile>>s1[j2].a[k].state;

	if(j2==0){ //scene 0
		sfile>>s1[j2].objnum;
		for(int k=0;k<s1[j2].objnum;k++){
			sfile>>s1[j2].o[k].cd>>s1[j2].o[k].num;
			int q = s1[j2].o[k].num - 1; //inventory num is obj num - 1
			//restore objects in inventory scene
			int qtrue=0;
			for(int q2=0;q2<s1[inv[q]/100].objnum;q2++){
				if(s1[inv[q]/100].o[q2].num==inv[q]%100) qtrue=q2;
			}
			s1[j2].o[k] = s1[inv[q]/100].o[qtrue];
			s1[j2].o[k].num=q+1;
		}
	}
	else{ //only descr changes
	for(int k=0;k<s1[j2].objnum;k++)
		sfile>>s1[j2].o[k].cd;
	}
}
sfile.close();

return 1;
}

parser::parser(){
	for(int j=0;j<10;j++){
		//buffer[j] = new char[81];
		for(int i=0;i<80;i++) buffer[j][i]=0;
	}
	word=0;
}

//read user input
int parser::read(int line, WINDOW * win){
void draw_verbs();

static const char spinner[5] = "/-\\|";
int spinner_count = 0;
int end=0;
int c,c2;
int pos=0, pos2=0;
int space_flag=-1; //this means you can insert verb by pressing space

word=0;
if(view==1)draw_verbs();

wattrset(win, COLOR_PAIR(3));
		while (!end)
		{
			c = wgetch(win);

			if (c == ERR)
			{
				spinner_count++;
				if (spinner_count == 4)
					spinner_count = 0;
				mvwaddch(win, line, 1+pos+pos2, spinner[spinner_count]);
				wrefresh(win);
			}
			else{
				if((c2=to_rus(c))!=0&&pos<80){
					//add char to the screen and to the buffer
					buffer[word][pos]=c2;
					mvwaddch(win, line, 1+pos2+pos, c2);
					pos++;
					space_flag=0;
				}
				if(c==8&&pos>0){
					//delete char
					buffer[word][pos-1]=0;
					mvwaddch(win, line, 1+pos2+pos, ' ');
					pos--;
				}
				if(c==32&&space_flag==-1/*&&view==1*/){
					//you add a verb from win_verbs to the command line
					//char verbs[22][20]={"выход","помощь","осмотреться","инвентарь","осмотреть","поговорить","взять","съесть",
					//	"совместить","обнюхать","надеть","снять","ощупать","открыть","закрыть","отдать","прочитать","идти", "нажать"};
					for(int i=0;i<strlen(v[cur_verb].name);i++){
						buffer[word][pos]=unsigned char(v[cur_verb].name[i]);
						mvwaddch(win, line, 1+pos2+pos, unsigned char(v[cur_verb].name[i]));
						pos++;
					}
					//c=32;
					space_flag=0;
					if(v[cur_verb].words==0) c=10;
				}
				if(c==32&&space_flag==0){
					//space pressed - next word
					mvwaddch(win, line, 1+pos2+pos, ' ');
					pos2+=pos+1;
					pos=0;
					word++;
					space_flag=1;
					if(word>=10){
						//line++;
						pos=0;
						pos2=0;
						end=1;
					}
				}
				if(c==10||c==13){
					//finish
					mvwaddch(win, line, 1+pos2+pos, ' ');
					//line++;
					pos=0;
					pos2=0;
					end=1;
				}
				if(view==1){ //verbs routines
					if(c==KEY_LEFT&&cur_verb>0){
						cur_verb--;
						draw_verbs();
					}
					if(c==KEY_RIGHT&&cur_verb<18){
						cur_verb++;
						draw_verbs();
					}


				}
				//if(c!=32) space_flag=0;
			}	
		}
wattrset(win, COLOR_PAIR(1));
return 1;
}

int parser::test_scene(scene *ts,int *line, WINDOW *win){
//and now for main part: actions!!!
//here input line (buffer[10] char* variables) is analyzed and corresponding action is found and executed
int qtrue=0,qtrue2=0;
int wtrue=0,wtrue2=0;
int qact=-1;
int do_flag=0;
int nevozmogn=0;
int showed=0;

//find corresponding verb
int vnum=-1;
for(int j=0;j<verbs;j++){
	if(strcmp2(v[j].name,buffer[0])==0)vnum=j;
}
if(vnum==-1) return 0;

for(int z=0;z<ts->actnum-1;z++){ //loop over actions
	//find corresponding verb
	int vanum=-1;
	qact=-1;

	for(int j=0;j<verbs;j++){
		if(strcmp2(ts->a[z].w[0],v[j].name)==0)vanum=j;
	}
	//	int do_flag=0;
	//for(int q2=0;q2<s1.a[q].word;q2++)

	qtrue=-1;
	qtrue2=-1;
	if(v[vanum].what==2){
		for(int q2=0;q2<s1[ts->a[z].where/1000].objnum;q2++)
			if(s1[ts->a[z].where/1000].o[q2].num==ts->a[z].what){ qtrue=q2; wtrue=ts->a[z].where/1000;}
		for(int q2=0;q2<s1[ts->a[z].where%1000].objnum;q2++)
			if(s1[ts->a[z].where%1000].o[q2].num==ts->a[z].what2){ qtrue2=q2; wtrue2=ts->a[z].where%1000;}
	}
	if(v[vanum].what==1){
//		printf("!!! %d\n",s1[ts->a[q].where].objnum);
		for(int q2=0;q2<s1[ts->a[z].where].objnum;q2++){
//			printf("%d: %s\n",s1[ts->a[q].where].o[q2].num,s1[ts->a[q].where].o[q2].name);
			if(s1[ts->a[z].where].o[q2].num==ts->a[z].what) {qtrue=q2; wtrue=ts->a[z].where;}
		}
	}
	if(qtrue==-1) continue;
	if(v[vnum].what==2 && qtrue2==-1) continue;

	int do_flag2=0;
	if(strcmp2(ts->a[z].w[0],buffer[0])!=0) continue;
	if(debug)printf("test action: where:%d what:%d\n",ts->a[z].where,ts->a[z].what);

	char testall[255];
	strcpy(testall,v[vnum].name);

	int onum=1;
	for(int i=0;i<ts->a[z].word;i++){ //loop over words in magic, they should be equal to magic, symbol # is replaced by object name in corresponding padeg
		char test[255];

		if(strcmp(v[vnum].magic[i],"#")==0){
			if(onum==1){ //first object in magic
				if(v[vnum].padeg==0) strcpy(test,s1[wtrue].o[qtrue].name);
				else if(v[vnum].padeg==1) strcpy(test,s1[wtrue].o[qtrue].name_t);
				else if(v[vnum].padeg==2) strcpy(test,s1[wtrue].o[qtrue].name_d);
				else if(v[vnum].padeg==3) strcpy(test,s1[wtrue].o[qtrue].name_v);
				else strcpy(test,"???");
			}
			else if(onum==2){ //second object in magic
				if(v[vnum].padeg2==0) strcpy(test,s1[wtrue2].o[qtrue2].name);
				else if(v[vnum].padeg2==1) strcpy(test,s1[wtrue2].o[qtrue2].name_t);
				else if(v[vnum].padeg2==2) strcpy(test,s1[wtrue2].o[qtrue2].name_d);
				else if(v[vnum].padeg2==3) strcpy(test,s1[wtrue2].o[qtrue2].name_v);
				else strcpy(test,"???");
			}
			else strcpy(test,"???");
			onum++;
		}
		else{
			strcpy(test,v[vnum].magic[i]);
		}

		strcat(testall," ");
		strcat(testall,test);
//		printf("buffer %s %s\n",v[vnum].name,v[vnum].magic[i]);
//		printf("action %s %s\n",v[vanum].name,test);

		if(strcmp2(test,buffer[i+1])!=0) do_flag2=1; //check if magic and buffer are equal
	}
	if(!do_flag2){
		qact=z;
	}

	do_flag2=0;
	onum=1;
	//different order of words for merge and give to
	for(int i=0;i<ts->a[z].word;i++){ //loop over words in magic, they should be equal to magic, symbol # is replaced by object name in corresponding padeg
		char test2[255];

		//different order of objects to mix or give to
		if(strcmp(v[vnum].magic[i],"#")==0){
			if(onum==1){ //first object in magic
				if(v[vnum].padeg2==0) strcpy(test2,s1[wtrue2].o[qtrue2].name);
				else if(v[vnum].padeg2==1) strcpy(test2,s1[wtrue2].o[qtrue2].name_t);
				else if(v[vnum].padeg2==2) strcpy(test2,s1[wtrue2].o[qtrue2].name_d);
				else if(v[vnum].padeg2==3) strcpy(test2,s1[wtrue2].o[qtrue2].name_v);
				else strcpy(test2,"???");
			}
			else if(onum==2){ //second object in magic
				if(v[vnum].padeg==0) strcpy(test2,s1[wtrue].o[qtrue].name);
				else if(v[vnum].padeg==1) strcpy(test2,s1[wtrue].o[qtrue].name_t);
				else if(v[vnum].padeg==2) strcpy(test2,s1[wtrue].o[qtrue].name_d);
				else if(v[vnum].padeg==3) strcpy(test2,s1[wtrue].o[qtrue].name_v);
				else strcpy(test2,"???");
			}
			else strcpy(test2,"???");

			onum++;
		}
		else{
			strcpy(test2,v[vnum].magic[i]);
		}

	if(strcmp2(test2,buffer[i+1])!=0) do_flag2=1; //check if magic and buffer are equal
	}
	if(!do_flag2){
		qact=z;
	}
	do_flag2=0;
	
	//	printf("qact: %d\n",qact);
/*
	if(ts->a[q].word==3){
		if(strcmp(ts->a[q].w[0],"совместить")==0){ //for merging items
			if(strcmp2(ts->a[q].w[0],buffer[0])!=0)do_flag2=1;
			for(int q2=0;q2<s1[ts->a[q].where/1000].objnum;q2++)
				if(s1[ts->a[q].where/1000].o[q2].num==ts->a[q].what) qtrue=q2;
			for(int q2=0;q2<s1[ts->a[q].where%1000].objnum;q2++)
				if(s1[ts->a[q].where%1000].o[q2].num==ts->a[q].what2) qtrue2=q2;
			if(strcmp2(s1[ts->a[q].where/1000].o[qtrue].name_v,buffer[1])!=0&&strcmp2(s1[ts->a[q].where%1000].o[qtrue2].name_v,buffer[1])!=0)do_flag2=1;
			if(strcmp2(s1[ts->a[q].where/1000].o[qtrue].name_v,buffer[3])!=0&&strcmp2(s1[ts->a[q].where%1000].o[qtrue2].name_v,buffer[3])!=0)do_flag2=1;
			if(strcmp(buffer[2],"и")!=0)do_flag2=1;
			if(!do_flag2){
				//if(s1.a[qact].state!=0)
				qact=q;
			}
			do_flag2=0;
		}
	}
	if(ts->a[q].word==2){
		if(strcmp(ts->a[q].w[0],"отдать")==0){ //give to
			if(strcmp2(ts->a[q].w[0],buffer[0])!=0)do_flag2=1;
			for(int q2=0;q2<s1[ts->a[q].where/1000].objnum;q2++)
				if(s1[ts->a[q].where/1000].o[q2].num==ts->a[q].what) qtrue=q2;
			for(int q2=0;q2<s1[ts->a[q].where%1000].objnum;q2++)
				if(s1[ts->a[q].where%1000].o[q2].num==ts->a[q].what2) qtrue2=q2;

			if(strcmp2(s1[ts->a[q].where/1000].o[qtrue].name_v,buffer[1])!=0)do_flag2=1;
			if(strcmp2(s1[ts->a[q].where%1000].o[qtrue2].name_d,buffer[2])!=0)do_flag2=1;

//			if(strcmp2(s1[ts->a[q].where/1000].o[qtrue].name_v,buffer[1])!=0&&strcmp2(s1[ts->a[q].where%1000].o[qtrue2].name_v,buffer[1])!=0)do_flag2=1;
//			if(strcmp2(s1[ts->a[q].where%1000].o[qtrue2].name_d,buffer[2])!=0&&strcmp2(s1[ts->a[q].where%1000].o[qtrue2].name_v,buffer[2])!=0)do_flag2=1;
			//if(strcmp2(buffer[2],"и")!=0)do_flag2=1;
			if(!do_flag2){
				//if(s1.a[qact].state!=0)
				qact=q;
			}
			do_flag2=0;
		}
		if(strcmp(ts->a[q].w[0],"поговорить")==0){ //talk to
			if(strcmp2(ts->a[q].w[0],buffer[0])!=0)do_flag2=1;
			for(int q2=0;q2<s1[ts->a[q].where].objnum;q2++){
				if(s1[ts->a[q].where].o[q2].num==ts->a[q].what) qtrue=q2;
			}
			if(strcmp2(s1[ts->a[q].where].o[qtrue].name_t,buffer[2])!=0)do_flag2=1;
			if(strcmp(buffer[1],"с")!=0)do_flag2=1;
			if(!do_flag2){
				//if(s1.a[qact].state!=0)
				qact=q;
			}
			do_flag2=0;
		}
		if(strcmp(ts->a[q].w[0],"нажать")==0){ //push
			if(strcmp2(ts->a[q].w[0],buffer[0])!=0)do_flag2=1;
			for(int q2=0;q2<s1[ts->a[q].where].objnum;q2++){
				if(s1[ts->a[q].where].o[q2].num==ts->a[q].what) qtrue=q2;
			}
			if(strcmp2(s1[ts->a[q].where].o[qtrue].name_v,buffer[2])!=0)do_flag2=1;
			if(strcmp(buffer[1],"на")!=0)do_flag2=1;
			if(!do_flag2){
				//if(s1.a[qact].state!=0)
				qact=q;
			}
			do_flag2=0;
		}
	}
	if(ts->a[q].word==1){ //number of words = 1, typical case
		if(strcmp(ts->a[q].w[0],buffer[0])!=0)do_flag2=1; //action word = entered word
		for(int q2=0;q2<s1[ts->a[q].where].objnum;q2++){ //get object in scene pointed by where and what pointers of the action
				if(s1[ts->a[q].where].o[q2].num==ts->a[q].what) qtrue=q2;
		}
		if(strcmp2(s1[ts->a[q].where].o[qtrue].name_v,buffer[1])!=0)do_flag2=1; //this object's name should be second word in entered buffer
		if(!do_flag2){
			//if(qact!=-1){
			//	if(s1.a[qact].state!=0)
if(debug) printf("%s\n",s1[ts->a[q].where].o[qtrue].name_v);
			qact=q;
		}
		do_flag2=0;
	}
//}
*/
	if(qact!=-1){ //action id found
//note that there could be several actions using the same initial values, than the correct one is choosed based on action conditions
	if(!showed){
	showed=1; //if there are more than one action with the same initial values (only difference is conditions), then show only first one
	if(echo_on>1){ //echo input to output
/*	char outstr[255]="";
	for(int i=0;i<=word;i++){
		if(i==0) strcat(outstr,ts->a[qact].w[0]);
		else if(i==word){ //suppose last word is object name
			if(ts->a[qact].where>1000) strcat(outstr,s1[ts->a[qact].where/1000].o[ts->a[qact].what-1].name_v);
			else strcat(outstr,s1[ts->a[qact].where].o[ts->a[qact].what-1].name_v);
		}
		else strcat(outstr,buffer[i]);
		strcat(outstr," ");
	}
*/
	wattrset(win, COLOR_PAIR(2));
	mvwaddstr2(win, *line, 0, testall);	
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
	wattrset(win, COLOR_PAIR(1));
	}
	}
		
	if(ts->a[qact].state!=1){
if(debug)printf("state ok\n");
	//state ok, check conditions:
		int q=0;
		int do_flag=0;
		int cond_w,cond_s,cond_sc;
if(debug) printf("%s\n",ts->a[qact].answer);
		while(ts->a[qact].conditions[q][0]!=0){
			cond_w=ts->a[qact].conditions[q][0]; //condition action num
			cond_s=ts->a[qact].conditions[q][2]; //state
			cond_sc=ts->a[qact].conditions[q][1]; //scene
			int cond_index=0;
			for(int k=0;k<s1[cond_sc].actnum;k++)
				if(cond_w==s1[cond_sc].a[k].num) cond_index=k;

if(debug) printf("!!! condition of action %d (scene %d): %d should be %d\n", cond_w, cond_sc, s1[cond_sc].a[cond_index].state, cond_s);
if(debug) printf("answer for condition: %s\n",s1[cond_sc].a[cond_index].answer);
			if(s1[cond_sc].a[cond_index].state!=cond_s)do_flag=1;
			q++;
		}
		if(do_flag==0){//all conditions are fulfilled
if(debug) printf("!!!conditions fullfilled\n");
			//now change states of actions in result
//mvwaddstr2(win, *line, 0, "OK!!!");
			int cond_w,cond_s,cond_sc;
			q=0;
			//change state
//			printf("condition was: %d\n",ts->a[qact].state);
			if(ts->a[qact].state==0)ts->a[qact].state=1;
			if(ts->a[qact].state==2)ts->a[qact].state=3;

			while(ts->a[qact].results[q][0]!=0){ //loop over results
				cond_w=ts->a[qact].results[q][0]; //changing action id
				cond_s=ts->a[qact].results[q][2]; //result state
				cond_sc=ts->a[qact].results[q][1]; //result scene

				if(cond_w>0){ //normal action result or death
					if(cond_w>=1000){
					//change exits! scene[r[0]-1000].exit[r[2]] = r[1]
						s1[cond_w-1000].exits[ts->a[qact].results[q][2]]=ts->a[qact].results[q][1]; //cur_scene dest_scene exit_num
					}
					else{
if(debug) printf("action %d, result %d\n",cond_w,cond_s);
						int cond_index=0;
						for(int k=0;k<s1[cond_sc].actnum;k++)
							if(cond_w==s1[cond_sc].a[k].num) cond_index=k;

						s1[cond_sc].a[cond_index].state=cond_s;
					}
				}
				else{ //change description
					if(cond_w==-1&&cond_s==-1){ //death
							ts->a[qact].show_result(line,win);
							//napms(5000);
							return -1; //death
						}
					if(ts->a[qact].results[q][0]==-1000)
					//change scene description!
					s1[ts->a[qact].results[q][1]].cd = ts->a[qact].results[q][2];
					else
					//change objects descriptions!
					s1[ts->a[qact].results[q][1]].o[(-1)*ts->a[qact].results[q][0]-1].cd = ts->a[qact].results[q][2];
				}
				q++;
			}
			q=0;
			//change inventory now:
			while(ts->a[qact].inv_change[q]!=0){
				if(ts->a[qact].inv_change_s[q]>0){ //add item
					inv[lastitem]=ts->a[qact].inv_change_s[q]*100+ts->a[qact].inv_change[q];
					lastitem++;

					if(debug)printf("s1[0].objnum = %d\n",s1[0].objnum);
					int qobj=0;
					for(int qi=0;qi<s1[ts->a[qact].inv_change_s[q]].objnum;qi++)
						if(s1[ts->a[qact].inv_change_s[q]].o[qi].num == ts->a[qact].inv_change[q]) qobj=qi;
					s1[0].o[s1[0].objnum]=s1[ts->a[qact].inv_change_s[q]].o[qobj];
					s1[0].o[s1[0].objnum].num=lastitem;
					s1[0].objnum++;
				}
				else{ //remove item
					int q_item=0;
					for(int qi=0;qi<lastitem;qi++)
						if(ts->a[qact].inv_change_s[q]*(-100)+ts->a[qact].inv_change[q]==inv[qi]) q_item=qi;
					if(debug)printf("qitem %d\n",q_item);
					for(int qi=q_item;qi<lastitem-1;qi++)
						inv[qi]=inv[qi+1];
					inv[lastitem-1]=0;
					lastitem--;
					//remove from scene 0
					int flag=0;
					for(int qi=0;qi<s1[0].objnum;qi++){
						if(flag==1){
							s1[0].o[qi-1]=s1[0].o[qi];
							s1[0].o[qi-1].num--;
						}
						if(s1[0].o[qi].num == q_item + 1){ //num in inv scene obj equal to inv array item + 1
							if(debug)printf("remove %s\n",s1[0].o[qi].name);
							flag=1;
						}
					}
					s1[0].objnum--;
				}
				q++;
				for(int qi=0;qi<s1[0].objnum;qi++){
					if(debug)printf("%s, ",s1[0].o[qi].name);
				}
				if(debug)printf("\n");
			}
			//show result
			ts->a[qact].show_result(line,win);
//			printf("condition is: %d\n",ts->a[qact].state);
			return 1;
		}
		else{ //action blocked by conditions
			nevozmogn=2;
		//mvwaddstr2(win, *line, 0, "Похоже, что перед этим надо что-то сделать.");
		//*line+=1;
		//return 1;
		}
	} 
	else{ //action blocked by state
		nevozmogn=3;
		//mvwaddstr2(win, *line, 0, "В данный момент это сделать невозможно.");
		//*line+=1;
		//return 1;
	}
	} //no action found
} //loop over actions

if(!showed){ //could not find any action
	for(int q=0;q<ts->objnum;q++){
		char outstr[255]="Не получается ";
		if(strcmp2(ts->o[q].name_v,buffer[word])==0){ //suppose last word is object name
			for(int i=0;i<=word;i++){
				if(i==word) //suppose last word is object name
					strcat(outstr,ts->o[q].name_v);
				else strcat(outstr,buffer[i]);
				strcat(outstr," ");
			}

			//show this only if object name from input found in current scene
			wattrset(win, COLOR_PAIR(2));
			mvwaddstr2(win, *line, 0, outstr);	
			*line+=1;
			while(*line>winsize){
				scroll(win);
				*line-=1;
			}
			wattrset(win, COLOR_PAIR(1));
			nevozmogn=4;
		}
	}
}
return nevozmogn;
}

//interactive part of the game:
int parser::analyze(int *line, WINDOW *win){

if(echo_on==1){
	char outstr[255]="";
	for(int i=0;i<=word;i++){
		strcat(outstr,buffer[i]);
		strcat(outstr," ");
	}
	wattrset(win, COLOR_PAIR(2));
	mvwaddstr2(win, *line, 0, outstr);	
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
	wattrset(win, COLOR_PAIR(1));
}
//exit
if(strcmp2("выход",buffer[0])==0&&word==0){
	wattrset(win, COLOR_PAIR(3));
	mvwaddstr2(win, *line, 0, "Вы уверены? y/n");
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
	int end=0;
	while (!end){
			unsigned char c = wgetch(win);

			if(c=='y')
				end=1;
			if(c=='n')
				end=-1;
	}
	if (end==1) return 0;
	if (end==-1) return 1;
}
//look around
if(strcmp2("осмотреться",buffer[0])==0&&word==0){
	s1[curscene].show_discr(line, win);
	return 1;
}
//restart
if(strcmp2("сначала",buffer[0])==0&&word==0){
	wattrset(win, COLOR_PAIR(3));
	mvwaddstr2(win, *line, 0, "Вы уверены? y/n");
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
	int end=0;
	while (!end){
			unsigned char c = wgetch(win);

			if(c=='y')
				end=1;
			if(c=='n')
				end=-1;
	}
	if (end==-1) return 1;
	
	restart_game();
	s1[curscene].show_discr(line, win);

	return 1;
}
//save
if(strcmp2("сохраниться",buffer[0])==0&&word==0){
	save_game();
	wattrset(win, COLOR_PAIR(3));
	mvwaddstr2(win, *line, 0, "Игра сохранена");
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
	return 1;
}
//load
if(strcmp2("загрузиться",buffer[0])==0&&word==0){
	wattrset(win, COLOR_PAIR(3));
	if(load_game())	mvwaddstr2(win, *line, 0, "Загрузка прошла успешно");
	else mvwaddstr2(win, *line, 0, "Ошибка при загрузке игры");
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
	s1[curscene].show_discr(line, win);
	return 1;
}
//help
if(strcmp2("помощь",buffer[0])==0&&word==0){
	wattrset(win, COLOR_PAIR(3));
	char help[1000];
	strcpy(help, "Доступные команды интерпретатора: ");
//	mvwaddstr2(win, *line, 0, "Доступные команды интерпретатора: выход, помощь, осмотреться, осмотреть <объект>, инвентарь, поговорить с <человек>, взять <объект>, съесть <объект>,");

	int pos=0;
	for(int q2=0;q2<verbs-1;q2++){
		strcat(help, v[q2].name);
//		strcat(help, " ");
		pos+=strlen(v[q2].name);

		int what=0;
		for(int q3=0;q3<v[q2].words;q3++){ //magic
			char tmp[255];
			strcat(help, " ");

			if(strcmp(v[q2].magic[q3],"#")==0){
				if(strcmp(v[q2].name,"идти")==0){
					strcpy(tmp,"<север/юг/запад/восток>");
				}
				else if(what==0){
					if(v[q2].padeg==0) strcpy(tmp,"объект");
					if(v[q2].padeg==1) strcpy(tmp,"объектом");
					if(v[q2].padeg==2) strcpy(tmp,"объекту");
					if(v[q2].padeg==3) strcpy(tmp,"объект");
				}
				else{
					if(v[q2].padeg2==0) strcpy(tmp,"объект");
					if(v[q2].padeg2==1) strcpy(tmp,"объектом");
					if(v[q2].padeg2==2) strcpy(tmp,"объекту");
					if(v[q2].padeg2==3) strcpy(tmp,"объект");
				}
				what++;
			}
			else strcpy(tmp,v[q2].magic[q3]);

			strcat(help, tmp);
			pos+=strlen(tmp)+1;
		}

		if(q2!=verbs-2){
			strcat(help, ", ");
			pos+=2;
		}
	}
	strcat(help, ".");
	pos++;

	int q2=0;
	for(int qq=0;qq<int(strlen(help));qq++){
		if(unsigned char(help[qq])==32&&perenos==1){ //space
				int eol=q2%80+find_next_space(help,qq);
				if(eol>79){
					q2+=79-q2%80;
					*line+=1;
					while(*line>winsize){
						scroll(win);
						*line-=1;
					}
				}
		}
		mvwaddch(win, *line/*+q2/80*/, q2%80, unsigned char(help[qq]));
		q2++;
		if(q2%80==0&&perenos==0){
			*line+=1;
			while(*line>winsize){
				scroll(win);
				*line-=1;
			}
		}
	}
	*line+=1;

/*	mvwaddstr2(win, *line, 0, help);

	*line+=pos/80+1;

	while(*line>20){
		scroll(win);
		*line-=1;
	}
*/	
/*	mvwaddstr2(win, *line, 0, "совместить <объект> и <объект>, обнюхать <объект>, одеть <объект>, снять <объект>, ощупать <объект>, открыть <объект>, закрыть <объект>, отдать <объект>");
	*line+=2;
	if(*line>20){
		scroll(win);
		scroll(win);
		*line-=2;
	}
	mvwaddstr2(win, *line, 0,  "<человеку>, прочитать <объект>, идти на <восток/запад/север/юг>, нажать на <объект>");
	*line+=1;
	if(*line>20){
		scroll(win);
		*line-=1;
	}
*/
	char tmpstr[20],tmpstr2[20]; //show score
			strcpy(tmpstr2,"Ваш счет: ");
			itoa(game_score,tmpstr,10);
			strcat(tmpstr2,tmpstr);
			mvwaddstr2(win, *line, 0, tmpstr2);
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}

	return 1;
}
//inventory
if(strcmp2("инвентарь",buffer[0])==0&&word==0){
	mvwaddstr2(win, *line, 0, "Ваш инвентарь: ");
	int q=0,qtrue;
	int pos=15;
	while(inv[q]!=0){
		for(int q2=0;q2<s1[inv[q]/100].objnum;q2++){
			if(s1[inv[q]/100].o[q2].num==inv[q]%100) qtrue=q2;
		}
		if(debug)printf("!!! %d %d\n",inv[q],qtrue);

		if(q>0){
			mvwaddstr2(win, *line, pos, ", ");
			pos+=2;
		}
		wattrset(win, COLOR_PAIR(2));
		mvwaddstr2(win, *line, pos, s1[inv[q]/100].o[qtrue].name);
		wattrset(win, COLOR_PAIR(1));
		pos+=strlen(s1[inv[q]/100].o[qtrue].name);
		q++;
	}
	if(q==0){
		mvwaddstr2(win, *line, pos, "пусто");
		pos+=5;
	}
	mvwaddstr2(win, *line, pos, ".");
	pos++;
	*line+=pos/80+1;
	return 1;
}

//look at
for(int q=0;q<s1[curscene].objnum;q++){
	if(strcmp2("осмотреть",buffer[0])==0&&strcmp2(s1[curscene].o[q].name_v,buffer[1])==0&&word==1){
		s1[curscene].o[q].show_discr(line, win, tab, perenos);
	//	mvwaddstr(win, *line, 1, "yes");
		return 1;
	}
	//mvwaddstr(win, *line, 1, s1.o[1].name);
	//*line++;
}
for(int q=0;q<s1[0].objnum;q++){
	if(strcmp2("осмотреть",buffer[0])==0&&strcmp2(s1[0].o[q].name_v,buffer[1])==0&&word==1){
		s1[0].o[q].show_discr(line, win, tab, perenos);
	//	mvwaddstr(win, *line, 1, "yes");
		return 1;
	}
	//mvwaddstr(win, *line, 1, s1.o[1].name);
	//*line++;
}


//go to
if(word==2)
if(strcmp2("идти",buffer[0])==0&&strcmp(buffer[1],"на")==0){
	int do_exit=1;
	int old_curscene=curscene;
	if(strcmp2("восток",buffer[2])==0&&s1[curscene].exits[0]!=-1) curscene=s1[curscene].exits[0];
	else if(strcmp2("запад",buffer[2])==0&&s1[curscene].exits[1]!=-1) curscene=s1[curscene].exits[1];
	else if(strcmp2("север",buffer[2])==0&&s1[curscene].exits[2]!=-1) curscene=s1[curscene].exits[2];
	else if(strcmp2("юг",buffer[2])==0&&s1[curscene].exits[3]!=-1) curscene=s1[curscene].exits[3];
	else do_exit=0;
	if(do_exit){
		wattrset(win, COLOR_PAIR(3));
		mvwaddstr2(win, *line, 0,  "Вы успешно перешли на локацию ");
		mvwaddstr2(win, *line, 30,  s1[curscene].name);
	//save game? - outdated
	/*if(curscene==10&&old_curscene==9){
		fstream savegame;
		savegame.open("save.dat",ios::out);
		savegame<<"sordid_is_here"<<endl;
		savegame.close();
	}*/
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
	return 1;
	}
	if(!do_exit){
		wattrset(win, COLOR_PAIR(3));
		mvwaddstr2(win, *line, 0,  "Туда пройти нельзя.");
		*line+=1;
		while(*line>winsize){
			scroll(win);
			*line-=1;
		}
		return 1;
	}
}

//testing actions of current and global scenes
int nevozmogn=parser::test_scene(&s1[0],line,win); //global
int nevozmogn2=0;
if(nevozmogn==3) *line-=1;
if(nevozmogn==0||nevozmogn==3||nevozmogn==4) nevozmogn2=parser::test_scene(&s1[curscene],line,win); //current
if(debug)printf("nevozmogn: %d, %d\n",nevozmogn,nevozmogn2);

wattrset(win, COLOR_PAIR(3));
if(nevozmogn==-1||nevozmogn2==-1){ //exit or death 
	wattrset(win, COLOR_PAIR(3));
	mvwaddstr2(win, *line, 0, "Начать сначала? y/n");
	*line+=1;
	while(*line>winsize){
		scroll(win);
		*line-=1;
	}
	int end=0;
	while (!end){
			unsigned char c = wgetch(win);

			if(c=='y')
				end=1;
			if(c=='n')
				end=-1;
	}
	if (end==-1) return 0;
	
	restart_game();
	s1[curscene].show_discr(line, win);

	return 1;
}
if((nevozmogn==3&&nevozmogn2==0)||(nevozmogn==0&&nevozmogn2==3)){ //blocked by state
mvwaddstr2(win, *line, 0, "В данный момент это сделать невозможно.");
*line+=1;
return 1;
}
if(nevozmogn==2||nevozmogn2==2){ //blocked by conditions
mvwaddstr2(win, *line, 0, "Похоже, что перед этим надо что-то сделать.");
*line+=1;
return 1;
}
if(nevozmogn==4||nevozmogn2==4){ //found object only
	//outputed already in test_scene()
}
if(nevozmogn==0&&nevozmogn2==0&&dunno_f==1){ //don't understand input string
		mvwaddstr2(win, *line, 0, dunno);
		*line+=1;
}

return 1;
}
