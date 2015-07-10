#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal.h>

#define Enc_A_PIN 2    // This genetates the interrupt, providing the click
#define Enc_B_PIN 10    // This is providing the direction 
#define Enc_PRESS_PIN 3

#define LONGPRESS_TIMEOUT 100000
char EncoderCnt = 0;              
char EncoderPressCnt = 0;
unsigned char ShortPressCnt = 0;
unsigned char LongPressCnt = 0;


// Using the nLCD library from https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // in 4 bit interface mode
// Creat a set of new characters
byte smiley[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte armsUp[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};

byte frownie[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001
};

/* for conntrastcontrol 
#define   CONTRAST_PIN   12
#define   CONTRAST       110

  // Set PWM for contrast levels
  pinMode(CONTRAST_PIN, OUTPUT);
  analogWrite ( CONTRAST_PIN, CONTRAST );
/*
  This ISR handles the debounce and reading of a quadrature encoder knob and increments or decrements  two global encoder count  variables
  depending on the direction of the encoder and the state of the button. The encoder functions at 1/4 the maximal possible resolution and generally provides one inc/dec
  per mechanical detent.

  One of the quadrature inputs is used to trigger this interupt handler, which checks the other quadrature input to decide the direction.
  A dead time after an initial deection is used to block out bounces.  It is assumed that the dirction qaudrature signal is not bouncing while
  the first phase is causing the initial interrupt as it's signal is 90deg opposed.

  Two encoder count variables are beeing modified by this ISR
     A: EncoderCnt when the knob is turned whithot the button being press
     B: EncoderPressCnt when the knob is  turned while the button is also pressed. This happens only after a LONG press timeout
  */

void
ISR_0( void)
{
  if ( digitalRead( Enc_PRESS_PIN ) )
  {
    if ( digitalRead( Enc_B_PIN ) )
      EncoderCnt++;
    else
      EncoderCnt--;

  }
  else
  {
    if ( digitalRead( Enc_B_PIN ) )
      EncoderPressCnt++;
    else
      EncoderPressCnt--;
  }

}

/*
  ISR to handle the button press interrupt. Two modes of button presses are recognized. A short, momentary press and a long, timing out press.
   While the hardware debounced button signal is sampled for up to TIMEOUT time in this ISR no other code is beeing executed. If the timeout occurs
   a long button press-, otherwise a short button press is registered. Timing has to be done by a software counter since interrupts are disabled and function millis() and micros()
   don't advance in time. Software timing is of course CPU clock dependent an d therefore has to be adjusted to the clock frequency if it is not running at 16Mhz.
*/
void
ISR_1(void)
{
  volatile unsigned long t = 0;
  while ( !digitalRead( Enc_PRESS_PIN ))
  {
    if (t++ > LONGPRESS_TIMEOUT)
    {
      LongPressCnt++;
      return;
    }
  }
  ShortPressCnt++;
}

void setup() {
  // Setup the Encoder pins to be inputs with pullups
  pinMode(Enc_A_PIN, INPUT);    // Use external 10K pullup and 100nf to gnd for debounce
  pinMode(Enc_B_PIN, INPUT);    // Use external 10K pullup and 100nf to gnd for debounce
  pinMode(Enc_PRESS_PIN, INPUT);// Use external 10K pullup and 100nf to gnd for debounce

  pinMode(13, OUTPUT);      // the LED

  lcd.begin(20, 4);              // initialize the lcd coloums and rows
  lcd.createChar (0, smiley);    // load character to the LCD
  lcd.createChar (1, armsUp);    // load character to the LCD
  lcd.createChar (2, frownie);   // load character to the LCD

  
  lcd.home ();                   // go home
  lcd.print("Hello, ARDUINO ");
  lcd.print( char(1));    // print character 1
   lcd.print( char(0));    // print character 0


  attachInterrupt(0, ISR_0, FALLING);
  attachInterrupt(1, ISR_1, FALLING);
/*  
  Serial.begin(57600);
  Serial.print("Rotary Knobtest\n");
*/


}

void loop()
{
  static short knob1 = 99;
  static short knob2 = 99;
  static short sp=99, lp=99;

  if (knob1 != EncoderCnt )
  {
    knob1 = EncoderCnt;
    /*
    Serial.print("Knob1 = ");
    Serial.println(knob1);
    */
    lcd.setCursor ( 0, 1 );
    lcd.print( "Knob1=");
    lcd.print( knob1);
    lcd.print(" ");


  }

  if (knob2 != EncoderPressCnt )
  {
    knob2 = EncoderPressCnt;
    /*
    Serial.print("Knob2 = ");
    Serial.println(knob2);
    */
    lcd.setCursor ( 10, 1 );
    lcd.print( "Knob2=");
    lcd.print( knob2);
    lcd.print(" ");


  }

  if (sp != ShortPressCnt)
  {
    sp = ShortPressCnt;
    /*
    Serial.print("ShortPress = ");
    Serial.println(ShortPressCnt);
    */
    lcd.setCursor ( 0, 2 );
    lcd.print( "Short=");
    lcd.print( ShortPressCnt);

  }

  if (lp != LongPressCnt )
  {
    lp = LongPressCnt;
    /*
    Serial.print("LongPress = ");
    Serial.println(LongPressCnt);
    */
    EncoderCnt = 0;
    lcd.setCursor ( 10, 2 );
    lcd.print( "Long=");
    lcd.print( LongPressCnt);

  }



}
