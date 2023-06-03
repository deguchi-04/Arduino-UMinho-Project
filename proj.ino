/************************************/
/*              GRUPO X             */
/************************************/
 
// Membros:
// Inês Amaro - A101668
// Nome - Numero
// Nome - Numero
// Nome - Numero
// Nome - Numero


/************************************/
/*             INCLUDES             */
/************************************/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_NeoPixel.h>

/************************************/
/*             DEFINES              */
/************************************/

Adafruit_7segment matrix = Adafruit_7segment();
const int increaseButtonPin = 2;        // Pin for the increase button
const int decreaseButtonPin = 3;        // Pin for the decrease button
const int photoresistorPin = A0;        // Pin for the photoresistor
const int ledPin = 4;                   // Pin for the LED
const int ultrasonicSensorPin = A1;     // Pin for the ultrasonic sensor
const int neoPixelPin = 6;              // Pin for the NEO pixel ring
const int neoPixelCount = 12;           // Number of pixels in the ring

bool increaseButtonState = HIGH;        // State of the increase button
bool decreaseButtonState = HIGH;        // State of the decrease button
bool increaseButtonPressed = false;     // Flag to track increase button press
bool decreaseButtonPressed = false;     // Flag to track decrease button press

Adafruit_NeoPixel neoPixelRing = Adafruit_NeoPixel(neoPixelCount, neoPixelPin, NEO_GRB + NEO_KHZ800);



/************************************/
/*             VARIABLES            */
/************************************/



/************************************/
/*              TRIGGERS            */
/************************************/



/************************************/
/*              ACTIONS             */
/************************************/

int measureDistance() {
  // Send a pulse to the ultrasonic sensor
  pinMode(ultrasonicSensorPin, OUTPUT);
  digitalWrite(ultrasonicSensorPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicSensorPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicSensorPin, LOW);

  // Measure the time for the pulse to return
  pinMode(ultrasonicSensorPin, INPUT);
  unsigned long duration = pulseIn(ultrasonicSensorPin, HIGH);

  // Calculate the distance based on the speed of sound
  float distance = duration * 0.034 / 2;  // Divide by 2 because the sound travels forth and back

  return distance;
}


/************************************/
/*               OTHER              */
/************************************/



/*************  SETUP  *************/

void setup() {
  matrix.begin(0x70);                    // Initialize HT16K33 display
  matrix.setBrightness(15);              // Set brightness (0-15)
  matrix.print("0000");                   // Set initial countdown value
  matrix.writeDisplay();

  pinMode(increaseButtonPin, INPUT_PULLUP);      // Set increase button pin as input with internal pull-up resistor
  pinMode(decreaseButtonPin, INPUT_PULLUP);      // Set decrease button pin as input with internal pull-up resistor
  pinMode(photoresistorPin, INPUT);               // Set photoresistor pin as input
  pinMode(ledPin, OUTPUT);                        // Set LED pin as output
  neoPixelRing.begin();                           // Initialize NEO pixel ring
  neoPixelRing.clear();                           // Clear any existing pixel colors
  neoPixelRing.show();                            // Update the ring

  Serial.begin(9600);
  delay(1000);                              // Wait for display to initialize
}


/*************  LOOP  *************/





void loop() {
  int countdown = 120;                      // Set the initial countdown value (in seconds)
  bool countdownFrozen = false;             // Flag to indicate if the countdown is frozen

  while (!countdownFrozen) {
    int minutes = countdown / 60;            // Calculate the minutes
    int seconds = countdown % 60;            // Calculate the seconds

    // Check if the countdown reached 0 or decreased time is higher than current display time
    if (countdown == 0 || (decreaseButtonPressed && countdown <= 120)) {
      minutes = 0;
      seconds = 0;
      countdownFrozen = true;                // Freeze the countdown
    }

    // Display the countdown on the 7-segment display
    matrix.print(minutes * 100 + seconds);
    matrix.drawColon(true);
    matrix.writeDisplay();

    // Check if the increase button state changed (button pressed)
    if (digitalRead(increaseButtonPin) == LOW && increaseButtonState == HIGH) {
      increaseButtonPressed = true;
    }
    increaseButtonState = digitalRead(increaseButtonPin);

    // Check if the decrease button state changed (button pressed)
    if (digitalRead(decreaseButtonPin) == LOW && decreaseButtonState == HIGH) {
      decreaseButtonPressed = true;
    }
    decreaseButtonState = digitalRead(decreaseButtonPin);

    // Handle button presses
    if (increaseButtonPressed) {
      countdown += 120;                    // Increase countdown by 2 minutes
      increaseButtonPressed = false;       // Reset the button flag
    }
    if (decreaseButtonPressed) {
      // Check if the decreased time is higher than the current countdown time
      if (countdown > 120) {
        countdown -= 120;                   // Decrease countdown by 2 minutes
      } else {
        countdown = 0;                       // Set countdown to zero
        minutes = 0;
        seconds = 0;
        countdownFrozen = true;              // Freeze the countdown
      }
      decreaseButtonPressed = false;         // Reset the button flag
    }

    Serial.println(analogRead(photoresistorPin));
    if (analogRead(photoresistorPin) > 900) { //adjust value to the reality
      neoPixelRing.clear();           // Turn on the LED
    } else {
      for (int i = 0; i < neoPixelCount; i++) {
        neoPixelRing.setPixelColor(i, 255, 0, 0);  // Set pixel color to red
      }    
      neoPixelRing.show(); // Turn off the LED
    }

    // Read ultrasonic sensor value
    int distance = measureDistance();

    // Check if there is movement detected (adjust the threshold value according to your sensor)
    if (distance < 100) {
      // Light up the NEO pixel ring
      for (int i = 0; i < neoPixelCount; i++) {
        neoPixelRing.setPixelColor(i, 255, 0, 0);  // Set pixel color to red
      }
    } else {
      //neoPixelRing.clear();                         // Clear the pixel colors
    }
    neoPixelRing.show();                             // Update the ring

    delay(1000);                              // Delay for 1 second
    countdown--;                              // Decrease the countdown by 1 second
  }

  // Freeze the display at "00:00"
  while (true) {
    matrix.print("0000");
    matrix.drawColon(true);
    matrix.writeDisplay();

    // Check if the increase button is pressed to start a new countdown
    if (digitalRead(increaseButtonPin) == LOW) {
      countdown = -120;                      // Set the countdown value to 2 minutes
      countdownFrozen = false;               // Unfreeze the countdown
      delay(1000);
      break;                                 // Exit the freeze loop and start a new countdown
    }
  }
}





