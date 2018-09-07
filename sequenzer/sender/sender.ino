#include <Wire.h>

//LCD display
#include <LiquidCrystal_I2C.h>
#include <I2CIO.h>
#include <LCD.h>

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
//button
#define BUTTON 2

// Set up nRF24L01 radio on SPI bus plus pins 8 & 9
RF24 radio( 9, 10 );
// Topology
const boolean c_autoAck = false;
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t c_pipes[ 2 ] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//LCD Display
LiquidCrystal_I2C lcd( 0x27, 2, 1, 0, 4, 5, 6, 7 );

// debug flags
const boolean c_potDebug = false;
const boolean c_sequencerDebug = false;
const boolean c_debugRadio = false;
// serial boud rate: 57600 seems to work best
const unsigned int c_boudRate = 57600U;
// frequenzy bands used (<=7)
const unsigned int c_frequencyBands = 7U;
//Define spectrum variables
//int freq_amp;
int Frequencies_One[ c_frequencyBands ];
int Frequencies_Two[ c_frequencyBands ];
//int i;
int m_freqVal[ c_frequencyBands ];
// filtering
const boolean c_filtering = true;
const unsigned int c_sampleShift = 1U;

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
Freqs smoothed;

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
  delay( 1 );
  digitalWrite( RESET, HIGH );
  delay( 1 );
  digitalWrite( STROBE, HIGH );
  delay( 1 );
  digitalWrite( STROBE, LOW );
  delay( 1 );
  digitalWrite( RESET, LOW );

  // init the baudrate
  Serial.begin( c_boudRate );

  printf_begin();
  // Setup and configure rf radio
  radio.begin();
  //radio.setDataRate(RF24_250KBPS);
  //radio.enableDynamicPayloads();
  radio.setAutoAck( c_autoAck );
  // optionally, increase the delay between retries & # of retries
  radio.setRetries( 15,15 );

  radio.openWritingPipe( c_pipes[0] );
  radio.openReadingPipe( 1, c_pipes[1] );
  // Start listening
  radio.startListening();
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
  
  lcd.setBacklightPin( 3, POSITIVE );
  lcd.setBacklight( LOW ); // NOTE: You can turn the backlight off by setting it to LOW instead of HIGH
  lcd.begin( 16, 2 );
  lcd.clear();
}

void loop()
{
    delay(20);  
    //return 7 values of 7 bands pass filter
    ReadFrequencies();
    //Frequency(Hz):63  160  400  1K  2.5K  6.25K  16K
    //FreqVal[]:    0    1    2    3    4    5    6
    if( c_sequencerDebug )
    {
        DebugPrintFrequencies();
    }
    if( c_filtering )
    {
        ExponentialAverageFreq();
    }
    else
    {
        ArrayToFreqs();
    }

    //DummyToFreqs();
    SendValues();
}


void PrintFreqOnLCD()
{
  lcd.setCursor( 0, 0 );
  lcd.print( ' ' );
  lcd.print( m_freqs.freq0 );
  lcd.print( ' ' );
  lcd.print( m_freqs.freq1 );
  lcd.print( ' ' );
  lcd.print( m_freqs.freq2 );
  lcd.print( ' ' );
  lcd.print( m_freqs.freq3 );
  lcd.setCursor( 0, 1 );
  lcd.print( ' ' );
  lcd.print( m_freqs.freq4 );
  lcd.print( ' ' );
  lcd.print( m_freqs.freq5 );
  lcd.print( ' ' );
  lcd.print( m_freqs.freq6 );
}

void ExponentialAverageFreq()
{
  smoothed.freq0 = smoothed.freq0 - m_freqs.freq0 + m_freqVal[ 0 ];
  m_freqs.freq0 = smoothed.freq0 >> c_sampleShift;
  smoothed.freq1 = smoothed.freq1 - m_freqs.freq1 + m_freqVal[ 1 ];
  m_freqs.freq1 = smoothed.freq1 >> c_sampleShift;
  smoothed.freq2 = smoothed.freq2 - m_freqs.freq2 + m_freqVal[ 2 ];
  m_freqs.freq2 = smoothed.freq2 >> c_sampleShift;
  smoothed.freq3 = smoothed.freq3 - m_freqs.freq3 + m_freqVal[ 3 ];
  m_freqs.freq3 = smoothed.freq3 >> c_sampleShift;
  smoothed.freq4 = smoothed.freq4 - m_freqs.freq4 + m_freqVal[ 4 ];
  m_freqs.freq4 = smoothed.freq4 >> c_sampleShift;
  smoothed.freq5 = smoothed.freq5 - m_freqs.freq5 + m_freqVal[ 5 ];
  m_freqs.freq5 = smoothed.freq5 >> c_sampleShift;
  smoothed.freq6 = smoothed.freq6 - m_freqs.freq6 + m_freqVal[ 6 ];
  m_freqs.freq6 = smoothed.freq6 >> c_sampleShift;
}

void ArrayToFreqs()
{
  m_freqs.freq0 = m_freqVal[ 0 ];
  m_freqs.freq1 = m_freqVal[ 1 ];
  m_freqs.freq2 = m_freqVal[ 2 ];
  m_freqs.freq3 = m_freqVal[ 3 ];
  m_freqs.freq4 = m_freqVal[ 4 ];
  m_freqs.freq5 = m_freqVal[ 5 ];
  m_freqs.freq6 = m_freqVal[ 6 ];
}

//void DummyToFreqs()
//{
//  m_freqs.freq0 = 256;
//  m_freqs.freq1 = 333;
//  m_freqs.freq2 = 444;
//  m_freqs.freq3 = 111;
//  m_freqs.freq4 = 666;
//  m_freqs.freq5 = 567;
//  m_freqs.freq6 = 321;
//}

void SendValues()
{
    radio.stopListening();
    // Take the time, and send it.  This will block until complete
    bool ok = radio.write( &m_freqs, sizeof( m_freqs ) );

    if( c_debugRadio )
    {
        if( ok )
        {
            printf("=======================  ok...\n\r");
        }
        else
        {
            printf("failed=========================.\n\r");
        }
    }
    radio.startListening();
}

void ReadFrequencies()
{
  //Read frequencies for each band
  for( int i = 0; i < c_frequencyBands; i++ )
  {
    Frequencies_One[ i ] = analogRead( DC_One );
    Frequencies_Two[ i ] = analogRead( DC_Two ); 
    digitalWrite( STROBE, HIGH );
    digitalWrite( STROBE, LOW );
    
    //for now take the higher channel
    if( Frequencies_Two[ i ] > Frequencies_One[ i ] )
    {
        m_freqVal[ i ] = Frequencies_Two[ i ];
    }
    else
    {
        m_freqVal[ i ] = Frequencies_One[ i ];
    }
  }
}

void DebugPrintFrequencies()
{
    for( int i = 0; i < c_frequencyBands; i++ )
    {
        Serial.print( m_freqVal[ i ] );
        Serial.print( ' ' );
   }
   Serial.println();
}
