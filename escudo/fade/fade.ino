const int c_PulseWidth = 10;
const int c_Duration = 5;

void On( int channel )
{
    pinMode( channel, OUTPUT );
    digitalWrite( channel, HIGH );
};

void Off( int channel )
{
    pinMode( channel, INPUT );
};


void FadeIn( int channel )
{
    for( int brightness = 0; brightness <= c_PulseWidth; brightness++ )
    {
        for( int duration = 0; duration < c_Duration; duration++ )
        {
            On( channel );
            delay( brightness );
            Off( channel );
            delay( c_PulseWidth - brightness );
        }
    }
    On( channel );
};

void FadeOut( int channel )
{
    for( int brightness = c_PulseWidth; brightness >= 0; brightness-- )
    {
        for( int duration = 0; duration < c_Duration; duration++ )
        {
            On( channel );
            delay( brightness );
            Off( channel );
            delay( c_PulseWidth - brightness );
        }
    }
};

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
}

void loop()
{
    int x = 2;
    // Step through all eight EL channels (pins 2 through 9)
    for( x; x <= 8; x++ )
    {
      FadeIn( x + 1);
      FadeOut( x );
    }
    FadeIn( 2 );
    FadeOut( 9 );
}

