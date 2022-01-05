#ifndef parser_cpp
#define parser_cpp

#include "Scene.h"

class parser{
public:
char buffer[10][80]; //buffer for input string
int word; //number of words in the input string

parser::parser();
int read(int, WINDOW *);
int analyze(int *, WINDOW *);
int test_scene(scene *,int *, WINDOW *);
};
#endif