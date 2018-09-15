#include <SPI.h>
#include <FastLED.h>

#define NUM_LEDS_1 43
#define DATA_PIN_1 3
#define CLOCK_PIN 2

CRGB m_ledsOne[ NUM_LEDS_1 ];

const unsigned int c_boudRate = 57600U;
int m_currentLedOne = 0;

void setup()
{
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);
  FastLED.addLeds< APA102, DATA_PIN_1, CLOCK_PIN, RGB >( m_ledsOne, NUM_LEDS_1 );
  Serial.begin( c_boudRate );
}

void fadeall_one()
{
  for( int i = 0; i < NUM_LEDS_1; i++ )
  {
    m_ledsOne[i].nscale8( 250 );
  }
}

void loop()
{
    int poti = analogRead( A3 );
    int brightness = map( poti, 0, 890, 50, 255 );
    Serial.print( poti ); Serial.print(" == "); Serial.print( brightness ); Serial.println();
    static uint8_t hue_one = 0;

    m_ledsOne[ m_currentLedOne++ ] = CHSV( hue_one++, 255, brightness );

    FastLED.show();

    if( hue_one >= 255 )
    {
      hue_one = 0;
    }
    delay(30);
    if( m_currentLedOne >= NUM_LEDS_1 )
    {
      m_currentLedOne = 0;
    }
    fadeall_one();
}
