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

CRGB leds_one[ NUM_LEDS_1 ];
CRGB leds_two[ NUM_LEDS_2 ];
CRGB leds_three[ NUM_LEDS_3 ];
CRGB leds_four[ NUM_LEDS_4 ];
CLEDController *controllers[ NUM_STRIPS ];
uint8_t gBrightness = 128;

RF24 radio( 9, 10 );
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

const boolean c_radioDebug = true;
const unsigned int c_boudRate = 57600U;
// frequenzy bands used (<=7)
const unsigned int c_frequencyBands = 7U;
// noise gate value for frequency levels
const unsigned int c_noiseGate = 100U;
// divider for potentiometer read value
int currentLed_one = 0;
int currentLed_two = 0;
int currentLed_three = 0;
int currentLed_four = 0;

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
  controllers[0] = &FastLED.addLeds< APA102, DATA_PIN_1, CLOCK_PIN, RGB >( leds_one, NUM_LEDS_1 );
  controllers[1] = &FastLED.addLeds< APA102, DATA_PIN_2, CLOCK_PIN, RGB >( leds_two, NUM_LEDS_2 );
  controllers[2] = &FastLED.addLeds< APA102, DATA_PIN_3, CLOCK_PIN, RGB >( leds_three, NUM_LEDS_3 );
  controllers[3] = &FastLED.addLeds< APA102, DATA_PIN_4, CLOCK_PIN, RGB >( leds_four, NUM_LEDS_4 );
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
  radio.openWritingPipe( pipes[1] );
  radio.openReadingPipe( 1, pipes[0] );
  // Start listening
  //radio.enableDynamicPayloads();
  radio.setAutoAck( false );
  radio.startListening();
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
}

void fadeall_one()
{
  for( int i = 0; i < NUM_LEDS_1; i++ )
  {
    leds_one[i].nscale8(250);
  }
}

void fadeall_two()
{
  for( int i = 0; i < NUM_LEDS_2; i++ )
  {
    leds_two[i].nscale8(250);
  }
}

void fadeall_three()
{
  for( int i = 0; i < NUM_LEDS_3; i++ )
  {
    leds_three[i].nscale8(250);
  }
}

void fadeall_four()
{
  for( int i = 0; i < NUM_LEDS_4; i++ )
  {
    leds_four[i].nscale8(250);
  }
}
void ReadRadio()
{
    if( radio.available() )
    {
      //int test = -3;
      // Dump the payloads until we've gotten everything
      bool done = false;
      while( !done )
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &m_freqs, sizeof(m_freqs) );
        //done = radio.read( &test, sizeof(test) );
      }
      radio.stopListening();

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

        Serial.print( m_freqs.freq0 ); Serial.print( " " );
        Serial.print( m_freqs.freq1 ); Serial.print( " " );
        Serial.print( m_freqs.freq2 ); Serial.print( " " );
        Serial.print( m_freqs.freq3 ); Serial.print( " " );
        Serial.print( m_freqs.freq4 ); Serial.print( " " );
        Serial.print( m_freqs.freq5 ); Serial.print( " " );
        Serial.print( m_freqs.freq6 ); Serial.print( " " );

        //Serial.print( test );
        Serial.println();
      }
    }
    else
    {
        Serial.print( "meeeehh" );
    }

    radio.startListening();
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

 // for( int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1 )
  //{
    leds_one[ currentLed_one++ ] = CHSV( hue_one++, 255, 255 );
    leds_two[ currentLed_two++ ] = CHSV( hue_two++, 255, 255 );
    leds_three[ currentLed_three++ ] = CHSV( hue_three++, 255, 255 );
    leds_four[ currentLed_four++ ] = CHSV( hue_four++, 255, 255 );
//    leds_one[ currentLed_one++ ] = CRGB::White;
//    leds_two[ currentLed_two++ ] = CRGB::White;
//    leds_three[ currentLed_three++ ] = CRGB::White;
//    leds_four[ currentLed_four++ ] = CRGB::White;

   // FastLED.show();
    controllers[0]->showLeds(gBrightness);
    controllers[1]->showLeds(gBrightness);
    controllers[2]->showLeds(gBrightness);
    controllers[3]->showLeds(gBrightness);
    
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
    //currentLed;
    
    if( currentLed_one >= NUM_LEDS_1 )
    {
      currentLed_one = 0;
    }
    if( currentLed_two >= NUM_LEDS_2 )
    {
      currentLed_two = 0;
    }
    if( currentLed_three >= NUM_LEDS_3 )
    {
      currentLed_three = 0;
    }
    if( currentLed_four >= NUM_LEDS_4 )
    {
      currentLed_four = 0;
    }
    
    delay(10);
      
    fadeall_one();
    fadeall_two();
    fadeall_three();
    fadeall_four();
  //}
}
