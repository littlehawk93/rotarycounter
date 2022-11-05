#include <TM1637.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define INTERRUPT_PIN PCINT3

#define CLK_PIN 0
#define DIO_PIN 1
#define INT_PIN 3
#define LED_PIN 4

#define LED_DURATION 500
#define LED_INTERVAL 5000
#define NUM_INTERVAL 17
#define SIG_INTERVAL 10

// timing variables
unsigned sigTime = 0;
unsigned long numTime = 0;
unsigned long currTime = 0;
unsigned long ledTime = 0;
byte ledState = LOW;

// Current rotations count
long count = 0;

TM1637 lcd(CLK_PIN, DIO_PIN);                                                                                                                                                                                                                                    

void setup() {

  // code for setting up the interrupt handler on ATTiny85
  cli();
  PCMSK |= (1 << INTERRUPT_PIN);
  GIMSK |= (1 << PCIE);
  pinMode(INT_PIN, INPUT);
  sei(); 

  pinMode(DIO_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);

  // Setup the status LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize the 7 segment LCD display
  lcd.init();
  lcd.set(2);
}

void loop() {

  currTime = millis();
  runLED();
  runNumbers();
  delay(2);
}

// Checks the current time and pulses the LED on and off (1/2 second on, 1 second off)
void runLED() {

  if(ledState == LOW && currTime - ledTime >= LED_INTERVAL) {
    ledState = HIGH;
    digitalWrite(LED_PIN, HIGH);
    ledTime = currTime;
  } else if (ledState == HIGH && currTime - ledTime >= LED_DURATION) {
    ledState = LOW;
    digitalWrite(LED_PIN, LOW);
    ledTime = currTime;
  }
}

// Check if enough time has passed since the last time numbers were re-drawn on the LCD screen
// If enough time has passed (17 ms or 60 fps), then draw the current count to the screen
void runNumbers() {

  if (currTime - numTime >= NUM_INTERVAL) {
    drawNumber();
    numTime = currTime;
  }
}

// Draw the current count onto the LED 7 segment display
void drawNumber() {

  lcd.display(3, count % 10);
  lcd.display(2, count / 10 % 10);
  lcd.display(1, count / 100 % 10);
  lcd.display(0, count / 1000 % 10);
}

// Interrupt handler function for when another rotation is detected. increments the counter if enough time has passed since the last rotation
ISR(PCINT0_vect)
{
  if( digitalRead(INT_PIN) == HIGH && currTime - sigTime >= SIG_INTERVAL) {
    count++;
    sigTime = currTime;
  }
}

  
