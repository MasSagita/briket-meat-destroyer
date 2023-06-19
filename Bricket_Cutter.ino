#include <EEPROM.h>

#include "input_output.h"

void (* resetFunc) (void) = 0;

int last_counter_cut;

//dummy variable
int con = 0, cut = 1;

//speed motor 1 and 2
int con_speed, cut_speed;

//delay
int cut_delay;
int set_delay;

void setup() {
  // put your setup code here, to run once:
  cut_speed = EEPROM.read(0);
  con_speed = EEPROM.read(1);
  set_delay = EEPROM.read(2);

  Serial.begin(115200);

  setup_input_output();

  Serial.println("Bricket Cutter");

  standby();
}

int cutter_state;

int cutter_test;

unsigned long prev_millis_con;

int start_grinder;

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led_pin, !digitalRead(led_pin));
  refresh_screen(5);
  run_system();
  //counter cut 
  last_counter_cut = total_cut;
}

void repost_motor_cutter() {
  while (!ls_atas()) {
    run_motor(cut, -cut_speed, 1);
    Serial.print("Repost Cutter ");
    Serial.println(cut_speed);
    if (ls_atas()) {
      run_motor(cut, 0, 1);
      break;
    }
  }
}

unsigned long current_millis_con;

void run_system() {
  current_millis_con = millis();

  Serial.println(current_millis_con - prev_millis_con);
  lcd.setCursor(0, 0), lcd.print(F(" Extract Briket"));
  lcd.setCursor(0, 1), lcd.print(current_millis_con - prev_millis_con), lcd.print(F("ms"));
  lcd.setCursor(8, 1), lcd.print(total_cut);
  
  if ((cutter_state == 0) && current_millis_con - prev_millis_con >= cut_delay) {
    Serial.println("Cut Briket");
    run_motor(con, 0, 1), grinder_off;
    run_cutter();
    cutter_test = 1; // test menu
    cutter_state = 1;
    //prev_millis_con = current_millis_con;
  }
  else if (cutter_state == 1) {
    if (button(2)) standby();
    cutter_test = 0;
    Serial.println("Extract Briket");
    grinder_on;
    run_motor(con, con_speed, 1);
    cutter_state = 0;
    
    prev_millis_con = current_millis_con;
  }
}

void run_cutter() {
  run_motor(cut, cut_speed, 1);
  beep_buz(1, 50);
  lcd.clear();
  while (1) {
    counter_cut(ls_bawah());
    lcd.setCursor(0, 0), lcd.print(F("    Cutting"));
    if (ls_bawah()) run_motor(cut, -cut_speed, 1);
    if (ls_atas()) {
      lcd.clear();
      lcd.setCursor(0, 0), lcd.print(F("  Done Cutting"));
      run_motor(cut, 0, 1);
      beep_buz(1, 20);
      break;
    }
  }
}

int set_num = 0;

void standby() {
  lcd.clear();
  lcd.setCursor(0, 0), lcd.print(F("     Repost"));
  lcd.setCursor(0, 1), lcd.print(F("  Motor Cutter"));
  repost_motor_cutter();
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0), lcd.print(F("    Standby!"));
  Serial.println("Standby");
  beep_buz(1, 500);
  set_num = 0;
  while (1) {
    refresh_screen(5);
    if (button(2)) set_num += 1, beep_buz(1, 50);
    if (set_num > 4) set_num = 0;

    if (set_num == 0) {
      //change speed motor
      if (button(0)) set_delay += 1, beep_buz(1, 25);
      if (button(1)) set_delay -= 1, beep_buz(1, 25);

      set_delay = limitValue(set_delay, 0, 70);

      cut_delay = set_delay * 100;

      if (btn_test(0)) {
        lcd.clear();
        beep_buz(2, 100);
        cutter_state = HIGH;
        cutter_test = 0;
        while (1) {
          run_system();
          if (cutter_test) {
            lcd.clear(), beep_buz(1, 300);
            break;
          }
        }
      }
      lcd.setCursor(0, 1), lcd.write(0); 
      lcd.setCursor(1, 0), lcd.print(F("Delay Cutter"));
      lcd.setCursor(1, 1), lcd.print(cut_delay), lcd.print(F("ms"));
    }

    if (set_num == 1) {
      lcd.setCursor(1, 0), lcd.print(F("Done?"));      
      lcd.setCursor(9, 0), lcd.print(F("TC:")), lcd.print(last_counter_cut);
      lcd.setCursor(0, button(0)), lcd.write(0);
      if (button(0)) {
        lcd.clear();
        lcd.setCursor(0, 1), lcd.print(F("    Running"));
        total_cut = last_counter_cut;
        beep_buz(4, 55);
        cutter_state = HIGH;
        break;
      }
    }

    if (set_num == 2) {
      //change speed motor
      if (button(0)) cut_speed += 1, beep_buz(1, 25);
      if (button(1)) cut_speed -= 1, beep_buz(1, 25);
      //pwm motor limitation
      cut_speed = limitValue(cut_speed, 60, 250);

      if (btn_test(0)) {
        run_cutter();
      }
      lcd.setCursor(0, 1), lcd.write(0);
      lcd.setCursor(1, 0), lcd.print(F("Spd Cut "));
      lcd.setCursor(1, 1), lcd.print(cut_speed), lcd.print(F("pwm"));
    }

    if (set_num == 3) {
      //change speed motor
      if (button(0)) con_speed += 1, beep_buz(1, 25);
      if (button(1)) con_speed -= 1, beep_buz(1, 25);
      //pwm motor limitation
      con_speed = limitValue(con_speed, 50, 250);

      if (btn_test(0)) {
        run_motor(con, con_speed, 1), grinder_on;
      }
      else run_motor(con, 0, 1), grinder_off;
      lcd.setCursor(0, 1), lcd.write(0);
      lcd.setCursor(1, 0), lcd.print(F("Spd Con ")), lcd.print(btn_test(0));
      lcd.setCursor(1, 1), lcd.print(con_speed), lcd.print(F("pwm"));
    }

    if (set_num == 4) {
      lcd.setCursor(1, 0), lcd.print(F("Save?"));
      lcd.setCursor(0, button(0)), lcd.write(0);
      if (button(0)) save();
    }
    lcd.setCursor(13, 1), lcd.print(ls_atas());
    lcd.setCursor(15, 1), lcd.print(ls_bawah());
  }
}

// Save and auto reset software
void save() {
  beep_buz(2, 400);
  delay(500), lcd.clear();
  lcd.setCursor(0, 0), lcd.print("  Save & Rstrt");
  EEPROM.write(0, cut_speed);
  EEPROM.write(1, con_speed);
  EEPROM.write(2, set_delay);
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 1), lcd.print(F("."));
    delay(50);
  }
  resetFunc(); // reset microcontroller
}

int limitValue(int value, int minValue, int maxValue) {
  if (value < minValue) {
    value = minValue;
  } else if (value > maxValue) {
    value = maxValue;
  }
  return value;
}


