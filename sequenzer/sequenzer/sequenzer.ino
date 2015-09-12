#include <AudioAnalyzer.h>
#include <EL_Escudo.h>

Analyzer Audio = Analyzer(4,5,5);//Strobe pin ->4  RST pin ->5 Analog Pin ->5
//Analyzer Audio = Analyzer();//Strobe->4 RST->5 Analog->5

int FreqVal[7];

void setup()
{
  Serial.begin(57600);  //Init the baudrate
  Audio.Init();//Init module 
}


