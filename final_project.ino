#include <Wire.h>
#include "rgb_lcd.h"
#include <Adafruit_NeoPixel.h>

#define LED_PIN     6 // Which pin on the Arduino is connected to the NeoPixels?
#define LED_COUNT  16 // How many NeoPixels are attached to the Arduino?
#define BRIGHTNESS 64 // Set BRIGHTNESS to about 1/5 (max = 255) // NeoPixel brightness, 0 (min) to 255 (max)
// Declare our NeoPixel strip object: Argument 1 = Number of pixels in NeoPixel strip; Argument 2 = Arduino pin number (most are valid);  Argument 3 = Pixel type flags, add together as needed:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800); //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs); NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

rgb_lcd lcd;
const int numRows = 2;
const int numCols = 16;
bool throwLight = false; bool clear = false; bool wakeup = false; bool doStuff = false; bool flag = false; bool btnPress = false; bool select = false;
bool think = false; bool opt1 = false; bool opt2 = false; bool opt3 = false;  
int optionValue;
byte heart[8] = {0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000};
byte smile[8] = { 0b00000, 0b00000, 0b01010, 0b00000, 0b10001, 0b01110, 0b00000, 0b00000 };

void setup() {
  // put your setup code here, to run once:
  //lcd.begin(numCols, numRows);
  Serial.begin(57600);               // INITIALIZE Serial Communicatoin (for DEBUGGING)
  lcd.begin(numCols, numRows);      // INITIALIZE Grover LCD lcd object (REQUIRED)
  strip.begin();                    // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                     // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
  
  lcd.createChar(1, heart); // create heart character
  lcd.createChar(2, smile); // create smiling :) character
  pinMode(2,INPUT_PULLUP);          // btn sensor
}

void loop() {
  // put your main code here, to run repeatedly:
  int sensorValue_input; int setConfirm = 0;
  int touchRead = analogRead(A3); // touch sensor value
  //int btn = digitalRead(2);       //btn value default:1
  int light = analogRead(A2);     // light sensor value
  int potValue = analogRead(A0);// // potentiometer value
  //Serial.print("light  reading = ");
  //Serial.println(light); // print the raw analog reading

  if(light < 1000 && !throwLight){ //default mode
    playTouchMeNotGame(touchRead); // this is the sleep function of the Neopixel LED. Watch out, the light gets angry when you touch it.
  }
  else{ // when exposed to LOTS of light
  
    if(!doStuff && !select){
      bool wakeupCheck = wakeUp(potValue, sensorValue_input, setConfirm); //this function allows the Neopixel LED to switch to awake state, user has to confirm decision to trigger state.
      if(wakeupCheck)
      rainbowFade2White(3, 3, 1);
    }
    else if(!think && !select){
      //Serial.print("debug");
      bool wakeupCheck = wakeUp(potValue, sensorValue_input, setConfirm); //this function allows the Neopixel LED to do actions in awake state, user has to select option.
      colorWipe(strip.Color(  0,   0,   0, 255), 50);
    }
    else{
      bool wakeupCheck = wakeUp(potValue, sensorValue_input, setConfirm); //this function allows the Neopixel LED to do actions in awake state, user has to select option.
      if(select){
        if(opt2){
          brighten();
          darken();
        }
        if(opt3){
          colorWipe(strip.Color(255,   0,   0)     , 25); // Red
          colorWipe(strip.Color(  0,   0, 255)     , 50); // Blue
        }
      }
    }
      //whiteOverRainbow(75, 5);
    
    //doStuff = implementAction();

  }

}





