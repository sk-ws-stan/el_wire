//radio
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <FastLED.h>

#define NUM_LEDS_1 106
#define DATA_PIN_1 3
#define CLOCK_PIN 2

CRGB m_ledsOne[ NUM_LEDS_1 ];

RF24 m_radio( 9, 10 );
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t c_pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// array to hold read audio levels
const boolean c_radioDebug = true;
const unsigned int c_boudRate = 57600U;
// frequenzy bands used (<=7)
const unsigned int c_frequencyBands = 7U;
int m_currentLedOne = 1;

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
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds< APA102, DATA_PIN_1, CLOCK_PIN, RGB >( m_ledsOne, NUM_LEDS_1 );
  Serial.begin( c_boudRate );

  printf_begin();
  // Setup and configure rf radio
  m_radio.begin();
  m_radio.enableDynamicPayloads();
  m_radio.enableDynamicAck();
  m_radio.setAutoAck( false );
  // Open reading pipes
  m_radio.openWritingPipe( c_pipes[1] );
  m_radio.openReadingPipe( 1, c_pipes[0] );
  // Start listening
  m_radio.startListening();
  // Dump the configuration of the rf unit for debugging
  m_radio.printDetails();
}

void fadeall_one()
{
  for( int i = 0; i < NUM_LEDS_1; i++ )
  {
    m_ledsOne[i].nscale8( 250 );
  }
}
void ReadRadio()
{  
    uint8_t pipe_num;

    if( m_radio.available( &pipe_num ) && pipe_num == 1 )
    {
      uint8_t len = m_radio.getDynamicPayloadSize();

      if( len == sizeof( m_freqs ) )
      {
        Serial.print( "matches" );Serial.println();
      }
      m_radio.read( &m_freqs, sizeof( m_freqs ) );

//      m_radio.stopListening();

      if( c_radioDebug )
      {
        Serial.print( m_freqs.freq0 ); Serial.print( " " );
        Serial.print( m_freqs.freq1 ); Serial.print( " " );
        Serial.print( m_freqs.freq2 ); Serial.print( " " );
        Serial.print( m_freqs.freq3 ); Serial.print( " " );
        Serial.print( m_freqs.freq4 ); Serial.print( " " );
        Serial.print( m_freqs.freq5 ); Serial.print( " " );
        Serial.print( m_freqs.freq6 ); Serial.print( " " );
        Serial.println();
      }
    }
 //   m_radio.startListening();
}

void loop()
{
    ReadRadio();

    static uint8_t hue_one = 0;

    m_ledsOne[ m_currentLedOne++ ] = CHSV( hue_one++, 255, 255 );

    FastLED.show();

    if( hue_one >= 255 )
    {
      hue_one = 0;
    }
    delay(10);
    if( m_currentLedOne >= NUM_LEDS_1 )
    {
      m_currentLedOne = 0;
    }
    fadeall_one();
}
