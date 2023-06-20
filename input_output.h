#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//draw the arrow
byte customChar[] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
  B00000
};

//button pin up down and select
int btn_pin[3] = {2, 3, 4};

//limit switch top and buttom
int ls_pin[2] = {A0, A1};

const int led_pin = 13;
const int buz_pin = 12;

//Motor driver pin
const int motor_pwm_pin[2] = {9, 6};
const int motor_left_pin[2] = {11, 7};
const int motor_right_pin[2] = {10, 8};

// test button
int btn_test_pin[2] = {A2, A3};

//relay grinder
const int grinder = 5;

// relay grinder condition
#define grinder_off digitalWrite(grinder, LOW);
#define grinder_on digitalWrite(grinder, HIGH);

//input and output setup
void setup_input_output() {
  lcd.init();

  pinMode(grinder, OUTPUT);

  digitalWrite(grinder, LOW);
  
  for (int i = 0; i < 3; i++) {
    pinMode(btn_pin[i], INPUT);
  }

  for (int i = 0; i < 2; i++) {
    pinMode(btn_test_pin[i], INPUT_PULLUP);
    pinMode(ls_pin[i], INPUT_PULLUP);
    pinMode(motor_pwm_pin[i], OUTPUT);
    pinMode(motor_left_pin[i], OUTPUT);
    pinMode(motor_right_pin[i], OUTPUT);
  }
  
  pinMode(led_pin, OUTPUT);
  pinMode(buz_pin, OUTPUT);
  
  for (int i = 0; i < 4; i++) {
    digitalWrite(buz_pin, !digitalRead(buz_pin));
    digitalWrite(led_pin, !digitalRead(led_pin));
    delay(30);
  }
  
  lcd.createChar(0, customChar);
  lcd.backlight();
  lcd.setCursor(0, 0), lcd.print(F(" Bricket Cutter"));
  delay(1000);
  lcd.clear();
}

// run motor function
void run_motor(int ch, int spd, int brk) {
  if (spd == 0 && brk == 1) {
    analogWrite(motor_pwm_pin[ch], 255);
    digitalWrite(motor_left_pin[ch], 0);
    digitalWrite(motor_right_pin[ch], 0);
  }
  if (spd == 0 && brk == 0) {
    analogWrite(motor_pwm_pin[ch], 0);
    digitalWrite(motor_left_pin[ch], 0);
    digitalWrite(motor_right_pin[ch], 0);
  }
  if (spd > 0) {
    analogWrite(motor_pwm_pin[ch], spd);
    digitalWrite(motor_left_pin[ch], 1);
    digitalWrite(motor_right_pin[ch], 0);
  }
  if (spd < 0) {
    analogWrite(motor_pwm_pin[ch], -spd);
    digitalWrite(motor_left_pin[ch], 0);
    digitalWrite(motor_right_pin[ch], 1);
  }
}

//button reading function
bool button(int ch) {
  return !digitalRead(btn_pin[ch]); 
}

//test button
bool btn_test(int ch) {
  return !digitalRead(btn_test_pin[ch]);
}

//top limit switch
bool ls_atas() {
  return !digitalRead(ls_pin[0]);
}

//buttom limit switch
bool ls_bawah() {
  return !digitalRead(ls_pin[1]);
}

//ls counter function
int last_state_ls = HIGH;
int ls_counter = 0;
int total_cut = 0;
void counter_cut(int pin) {
  if (!last_state_ls && pin) {
    ls_counter += 1;
    total_cut += 1;
  }
  last_state_ls = pin;
}

//beep buzzer
void beep_buz(int sum, int interval) {
  if (sum > 0) sum = sum * 2;
  if (interval >= 500) interval = 500;
  for (int i = 0; i < sum; i++) {
    digitalWrite(buz_pin, !digitalRead(buz_pin));
    delay(interval);
  }
}

//refresh lcd screen
int refresh;
void refresh_screen(int intervalRefresh) {
  if (++refresh > intervalRefresh) {
    lcd.clear();
    refresh = 0;
  }
}

