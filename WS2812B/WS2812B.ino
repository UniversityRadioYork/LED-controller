#include <FastLED.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN     2
#define NUM_LEDS    130
#define BRIGHTNESS  45
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define CMD_START   '/'
#define CMD_STOP    ';'
#define MAX_CMD_SIZE 8
#define NUM_MODES    4
#define HIGH_BND_PIN 1
#define MID_BND_PIN  2
#define LOW_BND_PIN  3 
char cmd[MAX_CMD_SIZE];
char modes[NUM_MODES] = "abrv";
uint16_t index = 0;
uint16_t brightness = 80;
char mode = 'a';
int read_cmd = -1;
int step_time = 1;
// MODES:
// a: ambient
// b: breathing
// r: rainbow
// v: Visualizer

int i = 0;
CRGB leds[NUM_LEDS];

DEFINE_GRADIENT_PALETTE( spectrum ) {
  0,   50,  0,  0, //Red
 40,   40, 40,  0, //Yellow
 80,    0, 50,  0, //Green
120,    0, 40, 40, //Cyan
160,    0,  0, 50, //Blue
200,   40,  0, 40, //Magenta
255,   50,  0,  0  //Red
};               //Except this is GRB sooooo
CRGBPalette16 rainbowPal = spectrum;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  for(int i=0; i<5; i++){
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(300);                       // wait for a second
  }
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
}

void loop() {
  //Iterate animation variable
  i = (i+1)%768;
  delay(step_time);
  //Get incoming serial data
  if (Serial.available() > 0) {
    char c = Serial.read();
    //Process commands
    if(c == CMD_START){
      Serial.println(mode);
      read_cmd = 0;
    } else if(c == CMD_STOP){
      read_cmd = -1;
      handle_cmd(cmd);
    } else if(read_cmd > -1){
      cmd[read_cmd] = c;
      read_cmd++;
    }
  }
  //Control lights
  manage_lights();
  FastLED.show();
}

boolean includes(char array[], char element, int array_len) {
  for (int j = 0; j < array_len; j++) {
      if (array[j] == element) {
          return true;
      }
    }
  return false;
}

int getValue(char letters[]){
  int val = atoi(&letters[1]);
  Serial.println(val);
  return val;
}

void handle_cmd(char command[]){
  if(command[0] == 'M' && includes(modes, command[1], NUM_MODES)){
    mode = command[1];          //Solid colour; Default
    Serial.println("OK");
  } else if(command[0] == 'D'){ //Set animation delay
    step_time = getValue(command);
    Serial.println("OK");
  } else if(command[0] == 'B'){ //Set brightness  
    new_b = getValue(command);
    if(0 <= new_b < 256){
      brightness = new_b
      Serial.println("OK");
    } else {
      Serial.println("ERR");
    }
  } else {                      //Unrecognised command
    Serial.println("ERR");
  }
}

void manage_lights(){ //Runs every cycle
  if(mode == 'a'){        //Solid colour; Default
    fill_solid( leds, NUM_LEDS, CRGB(BRIGHTNESS,0,BRIGHTNESS));
  } else if(mode == 'b'){ //Breathing in 3 colours
    if(i < 256){
      float v = (float)(128-abs(128-i)) /128*BRIGHTNESS;
      fill_solid( leds, NUM_LEDS, CRGB(v,v,0));
    } else if(i < 512){
      float v = (float)(128-abs(384-i)) /128*BRIGHTNESS;
      fill_solid( leds, NUM_LEDS, CRGB(0,v,v));
    } else {
      float v = (float)(128-abs(640-i)) /128*BRIGHTNESS;
      fill_solid( leds, NUM_LEDS, CRGB(v,0,v));
    }
  } else if(mode == 'r'){ //RAINBOWS
    fill_solid( leds, NUM_LEDS, ColorFromPalette(rainbowPal,i%256));
  } else if(mode == 'v'){
    int lowBand = analogRead(LOW_BND_PIN);
    int midBand = analogRead(MID_BND_PIN);
    int highBand = analogRead(HIGH_BND_PIN);
    // Convert the raw data value (0 - 1023) to voltage (0.0V - 5.0V):
    float red = lowBand/1024 * BRIGHTNESS;
    float green = midBand/1024 * BRIGHTNESS;
    float blue = highBand/1024 * BRIGHTNESS;
    fill_solid( leds, NUM_LEDS, CRGB(red,green,blue));
  }
}
