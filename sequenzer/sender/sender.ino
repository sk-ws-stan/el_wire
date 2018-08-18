#include <AudioAnalyzer.h>
#include <EL_Escudo.h>

//Declare Spectrum Shield pin connections
#define STROBE 4
#define RESET 6
#define DC_One A0
#define DC_Two A1 

//Define spectrum variables
int freq_amp;
int Frequencies_One[7];
int Frequencies_Two[7]; 
int i;

//Analyzer Audio = Analyzer( 11, 12, 5 );//Strobe pin ->4  RST pin ->5 Analog Pin ->5
//Analyzer Audio = Analyzer();//Strobe->4 RST->5 Analog->5


// array to hold read audio levels
int m_freqVal[7];
// potentiometer output value [0,1023]
unsigned int m_potValue = 0U;
// debug flags
const boolean m_potDebug = true;
const boolean m_sequencerDebug = true;
// serial boud rate: 9600 seems to work best
const unsigned int c_boudRate = 9600U;
// frequenzy bands used (<=7)
const unsigned int c_frequencyBands = 7U;
// potentiometer analog read channel
const unsigned int c_potentionMeterInput = 3U;
// noise gate value for frequency levels
const unsigned int c_noiseGate = 100U;
// divider for potentiometer read value
const unsigned int c_potDevider = 1U;

void setup()
{
  // init the baudrate
  Serial.begin( c_boudRate );

  //Set spectrum Shield pin configurations
  pinMode( STROBE, OUTPUT );
  pinMode( RESET, OUTPUT );
  pinMode( DC_One, INPUT );
  pinMode( DC_Two, INPUT );
  digitalWrite( STROBE, HIGH );
  digitalWrite( RESET, HIGH );
  
  //Initialize Spectrum Analyzers
  digitalWrite( STROBE, LOW );
  delay( 1);
  digitalWrite( RESET, HIGH );
  delay( 1);
  digitalWrite( STROBE, HIGH );
  delay( 1);
  digitalWrite( STROBE, LOW );
  delay( 1);
  digitalWrite( RESET, LOW );

  // init audio analyzer  
  //Audio.Init();
}

void loop()
{
  ReadFrequencies();
  delay(50);
  
  //return 7 values of 7 bands pass filter
    //Frequency(Hz):63  160  400  1K  2.5K  6.25K  16K
    //FreqVal[]:    0    1    2    3    4    5    6
//    //devide pot value for coarser resolution / easier adjustment
    //m_potValue = max( ( analogRead( c_potentionMeterInput ) / c_potDevider ), 0U );

    //const unsigned int freq = ( m_freqVal[ x ] > c_noiseGate ) ? ( m_freqVal[ x ] - c_noiseGate ) : 0U;
    if( m_sequencerDebug )
    {
        DebugPrintFrequencies();
    }
          
    //debug output for potentiometer
    //if( m_potDebug )
    //{
    //    Serial.print( m_potValue );
    //    Serial.println();
    //}
}

void ReadFrequencies()
{
  //Read frequencies for each band
  for( freq_amp = 0; freq_amp < 7; freq_amp++ )
  {
    Frequencies_One[ freq_amp ] = analogRead( DC_One );
    Frequencies_Two[ freq_amp ] = analogRead( DC_Two ); 
    digitalWrite(STROBE, HIGH);
    digitalWrite(STROBE, LOW);
  }
}

void DebugPrintFrequencies()
{
    for( i = 0; i < 7; i++ )
    {
        int value = 0;
        
        if( Frequencies_Two[ i ] > Frequencies_One[ i ] )
        {
            value = Frequencies_Two[ i ];
        }
        else
        {
            value = Frequencies_One[ i ];
        }
        Serial.print( value );
        Serial.print( ' ' );
   }
   Serial.println();
}