//---------1. Touch Me Not Game functions  ---------------
void playTouchMeNotGame(int touchRead){ //works fine
  if (touchRead < 500){       // from 0 to 499
    //Serial.println(" -> light squeeze");
    lcd.clear(); //delay(500);
    lcd.setCursor(0, 0);
    lcd.print("DND!"); //delay(2000);
    lcd.setCursor(0, 1);
    lcd.print("Zzz..Zz..Z.."); 
    pulseWhite(10);   //sleeping animation
  }
  else if (touchRead < 800){  // from 500 to 799
    //Serial.println(" -> medium squeeze");
    lcd.clear(); delay(500);
    lcd.setCursor(0, 0);
    lcd.print("Don't Wake me up!"); //delay(2000);
    lcd.setCursor(0, 1);
    lcd.print("Zzz..Zz..Z.."); 
    // sleeping animation mild
    irritatedEmoji();
  } 
  else{ // from 800 to 1023
    //Serial.println(" -> big squeeze");
    lcd.clear(); delay(500);
    lcd.setCursor(0, 0);
    lcd.print("STOP IT!"); delay(200);
    lcd.setCursor(0, 1);
    lcd.print("GRRrr.."); delay(500);
    //lcd.clear();
    pissedEmoji();
  } 
  delay(100);
}
void pulseWhite(uint8_t wait) {
  for(int j=0; j<256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }

  for(int j=255; j>=0; j--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
}
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    //Serial.println(i);

    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
    strip.clear();
  }
  //strip.clear();
}
void irritatedEmoji(){ //trigger Anger Level - 1
  colorWipe(strip.Color(255,   0,   0)     , 50); // Red
  pulseWhite(5);
}
void pissedEmoji(){ //trigger Anger Level - 2
  colorWipe(strip.Color(255,   0,   0)     , 25); // Red
  pulseWhite(1);
}

//---------2. Wake up functions ---------------
bool wakeUp(int potValue, int sensorValue_input, int setConfirm){
  // display wake up msg
    throwLight = true;
    if(!clear){
      lcd.clear();
      clear = true;
    }
    if(!wakeup){
      lcd.setCursor(0, 0);
      lcd.print("Wake UP?1:Y::2:N"); //delay(1000);
    
      //...........Ask user to confirm.........
      int sensorValue_Yes = map(potValue, 0, 511, 1, 1); //option YES
      int sensorValue_No = map(potValue, 512, 1023, 2, 2); //option NO
      //yes - potentiometer: show rainbow animation (happy and gay) : use flag to not enter if block
      //no - potentiometer: go back to sleep
      if(potValue >= 0 && potValue < 512){
        lcd.setCursor(0, 1);
        //lcd.print("                ");
        lcd.print(sensorValue_Yes); 
        sensorValue_input = sensorValue_Yes;
      }
      else{
        lcd.setCursor(0, 1);
        //lcd.print("                ");
        lcd.print(sensorValue_No); 
        sensorValue_input = sensorValue_No;
      }
    }
    
    setConfirm = checkBtnTrigger(sensorValue_input);
    if(setConfirm == 1 && !wakeup){ //wake up happy and gay
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Okay then       ");
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.print("INITIALISING");
      for(int i =0; i < 4; i++ ){
        delay(1500); lcd.print(".");
      } 
      lcd.setCursor(0, 0);
      lcd.print("I woke up!       ");
      lcd.setCursor(11, 0);
      lcd.write(2);
      lcd.setCursor(0, 1);
      lcd.print("Long Press Btn");
      for(int i =0; i < 3; i++ ){
        delay(300); lcd.print(".");
      } 
      wakeup = true; //flag = false;
    }
    else if(setConfirm == 2  && !wakeup){ // go back to Sleep mode which is the default mode
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ok dozing off"); 
      for(int i =0; i < 3; i++ ){
        delay(200); lcd.print(".");
      }
      delay(2000);
      throwLight = false; clear = false; wakeup=false;
    }

    if(wakeup){
      //Serial.print("Debug");
      implementAction(sensorValue_input);
    }
    return wakeup;
}
int checkBtnTrigger(int input){
  int setValue;
  int btn_setNext = digitalRead(2);//sets value to 0 when button is pressed
  //Serial.print(input);
  if(btn_setNext == 0){
      setValue = input;
      //Serial.print(setValue);
      delay(100);
    }
    else{
      setValue = 0;
    }
  return setValue;
}
void rainbowFade2White(int wait, int rainbowLoops, int whiteLoops) {
  int fadeVal=0, fadeMax=100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) {

    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255,
        255 * fadeVal / fadeMax)));
    }

    strip.show();
    delay(wait);

    if(firstPixelHue < 65536) {                              // First loop,
      if(fadeVal < fadeMax) fadeVal++;                       // fade in
    } else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) { // Last loop,
      if(fadeVal > 0) fadeVal--;                             // fade out
    } else {
      fadeVal = fadeMax; // Interim loop, make sure fade is at max
    }
  }

  for(int k=0; k<whiteLoops; k++) {
    for(int j=0; j<256; j++) { // Ramp up 0 to 255
      // Fill entire strip with white at gamma-corrected brightness level 'j':
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
    }
    delay(1000); // Pause 1 second
    for(int j=255; j>=0; j--) { // Ramp down 255 to 0
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
    }
  }

  delay(500); // Pause 1/2 second
}

