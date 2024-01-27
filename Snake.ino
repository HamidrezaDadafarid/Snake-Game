//define 16*16 matrix for rgb led matrix
#include <FastLED.h>
#define NUM_LEDS 256
#define DATA_PIN 6
#define UP_PIN 2
#define DOWN_PIN 4
#define LEFT_PIN 5
#define RIGHT_PIN 3
#define brightness 250
#define tick 500
int next_x, next_y;
byte matrix[16][16];
CRGB leds[NUM_LEDS];
//0 for being off 1 for snake 2 for food
byte snake_length = 3;
byte snake_x[256] = {};
byte snake_y[256] = {};
byte direction;
byte food_x;
byte food_y;
bool game_over = false;
bool eat_food = false;
void spawn_food(){
    randomSeed(millis());
    food_x = random(0, 16);
    food_y = random(0, 16);
    if(matrix[food_x][food_y] == 0){
        matrix[food_x][food_y] = 2;
    }
    else{
        spawn_food();
    }
}
bool check_game_over(){
    if (snake_length == 256){
        return true;
    } 
    if(direction == 0){
        next_x = snake_x[0];
        next_y = snake_y[0]-1;
    }
    else if(direction == 1){
        next_x = snake_x[0]+1;
        next_y = snake_y[0];
    }
    else if(direction == 2){
        next_x = snake_x[0];
        next_y = snake_y[0]+1;
    }
    else if(direction == 3){
        next_x = snake_x[0]-1;
        next_y = snake_y[0];
    }
    if(next_x < 0 || next_x > 15 || next_y < 0 || next_y > 15){
        return true;
    }
    for(short i = 1; i < snake_length; i++){
        if(next_x == snake_x[i] && next_y == snake_y[i]){
            return true;
        }
    }
    return false;
}
bool hit_food(){
    if(direction == 0){
        next_x = snake_x[0];
        next_y = snake_y[0]-1;
    }
    else if(direction == 1){
        next_x = snake_x[0]+1;
        next_y = snake_y[0];
    }
    else if(direction == 2){
        next_x = snake_x[0];
        next_y = snake_y[0]+1;
    }
    else if(direction == 3){
        next_x = snake_x[0]-1;
        next_y = snake_y[0];
    }
  return (next_x == food_x && next_y == food_y);
}
void setup() {
    //make the pins for the arrow keys input
    pinMode(UP_PIN, INPUT);
    pinMode(DOWN_PIN, INPUT);
    pinMode(LEFT_PIN, INPUT);
    pinMode(RIGHT_PIN, INPUT);
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    //create a Thread for getting input from the arrow keys
    Serial.begin(9600);
    // put your setup code here, to run once:
    FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
    FastLED.setBrightness(brightness);
    FastLED.clear();
    for(short i = 0; i < 16; i++){
        for(short j = 0; j < 16; j++){
            matrix[i][j] = 0;
        }
    }
    for(short i = 0; i < snake_length; i++){
        snake_x[i] = 5;
        snake_y[i] = 8+i;
        matrix[snake_x[i]][snake_y[i]] = 1;
        direction = 0;
    }
    spawn_food();
    update_matrix();
}

void (*resetFunc)(void) = 0;

void loop() {
    // change direction of snake based on input
    if(digitalRead(UP_PIN) == LOW && direction != 2){
        direction = 0;
    }
    else if(digitalRead(RIGHT_PIN) == LOW && direction != 3){
        direction = 1;
    }
    else if(digitalRead(DOWN_PIN) == LOW && direction != 0){
        direction = 2;
    }
    else if(digitalRead(LEFT_PIN) == LOW && direction != 1){
        direction = 3;
    }
    if(millis() % tick == 0){
        update_game();
    }
}
void update_game(){
    eat_food = hit_food();
    if (eat_food == true){
        snake_length++;
        eat_food = false;
        spawn_food();
    }
    if (check_game_over() == true){
        game_over = true;
        resetFunc();
    }
    if (game_over == false){
        //move last block to the front
        if (direction == 0){
            for(short i = snake_length-1; i > 0; i--){
                snake_x[i] = snake_x[i-1];
                snake_y[i] = snake_y[i-1];
            }
            snake_y[0]--;
        }
        else if (direction == 1){
            for(short i = snake_length-1; i > 0; i--){
                snake_x[i] = snake_x[i-1];
                snake_y[i] = snake_y[i-1];
            }
            snake_x[0]++;
        }
        else if (direction == 2){
            for(short i = snake_length-1; i > 0; i--){
                snake_x[i] = snake_x[i-1];
                snake_y[i] = snake_y[i-1];
            }
            snake_y[0]++;
        }
        else if (direction == 3){
            for(short i = snake_length-1; i > 0; i--){
                snake_x[i] = snake_x[i-1];
                snake_y[i] = snake_y[i-1];
            }
            snake_x[0]--;
        }
        
        for(short i = 0; i < 16; i++){
            for(short j = 0; j < 16; j++){
                matrix[i][j] = 0;
            }
        }
        for(short i = 0; i < snake_length; i++){
            matrix[snake_x[i]][snake_y[i]] = 1;
        }
        matrix[food_x][food_y] = 2;
        update_matrix();
    }
    else{
        for(short i = 0; i < 16; i++){
            for(short j = 0; j < 16; j++){
                matrix[i][j] = 0;
            }
        }
        update_matrix();
    }
}
void update_matrix(){
    for(short i = 0; i < 16; i++){
        for(short j = 0; j < 16; j++){
        if(matrix[i][j] == 0){
            leds[i*16+j] = CRGB::Black;
        }
        else if(matrix[i][j] == 1){
            leds[i*16+j] = CRGB::Blue;
        }
        else if(matrix[i][j] == 2){
            leds[i*16+j] = CRGB::Red;
        }
        }
    }
    FastLED.show();
}
