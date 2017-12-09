#include <FastLED.h>
#ifdef __AVR__
	#include <avr/power.h>
#endif

#define LED_PIN     2
#define NUM_LEDS    484
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define CMD_START   '/'
#define CMD_STOP    ';'
#define CMD_VALID   '#'
#define MAX_CMD_SIZE 8
#define HIGH_BND_PIN 1
#define MID_BND_PIN  2
#define LOW_BND_PIN  3 
char cmd[] = "#######";
char modes[] = "abcfgpv";
uint16_t index = 0;
char mode = 'a';
int read_cmd = -1;
float step_time = 50;
bool initialised = true;
long time = 0;
int direction = 0;
int PingPongPos = 0;
bool offbeat = false;
bool Santa = false;
CRGB current_color;
CRGBPalette16 current_pal;
// MODES:
// a: ambient
// b: breathing
// r: rainbow
// R: moving rainbow
// v: Visualizer
// p: ping pong pang
// c: christmas

// PALETTES:
// r: rainbow / spectrum
// w: warm colors
// c: cold colors

int i = 0;
CRGB leds[NUM_LEDS];

DEFINE_GRADIENT_PALETTE( spectrum ) {
  0,  255,  0,  0, //Red
 40,  200,200,  0, //Yellow
 80,    0,255,  0, //Green
120,    0,200,200, //Cyan
160,    0,  0,255, //Blue
200,  200,  0,200, //Magenta
255,  255,  0,  0  //Red
};
CRGBPalette16 rainbowPal = spectrum;

DEFINE_GRADIENT_PALETTE( warm ) {
  0,  255,  0,  0, //Red
100,  200,120,  0, //Yellow
150,  255,  0,  0, //Red
200,  200,  0, 80, //Magenta
255,  255,  0,  0  //Red
};
CRGBPalette16 warmPal = warm;

DEFINE_GRADIENT_PALETTE( cold ) {
  0,    0,  0,255, //Blue
 40,    0,200,200, //Cyan
180,    0,255,  0, //Green
255,    0,  0,255, //Blue
};
CRGBPalette16 coldPal = cold;

CRGB parseColor(char color_data[]){
	String r = "00";
	String g = "00";
	String b = "00"; 

	r[0] = color_data[0];
	r[1] = color_data[1];
	g[0] = color_data[2];
	g[1] = color_data[3];
	b[0] = color_data[4];
	b[1] = color_data[5];
	
	long ri = strtol(&r[0],NULL,16);
	long gi = strtol(&g[0],NULL,16);
	long bi = strtol(&b[0],NULL,16);

	// Serial.println(color_data);
	return CRGB(ri,gi,bi);
}

void setup() {
	current_color = CRGB(255,0,255);
	current_pal = rainbowPal;
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
	FastLED.setBrightness(80);
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
	time = millis();
}

boolean includes(char array[], char element) {
	for (int j = 0; j < strlen(array); j++) {
			if (array[j] == element) {
					return true;
			}
		}
	return false;
}

int getValue(char letters[]){
	String number = ((String)letters).substring(1,4);
	char copy[3];
	number.toCharArray(copy, 4);
	int val = atoi(copy);
	// Serial.println(val);
	return val;
}

void handle_cmd(char command[]){
	//Do a thing depending on the command type
	switch(command[0]){
		//Set the mode
		case 'M':
			if(!includes(modes, command[1])){
				Serial.println("ERR");
			} else {
				mode = command[1];
				initialised = false;
        if(command[1] == "c")
        {
          Santa = false;
        }
			}
			break;
		//Set the palette
		case 'P':
			switch(command[1]){
				case 'r':
					current_pal = rainbowPal;
					break;
				case 'w':
					current_pal = warmPal;
					break;
				case 'c':
					current_pal = coldPal;
					break;
				default:
					Serial.println("ERR");
			}
			break;
		//Set the brightness
		case 'B':
			int new_b;
			new_b = getValue(command);
			if(new_b < 256){
				FastLED.setBrightness( new_b );
			}
			break;
		//Set the color
		case 'C':
			current_color = parseColor(&command[1]);
			Serial.print("|");
			Serial.print(current_color.red);
			Serial.print(",");
			Serial.print(current_color.green);
			Serial.print(",");
			Serial.print(current_color.blue);
			Serial.println("|");
			break;
		//Set the animation delay
		case 'D':
			step_time = getValue(command);
			break;
    case 'S':
      if(mode == "a")
      {
        Santa = true;
      }
      else
      {
        Santa = false;
      }
      break;
		//In case of unrecognised command
		default:
			Serial.println("ERR");
	}
}