//---------3. Implement Action functions ---------------
void implementAction(int sensorValue_input){
  
  int setConfirm = checkBtnTrigger();
  //Serial.print(setConfirm);
  //Serial.print(btnPress);
  if(setConfirm == 0 && !btnPress){
    lcd.setCursor(0, 0);
    lcd.print("Pick Action 4 me"); //delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("<-Prev    Next->"); 
    btnPress = true; doStuff = true;
  }
  if(btnPress){
    //Serial.print(btnPress);
    //...........Give User 4 options to select from.........
    int potValue = analogRead(A0);// // potentiometer value
    int sensor_opt1 = map(potValue, 0, 255, 1, 1); //option 1
    int sensor_opt2 = map(potValue, 255, 511, 2, 2); //option 2
    int sensor_opt3 = map(potValue, 511, 767, 3, 3); //option 3
    int sensor_opt4 = map(potValue, 767, 1023, 4, 4); //option 4

    //Serial.print(potValue);
    if(potValue >= 0 && potValue < 255){ //opt 1
      lcd.setCursor(0, 0);
      lcd.print("Ross's Sad Story");
      lcd.setCursor(0, 1);
      lcd.print("<-Prev 01 Next->"); 
      sensorValue_input = sensor_opt1;
      //delay(2000);
    }
    else if(potValue >= 255 && potValue < 512){ //opt 2
      lcd.setCursor(0, 0);
      lcd.print("Earth Warrior");
      for(int i = 13; i < numCols ;i++){
        lcd.setCursor(i, 0);
        lcd.write(1);
      }
      lcd.setCursor(0, 1);
      lcd.print("<-Prev 02 Next->"); 
      sensorValue_input = sensor_opt2;
    }
    else if(potValue >= 512 && potValue < 767){ //opt 3
      lcd.setCursor(0, 0);
      lcd.print("       SOS      ");
      lcd.setCursor(0, 1);
      lcd.print("<-Prev 03 Next->"); 
      sensorValue_input = sensor_opt3;
    }
    else{                             // opt 4
      lcd.setCursor(0, 0);
      lcd.print("Go back to sleep");
      lcd.setCursor(0, 1);
      lcd.print("<-Prev 04 Next->"); 
      sensorValue_input = sensor_opt4;
    }
    
    //-------------trigger tasks 
    int setConfirm = checkBtnTrigger();
    
    if(setConfirm == 0){
      optionValue = sensorValue_input;
      //Serial.print(optionValue);
      
      switch(optionValue){
        case 1: //01. Ross's Sad Story (Animation: Blue Fade)
            opt1 = true; opt2 = false; opt3 = false;
            brighten_blue();
            darken_blue();
            select = true;
            break;

        case 2: //02. Earth Warrior (Animation: Blue-Green Fade)
            //doStuff= true; 
            opt1 = false; opt2 = true; opt3 = false;
            brighten();
            darken();
            select = true;
            break;

        case 3: //03. SOS (Animation: Red-Blue rapid cycles)
            opt1 = false; opt2 = false; opt3 = true;
            colorWipe(strip.Color(255,   0,   0)     , 25); // Red
            colorWipe(strip.Color(  0,   0, 255)     , 50); // Blue
            select = true;
            break;

        case 4: //04. Go back to sleep (Animation: Default Sleep mode)
             lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Ok dozing off"); 
            for(int i =0; i < 3; i++ ){
              delay(200); lcd.print(".");
            }
            delay(2000);
            throwLight = false; clear = false; wakeup=false; select = false;
          break;
      }
    }
  }  
}
int checkBtnTrigger(){ //this is a function overload. "checkBtnTrigger" is a shared method name only difference is the # of parameters passed into the function.
  int setValue;
  int btn_setNext = digitalRead(2);//sets value to 0 when button is pressed
  //Serial.print(input);
  if(btn_setNext == 0){
      setValue = 0;
      //Serial.print(setValue);
      delay(100);
    }
    else{
      setValue = 1;
      //Serial.print(setValue);
    }
  return setValue;
}
// 0 to 255
void brighten() {
  uint16_t i, j;
  for (j = 0; j < 200; j++) {
    for (i = 0; i < strip.numPixels(); i=i+2) {
      strip.setPixelColor(i, 0, 100, j);
    }
    for (i = 1; i <= strip.numPixels()-1 ; i=i+2) {
      strip.setPixelColor(i, 0, j, 100);
    }
    strip.show();
    delay(10);
  }
  delay(1000);
}
// 255 to 0
void darken() {
  
  uint16_t i, j;

  for (j = 200; j > 0; j--) {
    for (i = 0; i < strip.numPixels(); i=i+2) {
      strip.setPixelColor(i, 0, 100, j);
    }
    for (i = 1; i <= strip.numPixels()-1 ; i=i+2) {
      strip.setPixelColor(i, 0, j, 100);
    }
    strip.show();
    delay(10);
    //Serial.println(j);
  }
  delay(1000);
}
// 0 to 255
void brighten_blue() {
  uint16_t i, j;

  for (j = 0; j < 200; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, j);
    }
    strip.show();
    delay(10);
  }
  //delay(1000);
}

