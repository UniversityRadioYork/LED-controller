#include <FastLED.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN     2
#define NUM_LEDS    130
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define CMD_START   '/'
#define CMD_STOP    ';'
#define MAX_CMD_SIZE 8
#define NUM_MODES    5
#define HIGH_BND_PIN 1
#define MID_BND_PIN  2
#define LOW_BND_PIN  3 
char cmd[MAX_CMD_SIZE];
char modes[NUM_MODES] = "abrRv";
uint16_t index = 0;
uint16_t brightness = 25;
char mode = 'a';
int read_cmd = -1;
int step_time = 50;
bool initialised = true;
long time = 0;
// MODES:
// a: ambient
// b: breathing
// r: rainbow
// R: moving rainbow
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
  time = millis();
}

void loop() {
  //As often as possible
  //Get incoming serial data
  if (Serial.available() > 0) {
    char c = Serial.read();
    //Serial.println(c);
    //Process commands
    if(c == CMD_START){
      read_cmd = 0;
    } else if(c == CMD_STOP){
      read_cmd = -1;
      handle_cmd(cmd);
    } else if(read_cmd > -1){
      cmd[read_cmd] = c;
      read_cmd++;
    }
  }
  //Every [delay] milliseconds
  if(millis() - time >= step_time){
    //Iterate animation variable
    i = (i+1)%768;
    //Control lights
    manage_lights();
    FastLED.show();
    time = millis();
  }
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
  return val;
}

void handle_cmd(char command[]){
  Serial.println(command);
  if(command[0] == 'M' && includes(modes, command[1], NUM_MODES)){
    mode = command[1];
    initialised = false;
    Serial.println("OK");
  } else if(command[0] == 'D'){ //Set animation delay
    step_time = getValue(command);
    Serial.println("OK");
  } else if(command[0] == 'B'){ //Set brightness  
    int new_b = getValue(command);
    if(new_b < 256){ //Validate brightness
      brightness = new_b;
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
    fill_solid( leds, NUM_LEDS, CRGB(brightness,0,brightness));

  } else if(mode == 'b'){ //Breathing in 3 colours
    if(i < 256){
      float v = (float)(128-abs(128-i)) /128*brightness;
      fill_solid( leds, NUM_LEDS, CRGB(v,v,0));
    } else if(i < 512){
      float v = (float)(128-abs(384-i)) /128*brightness;
      fill_solid( leds, NUM_LEDS, CRGB(0,v,v));
    } else {
      float v = (float)(128-abs(640-i)) /128*brightness;
      fill_solid( leds, NUM_LEDS, CRGB(v,0,v));
    }

  } else if(mode == 'r'){ //RAINBOWS
    fill_solid( leds, NUM_LEDS, ColorFromPalette(rainbowPal,i%256));

  } else if(mode == 'R'){ //Streaks
    if(!initialised){
      fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
      for(int dot = 0; dot < 5; dot++) { 
        leds[dot] = CRGB(0,brightness,brightness);
      }
    }
    for(int dot = 0; dot < NUM_LEDS-1; dot++) { 
      leds[dot] = leds[dot+1];
    }
    if(leds[0] != CRGB(0,0,0)){
      leds[NUM_LEDS-1] = ColorFromPalette(rainbowPal,i%256);
    }

  } else if(mode == 'v'){
    int lowBand = analogRead(LOW_BND_PIN);
    int midBand = analogRead(MID_BND_PIN);
    int highBand = analogRead(HIGH_BND_PIN);
    // Convert the raw data value (0 - 1023) to voltage (0.0V - 5.0V):
    float red = lowBand/1024 * brightness;
    float green = midBand/1024 * brightness;
    float blue = highBand/1024 * brightness;
    fill_solid( leds, NUM_LEDS, CRGB(red,green,blue));
  }
  initialised = true;
}
