#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include "printf.h"

#include <EL_Escudo.h>

RF24 radio(9,10);

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
role_e role = role_pong_back;

// array to hold read audio levels
int m_freqVal[7];
// potentiometer output value [0,1023]
unsigned int m_potValue = 0U;
// debug flags
const boolean c_potDebug = false;
const boolean c_radioDebug = true;
const boolean c_sequencerDebug = false;
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
  // init the baudrate
  Serial.begin( c_boudRate );

  printf_begin();
  // Setup and configure rf radio
  radio.begin();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);
  // Open reading pipes
  radio.openReadingPipe(1,pipes[0]);
  // Start listening
  radio.startListening();
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();

  // init EL_Escudo's EL channels: digital pins 2 through 9
  pinMode( 2, OUTPUT );  // channel A
  pinMode( 3, OUTPUT );  // channel B
  pinMode( 4, OUTPUT );  // channel C
  pinMode( 5, OUTPUT );  // channel D
  pinMode( 6, OUTPUT );  // channel E
  pinMode( 7, OUTPUT );  // channel F
  pinMode( 8, OUTPUT );  // channel G
//  pinMode( 9, OUTPUT );  // channel H
  // init status LEDs: pin 10 on EL_Escudo and pin 13 on Arduino
  //pinMode( 10, OUTPUT );
  //pinMode( 13, OUTPUT );
}

void LightWires()
{
    //go through all frequency bands
    for( unsigned int x = 0U; x < c_frequencyBands; x++ )
    {
        //wires start from digital pin 2
        const unsigned int wire = x + 2U;
        //apply noise gate to read value - avoid negative 'roll-over' with unary assignment
        const unsigned int freq = ( m_freqVal[ x ] > ( c_noiseGate / wire ) ) ? ( m_freqVal[ x ] - ( c_noiseGate / wire ) ) : 0U;
        //light up channel if exceeds pot
        if( freq > m_potValue )
        {
            digitalWrite( wire, HIGH );
        }
        else
        {
            digitalWrite( wire, LOW );
        }
        //debug output for normalized frequencies
        if( c_sequencerDebug )
        {
            Serial.print( freq ); Serial.print( " " );
        }
    }
    if( c_sequencerDebug )
    {
        Serial.println();
    }
}

void loop()
{
    //return 7 values of 7 bands pass filter
    //Frequency(Hz):63  160  400  1K  2.5K  6.25K  16K
    //FreqVal[]:    0    1    2    3    4    5    6
    delay(20);
    //read wifi here
    ReadRadio();
    
    //devide pot value for coarser resolution / easier adjustment
    //disable this for now as it isn't connected
    //m_potValue = max( ( analogRead( c_potentionMeterInput ) / c_potDevider ), 0U );
    m_potValue = 100U;    
    //debug output for potentiometer
    if( c_potDebug )
    {
        Serial.print( m_potValue );
        Serial.println();
    }

    LightWires();
}

void ReadRadio()
{
    if( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      bool done = false;
      while( !done )
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &m_freqVal, sizeof(m_freqVal) );
      }
      if( c_radioDebug )
      {
        Serial.print( "==========" );
        if( done )
        {
          Serial.print( "ok" );
        }
        else
        {
          Serial.print( "meeeehh" );
        }
        Serial.print( "==========" );
        Serial.println();

        for( unsigned int x = 0U; x < c_frequencyBands; x++ )
        {
            Serial.print( m_freqVal[ x ] ); Serial.print( " " );
        }
        Serial.println();
      }
    }
}
