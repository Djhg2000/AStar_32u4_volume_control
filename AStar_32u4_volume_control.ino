/*

  Requires Stefan Jones's modified HID.cpp and USBAPI.cpp from
  this GutHub repo: https://github.com/stefanjones/Arduino/tree/master/hardware/arduino/cores/arduino

*/

// Pins
#define REA 5       // Rotary Encoder A
#define REB 4       // Rotary Encoder B
#define REBUTTON 6  // Rotary Encoder Button
#define REG 8       // Rotary Encoder Green LED (currently unused)
#define RER 9       // Rotary Encoder Red LED
#define LED 13      // Debug LED

// Hardware features
#define RESCALE 4   // Rotary Encoder step scale, you probably want
                    // to set this to 1 if you have tactile steps
#define LEDA 1      // LED pin is analog (non-zero = analog)

boolean stateA;
boolean stateB;
boolean stateButton;
boolean oldstateA;
boolean oldstateB;
boolean oldstateButton;
unsigned long timedButton = 0;
int rotation = 0;
int momentum = 0;
unsigned long timeMomentum = millis();

void setup()
{
  Remote.begin();
  pinMode(REA, INPUT);
  pinMode(REB, INPUT);
  pinMode(REBUTTON, INPUT);
  pinMode(REG, OUTPUT);
  pinMode(RER, OUTPUT);
  // Pull up on pins A, B and Button
  digitalWrite(REA, HIGH);
  digitalWrite(REB, HIGH);
  digitalWrite(REBUTTON, HIGH);
  analogWrite(REG, 15);
  analogWrite(RER, 15);
  // Initial sampling of pins
  stateA = digitalRead(REA);
  stateB = digitalRead(REB);
  stateButton = digitalRead(REBUTTON);
}

void loop()
{
  if (LEDA)
  {
    // Cool light effects
    analogWrite(RER, momentum);
  }
  else if (momentum > 0)
  {
    digitalWrite(RER, HIGH);
  }
  else
  {
    digitalWrite(RER, LOW);
  }
  
  // 227 = 255 - ((4 + 3) * 4), worst case scenario
  if ((abs(rotation) > RESCALE) && (momentum < 227))
  {
    momentum += abs(rotation)*4;
  }
  else if ((momentum > 1) && (millis() > (timeMomentum + 10)))
  {
    timeMomentum = millis();
    momentum -= 1;
  }
  else if ((momentum == 1) && (millis() > (timeMomentum + 50)))
  {
    momentum = 0;
    rotation = 0;
  }

  // This code block scales down the rotation a bit
  while (abs(rotation) > RESCALE)
  {
    if (rotation > 0) {Remote.increase();rotation -= RESCALE;}
    else {Remote.decrease();rotation += RESCALE;}
    
    // Clear remainder of steps rotated
    // Very useful if you rewrite this block to have a timer for
    // rate limiting (very sensitive encoders might need this)
    //if (abs(rotation) <= RESCALE) {rotation = 0;}

    // Clear the output buffer or Microsoft Windows will start
    // to autorepeat the last key when finetuning the volume level
    Remote.clear();
  }

  // Save old values and resample
  oldstateA = stateA;
  oldstateB = stateB;
  oldstateButton = stateButton;
  stateA = digitalRead(REA);
  stateB = digitalRead(REB);
  stateButton = digitalRead(REBUTTON);

  // Figure out turning direction
  if (stateA ^ oldstateA)
  {
    if (stateA == stateB) {rotation--;}
    else {rotation++;}
  }
  else if (stateB ^ oldstateB)
  {
    if (stateB == stateA) {rotation++;}
    else {rotation--;}
  }

  // Mute on button press
  if (stateButton < oldstateButton) {Remote.mute();}
}