void manage_lights(){ //Runs every cycle
	if(Santa)
  {
    mode = 'c';
  }
	 
	switch(mode){
		//Solid dynamic color
		case 'a':
			fill_solid( leds, NUM_LEDS, current_color);
			break;
		//Breathing preset colors
		case 'b':
			if(i < 256){
        float v = 255 - abs(127 - i%256)*1.9;
				fill_solid( leds, NUM_LEDS, CRGB(v,v,0));
			} else if(i < 512){
        float v = 255 - abs(127 - i%256)*1.9;
				fill_solid( leds, NUM_LEDS, CRGB(0,v,v));
			} else {
        float v = 255 - abs(127 - i%256)*1.9;
				fill_solid( leds, NUM_LEDS, CRGB(v,0,v));
			}
			break;
		//Solid rainbow fading
		case 'f':
			fill_solid( leds, NUM_LEDS, ColorFromPalette(current_pal,i%256));
			break;
		//Moving rainbow gradient
		case 'g':
			leds[NUM_LEDS-1] = ColorFromPalette(current_pal,i%256);
			for(int dot = 0; dot < NUM_LEDS-1; dot++) { 
				leds[dot] = leds[dot+1];
			}
			leds[NUM_LEDS-1] = ColorFromPalette(current_pal,i%256);
			break;
		//ping pong ping pong pingpongpingpongPINGPONGPINGPONG
		case 'p':
			int MaxSlow;
			MaxSlow = 50;
			if(!initialised){
				step_time = MaxSlow;
				direction = 1;
				fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
				leds[0] = current_color;
				PingPongPos = 0;
				//fill_solid(leds, 5, current_color);
			}
			if(direction == 1){
				leds[PingPongPos] = CRGB(0,0,0);
				leds[PingPongPos+1] = current_color;
				PingPongPos++;
			} else {
				leds[PingPongPos] = CRGB(0,0,0);
				leds[PingPongPos-1] = current_color;
				PingPongPos--;
			} 
      if(!(leds[NUM_LEDS-1] == CRGB(0,0,0) && leds[0] == CRGB(0,0,0)) && initialised){
				leds[PingPongPos] = CRGB(0,0,0);
        leds[0] = current_color;
        PingPongPos = 0;
        step_time /= 3;
			} 
      if(step_time <= 0.00001) {
				step_time = MaxSlow;
			}
			break;
    //jingle
    case 'c':
      for(int i = 0+offbeat; i < NUM_LEDS; i+=2)
      {
        leds[i] = CRGB(255, 0 , 0);
      }
      for(int i = 1+offbeat; i < NUM_LEDS; i+=2)
      {
        leds[i] = CRGB(0, 255 , 0);
      }
      offbeat = !offbeat;
      break;
		default:
			fill_solid( leds, NUM_LEDS, CRGB(255,255,0));
	}
	initialised = true;
}

void loop() {
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

void serialEvent(){
	while (Serial.available()) {
		char c = Serial.read();
		switch(c){
			case CMD_START:
				read_cmd = 0;
				break;
			case CMD_STOP:
				read_cmd = -1;
				Serial.print("/");
				Serial.println(cmd);
				break;
		      case CMD_VALID:
		        handle_cmd(cmd);
			default:
				if(read_cmd > -1){
					cmd[read_cmd] = c;
					read_cmd++;
				}
		}
	}
}
