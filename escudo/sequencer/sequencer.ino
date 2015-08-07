#define STROBE 4
#define RESET 6
#define DC_ONE A0
#define DC_TWO A1

const unsigned int c_dimmingSteps = 2U;
const unsigned int c_maxAdjustLevel = 512U;
const unsigned int c_minAdjustLevel = 380U;
const unsigned int c_adjustStep = 5U;
const unsigned int c_maxTimerLimit = 20U;
const unsigned int c_frequencyBands = 7U;

unsigned int m_ampsLeft[ 7 ] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };
unsigned int m_ampsRight[ 7 ] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U };

unsigned int m_counter = 0U;
unsigned int m_maxLevel = 512U;
unsigned int m_maxTimer = 0U;

void setup()
{
    //EL channels are on pins 2 through 9 = output
    unsigned int i = 2U;
    const unsigned int channels = i + 8U;

    for( i; i < channels; i++ )
    {
        pinMode( i, OUTPUT );
        digitalWrite( i, LOW );
    }
    //Two status LEDs, pin 10 on the Escudo, pin 13 on the Arduino
    pinMode( 10, OUTPUT );
    pinMode( 13, OUTPUT );
    
    //Spectrum Shield pin configurations
    pinMode( STROBE, OUTPUT );
    pinMode( RESET, OUTPUT );
    pinMode( DC_ONE, INPUT );
    pinMode( DC_TWO, INPUT );  
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
}

void ReadFrequencies()
{
    unsigned int freqAmp = 0U;
    //read frequencies for each band
    for( freqAmp; freqAmp < c_frequencyBands; freqAmp++ )
    {
        m_ampsLeft[ freqAmp ] = analogRead( DC_ONE );
        m_ampsRight[ freqAmp ] = analogRead( DC_TWO ); 
        digitalWrite( STROBE, HIGH );
        digitalWrite( STROBE, LOW );
    }
}

void AdjustMaxLevel( unsigned int current )
{
  if( current >= c_maxAdjustLevel )
  {
      m_maxLevel -= c_adjustStep;
      m_maxTimer = 0U;
  }
  else
  {
      if( current < c_minAdjustLevel )
      {
          if( m_maxLevel < c_minAdjustLevel )
          {
              if( m_maxTimer++ > c_maxTimerLimit )
              {
                  m_maxLevel += c_adjustStep;
                  m_maxTimer = 0U;
              }
          }
      }
      else
      {
          m_maxTimer = 0U; 
      }
  }
}

void NormalizeFrequencies()
{
    unsigned int freqAmp = 0U;
    unsigned int currentMax = 0U;

    for( freqAmp; freqAmp < c_frequencyBands; freqAmp++ )
    {
        unsigned int currentLeft = m_ampsLeft[ freqAmp ];
        unsigned int currentRight = m_ampsRight[ freqAmp ];

        if( max( currentLeft, currentRight ) > currentMax )
        {
            currentMax = max( currentLeft, currentRight );
        }
        currentLeft = constrain( currentLeft, 0, m_maxLevel );
        currentRight = constrain( currentRight, 0, m_maxLevel );
        m_ampsLeft[ freqAmp ] = map( currentLeft, 0, m_maxLevel, 0, c_dimmingSteps );
        m_ampsRight[ freqAmp ] = map( currentRight, 0, m_maxLevel, 0, c_dimmingSteps );
    }
    AdjustMaxLevel( currentMax );
}

void LightWires()
{
    unsigned int x;
    for( x = 0U; x < c_frequencyBands; x++ )
    {
        const unsigned int wire = x + 2U;// wires start from pin 2 - there was a mapping function once...
        const unsigned int avgAmp = ( m_ampsLeft[ x ] + m_ampsRight[ x ] ) / 2U;

        if( avgAmp > m_counter )
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
    //fill amplitudes here
    ReadFrequencies();
    //normalize to dimming steps
    NormalizeFrequencies();
    //check counter
    if( m_counter++ >= c_dimmingSteps )
    {
        m_counter = 0U;
    }
    //light
    LightWires();
}

