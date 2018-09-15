// Use if you want to force the software SPI subsystem to be used for some reason (generally, you don't)
// #define FASTLED_FORCE_SOFTWARE_SPI
// Use if you want to force non-accelerated pin access (hint: you really don't, it breaks lots of things)
// #define FASTLED_FORCE_SOFTWARE_SPI
// #define FASTLED_FORCE_SOFTWARE_PINS
#include "FastLED.h"
// radio
#include <SPI.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include "printf.h"

#define NUM_LEDS_1 106
#define NUM_LEDS_2 106
#define NUM_LEDS_3 106
#define NUM_LEDS_4 106
#define DATA_PIN_1 3
#define DATA_PIN_2 4
#define DATA_PIN_3 5
#define DATA_PIN_4 6
#define CLOCK_PIN 2
#define NUM_STRIPS 4

CRGB m_ledsOne[ NUM_LEDS_1 ];
CRGB m_ledsTwo[ NUM_LEDS_2 ];
CRGB m_ledsThree[ NUM_LEDS_3 ];
CRGB m_ledsFour[ NUM_LEDS_4 ];
CLEDController *m_controllers[ NUM_STRIPS ];
uint8_t m_gBrightness = 128;

RF24 m_radio( 9, 10 );
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t c_pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const boolean c_radioDebug = true;
const unsigned int c_boudRate = 57600U;
// frequenzy bands used (<=7)
const unsigned int c_frequencyBands = 7U;

int m_currentLedOne = 0;
int m_currentLedTwo = 0;
int m_currentLedThree = 0;
int m_currentLedFour = 0;

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
  m_controllers[0] = &FastLED.addLeds< APA102, DATA_PIN_1, CLOCK_PIN, RGB >( m_ledsOne, NUM_LEDS_1 );
  m_controllers[1] = &FastLED.addLeds< APA102, DATA_PIN_2, CLOCK_PIN, RGB >( m_ledsTwo, NUM_LEDS_2 );
  m_controllers[2] = &FastLED.addLeds< APA102, DATA_PIN_3, CLOCK_PIN, RGB >( m_ledsThree, NUM_LEDS_3 );
  m_controllers[3] = &FastLED.addLeds< APA102, DATA_PIN_4, CLOCK_PIN, RGB >( m_ledsFour, NUM_LEDS_4 );
  // init the baudrate
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
    m_ledsOne[i].nscale8(250);
  }
}

void fadeall_two()
{
  for( int i = 0; i < NUM_LEDS_2; i++ )
  {
    m_ledsTwo[i].nscale8(250);
  }
}

void fadeall_three()
{
  for( int i = 0; i < NUM_LEDS_3; i++ )
  {
    m_ledsThree[i].nscale8(250);
  }
}

void fadeall_four()
{
  for( int i = 0; i < NUM_LEDS_4; i++ )
  {
    m_ledsFour[i].nscale8(250);
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

      //radio.stopListening();

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
    //radio.startListening();
}

void loop()
{
      //return 7 values of 7 bands pass filter
    //Frequency(Hz):63  160  400  1K  2.5K  6.25K  16K
    //FreqVal[]:    0    1    2    3    4    5    6
    //delay(20);
    //read wifi here
    ReadRadio();

  static uint8_t hue_one = 0;
  static uint8_t hue_two = 64;
  static uint8_t hue_three = 128;
  static uint8_t hue_four = 192;

    m_ledsOne[ m_currentLedOne++ ] = CHSV( hue_one++, 255, 255 );
    m_ledsTwo[ m_currentLedTwo++ ] = CHSV( hue_two++, 255, 255 );
    m_ledsThree[ m_currentLedThree++ ] = CHSV( hue_three++, 255, 255 );
    m_ledsFour[ m_currentLedFour++ ] = CHSV( hue_four++, 255, 255 );

   // FastLED.show();
    m_controllers[0]->showLeds( m_gBrightness );
    m_controllers[1]->showLeds( m_gBrightness );
    m_controllers[2]->showLeds( m_gBrightness );
    m_controllers[3]->showLeds( m_gBrightness );

    if( hue_one >= 255 )
    {
      hue_one = 0;
    }
    if( hue_two >= 255 )
    {
      hue_two = 0;
    }
    if( hue_three >= 255 )
    {
      hue_three = 0;
    }
    if( hue_four >= 255 )
    {
      hue_four = 0;
    }

    if( m_currentLedOne >= NUM_LEDS_1 )
    {
      m_currentLedOne = 0;
    }
    if( m_currentLedTwo >= NUM_LEDS_2 )
    {
      m_currentLedTwo = 0;
    }
    if( m_currentLedThree >= NUM_LEDS_3 )
    {
      m_currentLedThree = 0;
    }
    if( m_currentLedFour >= NUM_LEDS_4 )
    {
      m_currentLedFour = 0;
    }

    fadeall_one();
    fadeall_two();
    fadeall_three();
    fadeall_four();
}
