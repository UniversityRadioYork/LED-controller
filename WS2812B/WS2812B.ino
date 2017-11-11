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
#define CMD_STOP    '#'
#define MAX_CMD_SIZE 8
#define NUM_MODES    6
#define HIGH_BND_PIN 1
#define MID_BND_PIN  2
#define LOW_BND_PIN  3 
char cmd[] = "#######";
char modes[] = "abrRvp";
uint16_t index = 0;
uint16_t brightness = 25;
char mode = 'a';
int read_cmd = -1;
float step_time = 50;
bool initialised = true;
long time = 0;
int direction = 0;
int PingPongPos = 0;
CRGB current_color;
// MODES:
// a: ambient
// b: breathing
// r: rainbow
// R: moving rainbow
// v: Visualizer
// p: ping pong pang

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

	Serial.println(color_data);
	return CRGB(ri,gi,bi);
}

void setup() {
	current_color = CRGB(brightness,0,brightness);
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
	time = millis();
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
	String number = ((String)letters).substring(1,4);
	char copy[3];
	number.toCharArray(copy, 4);
	int val = atoi(copy);
	Serial.println(val);
	return val;
}

void handle_cmd(char command[]){
	Serial.println(command);
	//Do a thing depending on the command type
	switch(command[0]){
		//Set the mode
		case 'M':
			if(!includes(modes, command[1], NUM_MODES)){
				Serial.println("modeERR");
			} else {
				mode = command[1];
				initialised = false;
			}
			break;
		//Set the brightness
		case 'B':
			int new_b;
			new_b = getValue(command);
			if(new_b < 256){
				brightness = new_b;
			}
			break;
		//Set the color
		case 'C':
			current_color = parseColor(&command[1]);
			Serial.print("|");
			Serial.print(current_color.red);
			Serial.print(",");
			Serial.print(current_color.blue);
			Serial.print(",");
			Serial.print(current_color.green);
			Serial.println("|");
			break;
		//Set the animation delay
		case 'D':
			step_time = getValue(command);
			break;
		//In case of unrecognised command
		default:
			Serial.println("ERR");
	}
}

void manage_lights(){ //Runs every cycle
	switch(mode){
		//Solid dynamic color
		case 'a':
			fill_solid( leds, NUM_LEDS, current_color);
			break;
		//Breathing preset colors
		case 'b':
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
			break;
		//Solid rainbow fading
		case 'r':
			fill_solid( leds, NUM_LEDS, ColorFromPalette(rainbowPal,i%256));
			break;
		//Moving rainbow gradient
		case 'R':
			leds[NUM_LEDS-1] = ColorFromPalette(rainbowPal,i%256);
			for(int dot = 0; dot < NUM_LEDS-1; dot++) { 
				leds[dot] = leds[dot+1];
			}
			leds[NUM_LEDS-1] = ColorFromPalette(rainbowPal,i%256);
			break;
		//ping pong ping pong pingpongpingpongPINGPONGPINGPONG
		case 'p':
			int MaxSlow;
			MaxSlow = 99;
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
				direction = -direction;
				step_time = step_time * .75 ;
			} 
      if(step_time <= 0.00001) {
				step_time = MaxSlow;
			}
			break;
		default:
			fill_solid( leds, NUM_LEDS, CRGB(brightness,brightness,0));
	}
	initialised = true;
}

void loop() {
	//As often as possible
	//Get incoming serial data
	
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