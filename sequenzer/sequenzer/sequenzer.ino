#include <AudioAnalyzer.h>
#include <EL_Escudo.h>

Analyzer Audio = Analyzer(12,5,5);//Strobe pin ->4  RST pin ->5 Analog Pin ->5
//Analyzer Audio = Analyzer();//Strobe->4 RST->5 Analog->5

int FreqVal[7];
int m_counter = 50;
const unsigned int c_frequencyBands = 7U;

void setup()
{
  Serial.begin(57600);  //Init the baudrate
  Audio.Init();//Init module 
  // The EL channels are on pins 2 through 9
  // Initialize the pins as outputs
  pinMode( 2, OUTPUT );  // channel A
  pinMode( 3, OUTPUT );  // channel B
  pinMode( 4, OUTPUT );  // channel C
  pinMode( 5, OUTPUT );  // channel D
  pinMode( 6, OUTPUT );  // channel E
  pinMode( 7, OUTPUT );  // channel F
  pinMode( 8, OUTPUT );  // channel G
  pinMode( 9, OUTPUT );  // channel H
  // We also have two status LEDs, pin 10 on the Escudo,
  // and pin 13 on the Arduino itself
  pinMode( 10, OUTPUT );
  pinMode( 13, OUTPUT );
}

void LightWires()
{
    unsigned int x;
    for( x = 0U; x < c_frequencyBands; x++ )
    {
        const unsigned int wire = x + 2U;// wires start from pin 2 - there was a mapping function once...
        const unsigned int freq = max( ( FreqVal[ x ] - 100 ), 0 );

        if( freq > 0 )
        {
            digitalWrite( wire, HIGH );
        }
        else
        {
            digitalWrite( wire, LOW );
        }
    }
}

void loop()
{
  Audio.ReadFreq(FreqVal);//return 7 value of 7 bands pass filiter 
                          //Frequency(Hz):63  160  400  1K  2.5K  6.25K  16K
                          //FreqVal[]:      0    1    2    3    4    5    6  
  LightWires();
  delay( 20 );
}

