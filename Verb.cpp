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

int verb::decl(){
std::fstream f;
char t[255];
int nnum=0, exit=0;
f.open("verbs.dat",std::ios::in);
if(!f.is_open()) return 0;

while(!f.eof()&&!exit){
	f>>t;
	if(strcmp(t,"&")==0) nnum++;
	if(nnum==verb::num+1) exit=1;
}
if(f.eof()) return 0;

f>>verb::name;
f>>verb::words>>verb::what>>verb::padeg>>verb::padeg2>>t;
for(int i=0;i<verb::words;i++)
	f>>verb::magic[i];

if(f.eof()) return 0;

return 1;
}
