#ifndef verb_cpp
#define verb_cpp

class verb{
//char verbs[22][20]={"выход","помощь","осмотреться","инвентарь","осмотреть","поговорить","взять","съесть",
//"совместить","обнюхать","надеть","снять","ощупать","открыть","закрыть","отдать","прочитать","идти","нажать"};
public:

int num; //id
char name[255]; //verb
int words; //number of words, i.e. 'exit' = 0, 'push on something' = 2
int what; //number of objects in action
int padeg; // 0 = imenitel'nyj (name), 1 = tvoritel'nyj (name_t), 2 = datel'nyj (name_d), 3 = vinitel'nyj (name_v)
int padeg2; //padeg for object 2

char magic[10][30]; //10 words 30 chars each for phrase after command

int decl();
};
#endif