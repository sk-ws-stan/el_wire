// wifi uses SPI
#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
//custom printf
#include "printf.h"
//Declare Spectrum Shield pin connections
#define STROBE 4
#define RESET 6
#define DC_One A0
#define DC_Two A1 

//Define spectrum variables
int freq_amp;
int Frequencies_One[ 7 ];
int Frequencies_Two[ 7 ]; 
int i;

// Set up nRF24L01 radio on SPI bus plus pins 8 & 9
RF24 radio( 9, 10 );


//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_ping_out;

//old stuff and modifiers
// array to hold read audio levels
int m_freqVal[7];
// potentiometer output value [0,1023]
unsigned int m_potValue = 0U;
// debug flags
const boolean m_potDebug = true;
const boolean m_sequencerDebug = true;
// serial boud rate: 9600 seems to work best
//const unsigned int c_boudRate = 9600U;
const unsigned int c_boudRate = 57600U;
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

  // init the baudrate
  Serial.begin( c_boudRate );

  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(20,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  if( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }

  //
  // Start listening
  //

    // First, stop listening so we can talk.
    radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

void loop()
{
  ReadFrequencies();

  delay(100);
  
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

    radio.stopListening();
    // Take the time, and send it.  This will block until complete
    bool ok = radio.write( &m_freqVal, sizeof(m_freqVal) );
    
    if (ok)
      printf("=======================  ok...\n\r");
    else
      printf("failed=========================.\n\r");

    radio.startListening();
    
    
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
        if( Frequencies_Two[ i ] > Frequencies_One[ i ] )
        {
            m_freqVal[ i ] = Frequencies_Two[ i ];
        }
        else
        {
            m_freqVal[ i ] = Frequencies_One[ i ];
        }
        Serial.print( m_freqVal[ i ] );
        Serial.print( ' ' );
   }
   Serial.println();
}