// 255 to 0
void darken_blue() {
  
  uint16_t i, j;

  for (j = 200; j > 0; j--) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, j);
    }
    strip.show();
    delay(10);
    Serial.println(j);
  }
  delay(1000);
}


//------------UNUSED Functions Stored Under----------
void whiteOverRainbow(int whiteSpeed, int whiteLength) {

  if(whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;

  int      head          = whiteLength - 1;
  int      tail          = 0;
  int      loops         = 3;
  int      loopNum       = 0;
  uint32_t lastTime      = millis();
  uint32_t firstPixelHue = 0;

  for(;;) { // Repeat forever (or until a 'break' or 'return')
    for(int i=0; i<strip.numPixels(); i++) {  // For each pixel in strip...
      if(((i >= tail) && (i <= head)) ||      //  If between head & tail...
         ((tail > head) && ((i >= tail) || (i <= head)))) {
        strip.setPixelColor(i, strip.Color(0, 0, 0, 255)); // Set white
      } else {                                             // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
    }

    strip.show(); // Update strip with new contents
    // There's no delay here, it just runs full-tilt until the timer and
    // counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    if((millis() - lastTime) > whiteSpeed) { // Time to update head/tail?
      if(++head >= strip.numPixels()) {      // Advance head, wrap around
        head = 0;
        if(++loopNum >= loops) return;
      }
      if(++tail >= strip.numPixels()) {      // Advance tail, wrap around
        tail = 0;
      }
      lastTime = millis();                   // Save time of last movement
    }
  }
}