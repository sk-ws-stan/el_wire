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
// Topology
//
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// array to hold read audio levels
int m_freqVal[7];
// debug flags
const boolean m_potDebug = true;
const boolean m_sequencerDebug = true;
// serial boud rate: 57600 seems to work best
const unsigned int c_boudRate = 57600U;
// frequenzy bands used (<=7)
const unsigned int c_frequencyBands = 7U;

typedef struct
{
  int freq0;
  int freq1;
  int freq2;
  int freq3;
  int freq4;
  int freq5;
  int freq6;
} Freqs;

Freqs m_freqs;

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
  // Setup and configure rf radio
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  //radio.enableDynamicPayloads();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(20,15);

  radio.openWritingPipe( pipes[0] );
  radio.openReadingPipe( 1, pipes[1] );
  // Start listening
  radio.startListening();
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
}

void loop()
{
    delay(100);  
    //return 7 values of 7 bands pass filter
    ReadFrequencies();
    //Frequency(Hz):63  160  400  1K  2.5K  6.25K  16K
    //FreqVal[]:    0    1    2    3    4    5    6
    if( m_sequencerDebug )
    {
        DebugPrintFrequencies();
    }
    ArrayToFreqs();
    SendValues();
}

void ArrayToFreqs()
{
  m_freqs.freq0 = m_freqVal[0];
  m_freqs.freq1 = m_freqVal[1];
  m_freqs.freq2 = m_freqVal[2];
  m_freqs.freq3 = m_freqVal[3];
  m_freqs.freq4 = m_freqVal[4];
  m_freqs.freq5 = m_freqVal[5];
  m_freqs.freq6 = m_freqVal[6];
}

void SendValues()
{
    radio.stopListening();
    // Take the time, and send it.  This will block until complete
    bool ok = radio.write( &m_freqs, sizeof(m_freqs) );

    if( ok )
    {
      printf("=======================  ok...\n\r");
    }
    else
    {
      printf("failed=========================.\n\r");
    }
    radio.startListening();
}

void ReadFrequencies()
{
  //Read frequencies for each band
  for( freq_amp = 0; freq_amp < c_frequencyBands; freq_amp++ )
  {
    Frequencies_One[ freq_amp ] = analogRead( DC_One );
    Frequencies_Two[ freq_amp ] = analogRead( DC_Two ); 
    digitalWrite(STROBE, HIGH);
    digitalWrite(STROBE, LOW);
    //for now take the higher channel
    if( Frequencies_Two[ freq_amp ] > Frequencies_One[ freq_amp ] )
    {
        m_freqVal[ freq_amp ] = Frequencies_Two[ freq_amp ];
    }
    else
    {
        m_freqVal[ freq_amp ] = Frequencies_One[ freq_amp ];
    }
  }
}

void DebugPrintFrequencies()
{
    for( i = 0; i < 7; i++ )
    {
        Serial.print( m_freqVal[ i ] );
        Serial.print( ' ' );
   }
   Serial.println();
}
