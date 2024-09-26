void setup() {
  motor_setup();
  remote_setup();
  light_setup();
  horn_setup();
  servo_setup();
  distance_sensor_setup();
}

void loop() {
  motor_loop();
  remote_loop();
  light_loop();
  brightness_sensor_loop();  
  horn_loop();
  distance_sensor_loop();
  emergency_loop();
  automatic_driving_loop();  
}

void remote_setup() {
  Serial.begin(9600);  
}

void remote_loop() {
  check_remote_input();
  distribute_remote_input();
}

char remote_input;
bool remote_input_changed = false;

enum {
  AUTOMATIC = 'X',
  MANUAL = 'x',
}; /*AUTOMAITC DRIVING*/

bool AUTOMATIC_DRIVING_ENABLE = false;

void check_remote_input() {
  if(Serial.available()) {
    char r_data = Serial.read();    

    if(r_data == AUTOMATIC) {
      AUTOMATIC_DRIVING_ENABLE = true;
    }

    if(AUTOMATIC_DRIVING_ENABLE) {
      if(r_data == MANUAL) {
        AUTOMATIC_DRIVING_ENABLE = false;
      } else return;       
    }

    // 비상시 사용자 입력 막기
    extern bool EMERGENCY_STATE_ENABLE;
    if(EMERGENCY_STATE_ENABLE) return;
    
    if(r_data != remote_input) {
      remote_input = r_data;
      remote_input_changed = true;
    }
  }
}

enum {
  GOFORWARD = 'f',
  GOBACKWARD = 'b',
  TURNLEFT = 'l',
  TURNRIGHT = 'r',
  STOP = 's',
  GOFORWARDLEFT = 'g',
  GOFORWARDRIGHT = 'i',
  GOBACKWARDLEFT = 'h',
  GOBACKWARDRIGHT = 'j',
}; /*SERIAL*/

enum { 
  GOFORWARDBLUETOOTH = 'F',
  GOBACKWARDBLUETOOTH = 'B',
  TURNLEFTBLUETOOTH = 'L',
  TURNRIGHTBLUETOOTH = 'R',
  STOPBLUETOOTH = 'S', 
  GOFORWARDLEFTBLUETOOTH = 'G',
  GOFORWARDRIGHTBLUETOOTH = 'I',
  GOBACKWARDLEFTBLUETOOTH = 'H',
  GOBACKWARDRIGHTBLUETOOTH = 'J',
}; /*BLUETOOTH*/ 

enum {
  SPEED_0 = '0',
  SPEED_1 = '1',
  SPEED_2 = '2',
  SPEED_3 = '3',
  SPEED_4 = '4',
  SPEED_5 = '5',
  SPEED_6 = '6',
  SPEED_7 = '7',
  SPEED_8 = '8',
  SPEED_9 = '9',
  SPEED_10 = 'q',   
}; /*SPEED*/

enum {
  FRONTLIGHTON = 'W',
  FRONTLIGHTOFF = 'w',
  ALLLIGHTON = 'A',
  ALLLIGHTOFF = 'a',
  REARLEFTBLINK = 'z',
  REARRIGHTBLINK = 'c', 
}; /*LIGHT*/

enum {
  HORN = 'V',
}; /*HORN*/

char driving_action = STOP;
bool driving_action_changed = false;

char driving_speed = SPEED_0;
bool driving_speed_changed = false;

char light_onoff = ALLLIGHTOFF;
bool light_onoff_changed = false;

char light_blink;
bool light_blink_changed = false;

bool horn_state_changed = false;

void distribute_remote_input() {
  if(remote_input_changed) {
    remote_input_changed = false;

    // 대문자를 소문자로 변경
    if(remote_input == GOFORWARDBLUETOOTH ||
      remote_input == GOBACKWARDBLUETOOTH ||
      remote_input == TURNLEFTBLUETOOTH ||
      remote_input == TURNRIGHTBLUETOOTH ||
      remote_input == STOPBLUETOOTH ||
      remote_input == GOFORWARDLEFTBLUETOOTH ||
      remote_input == GOFORWARDRIGHTBLUETOOTH ||
      remote_input == GOBACKWARDLEFTBLUETOOTH ||
      remote_input == GOBACKWARDRIGHTBLUETOOTH) {
      remote_input = remote_input - 'A' + 'a';
    }

    if(remote_input == GOFORWARD ||
      remote_input == GOBACKWARD ||
      remote_input == TURNLEFT ||
      remote_input == TURNRIGHT ||
      remote_input == STOP ||
      remote_input == GOFORWARDLEFT ||
      remote_input == GOFORWARDRIGHT ||
      remote_input == GOBACKWARDLEFT ||
      remote_input == GOBACKWARDRIGHT) {
      driving_action = remote_input;
      driving_action_changed = true;        
    } else if(remote_input == SPEED_0 ||
      remote_input == SPEED_1 ||
      remote_input == SPEED_2 ||
      remote_input == SPEED_3 ||
      remote_input == SPEED_4 ||
      remote_input == SPEED_5 ||
      remote_input == SPEED_6 ||
      remote_input == SPEED_7 ||
      remote_input == SPEED_8 ||
      remote_input == SPEED_9 ||
      remote_input == SPEED_10) {
      driving_speed = remote_input;
      driving_speed_changed = true;        
    } else if(remote_input == FRONTLIGHTON ||
      remote_input == FRONTLIGHTOFF ||
      remote_input == ALLLIGHTON ||
      remote_input == ALLLIGHTOFF) {
      light_onoff = remote_input;
      light_onoff_changed = true;
    } else if(remote_input == REARLEFTBLINK ||
      remote_input == REARRIGHTBLINK) {
      light_blink = remote_input;
      light_blink_changed = true;
    } else if(remote_input == HORN) {        
        horn_state_changed = true;
    }      
  }
}

// 왼쪽 바퀴 - 전후진

const int DIRA = 4;                      // Motor A Direction (A 회전방향)
const int PWMA = 5;                      // Motor A PWM (A 속도)

// 오른쪽 바퀴 - 전후진

const int DIRB = 7;                      // Motor B Direction  (B 회전방향)
const int PWMB = 6;                      // Motor B PWM (B 속도)

void motor_setup() {
  // 왼쪽 바퀴 초기화
  pinMode(DIRA, OUTPUT);

  digitalWrite(DIRA, LOW);
  analogWrite(PWMA, 0);

  // 오른쪽 바퀴 초기화
  pinMode(DIRB, OUTPUT);

  digitalWrite(DIRB, LOW);
  analogWrite(PWMB, 0); 
}

void motor_loop() {
  process_driving_action();
  process_driving_speed();
}

void process_driving_action() {
  if(driving_action_changed) {
    driving_action_changed = false;
    
    if(driving_action == GOFORWARD) {
      go_forward();
    } else if(driving_action == GOBACKWARD) {
      go_backward();
    } else if(driving_action == TURNLEFT) {
      turn_left();
    } else if(driving_action == TURNRIGHT) {
      turn_right();
    } else if(driving_action == STOP) {
      stop_driving();
    } else if(driving_action == GOFORWARDLEFT) {
      go_forward_left();
      light_blink = REARLEFTBLINK;
      light_blink_changed = true;
    } else if(driving_action == GOFORWARDRIGHT) {
      go_forward_right();
      light_blink = REARRIGHTBLINK;
      light_blink_changed = true;
    } else if(driving_action == GOBACKWARDLEFT) {
      go_backward_left();
    } else if(driving_action == GOBACKWARDRIGHT) {
      go_backward_right();
    }
  }
}

void go_forward() {
  extern unsigned int acceleration;
  // 전진
  // 왼쪽 전진
  digitalWrite(DIRA, HIGH);  
  analogWrite(PWMA, 200+acceleration);
  // 오른쪽 전진
  digitalWrite(DIRB, HIGH);  
  analogWrite(PWMB, 200+acceleration);
}

void go_backward() {
  // 후진
  // 왼쪽 후진
  digitalWrite(DIRA, LOW);  
  analogWrite(PWMA, 200);
  // 오른쪽 후진
  digitalWrite(DIRB, LOW);  
  analogWrite(PWMB, 200);
}

void turn_left() {
  // 좌회전
  // 왼쪽 후진
  digitalWrite(DIRA, LOW);  
  analogWrite(PWMA, 200);
  // 오른쪽 전진
  digitalWrite(DIRB, HIGH);  
  analogWrite(PWMB, 200);
}

void turn_right() {
  // 우회전
  // 왼쪽 전진
  digitalWrite(DIRA, HIGH);  
  analogWrite(PWMA, 200);
  // 오른쪽 후진
  digitalWrite(DIRB, LOW);  
  analogWrite(PWMB, 200); 
}

void stop_driving() {
  // 정지
  // 왼쪽 멈춤
  digitalWrite(DIRA, LOW); 
  analogWrite(PWMA, 0);
  // 오른쪽 멈춤
  digitalWrite(DIRB, LOW); 
  analogWrite(PWMB, 0); 
  extern unsigned int acceleration;
  acceleration = 0;
}

void go_forward_left() {
  turn_left();
}

void go_forward_right() {
  turn_right();
}

void go_backward_left() {
  turn_right();
}

void go_backward_right() {
  turn_left();
}

void process_driving_speed() {
  if(driving_speed_changed) {
    driving_speed_changed = false;
   
    change_driving_speed();   
  }
}

unsigned int acceleration = 0;
void change_driving_speed() {
  int user_speed = driving_speed;
  if(user_speed != SPEED_10) user_speed -= SPEED_0;
  else user_speed = 10;
  
  acceleration = user_speed*(50/10.0);
  //Serial.println(acceleration);

  driving_action_changed = true;
}

const int front_left_pin = 2;
const int front_right_pin = 19; //A5
const int rear_left_pin = 3;
const int rear_right_pin = 18; //A4

void light_setup() {
  pinMode(front_left_pin, OUTPUT);
  pinMode(front_right_pin, OUTPUT);
  pinMode(rear_left_pin, OUTPUT);
  pinMode(rear_right_pin, OUTPUT);

  digitalWrite(front_left_pin, LOW);
  digitalWrite(front_right_pin, LOW);
  digitalWrite(rear_left_pin, LOW);
  digitalWrite(rear_right_pin, LOW);
}

void light_loop() {
  process_light_onoff();
  process_light_blink();
  process_rear_left_blink();
  process_rear_right_blink();
}

bool rear_left_blink = false;
bool rear_right_blink = false;

void process_light_onoff() {
  if(light_onoff_changed ) {
    light_onoff_changed = false;
    
    if(light_onoff == FRONTLIGHTON) {
      front_light_on();     
    } else if(light_onoff == FRONTLIGHTOFF) {
      front_light_off();

    } else if(light_onoff == ALLLIGHTON) {
      all_light_on();
    } else if(light_onoff == ALLLIGHTOFF) {
      all_light_off();
    } 
  }
}

void front_light_on() {
  digitalWrite(front_left_pin, HIGH);
  digitalWrite(front_right_pin, HIGH);
}

void front_light_off() {
  digitalWrite(front_left_pin, LOW);
  digitalWrite(front_right_pin, LOW);
}



void all_light_on() {
  front_light_on();
}
      
void all_light_off() {
  front_light_off();
} 

void process_light_blink() {  
  if(light_blink_changed ) {
    light_blink_changed = false;

    if(light_blink == REARLEFTBLINK) {
      rear_left_blink = true;
      rear_right_blink = false;      
    } else if(light_blink == REARRIGHTBLINK) {
      rear_left_blink = false;
      rear_right_blink = true;       
    }
  }
}

void process_rear_left_blink() {  
  static unsigned long prev_millis = 0;
  const unsigned long interval_millis = 500;

  static int blink_cnt = 0;
  
  if(rear_left_blink) {
    unsigned long curr_millis = millis();
    if(curr_millis - prev_millis >= interval_millis) {
      prev_millis = curr_millis;

      blink_cnt++;
      if(blink_cnt == 10) {
        blink_cnt = 0;
        rear_left_blink = false;
        light_onoff_changed = true;
      } else {
        digitalWrite(rear_left_pin, blink_cnt%2);
      }
    }    
  }  
}

void process_rear_right_blink() {
  static unsigned long prev_millis = 0;
  const unsigned long interval_millis = 500;

  static int blink_cnt = 0;
  
  if(rear_right_blink) {
    unsigned long curr_millis = millis();
    if(curr_millis - prev_millis >= interval_millis) {
      prev_millis = curr_millis;

      blink_cnt++;
      if(blink_cnt == 10) {
        blink_cnt = 0;
        rear_right_blink = false;
        light_onoff_changed = true;
      } else {
        digitalWrite(rear_right_pin, blink_cnt%2);
      }
    }    
  }    
}

void brightness_sensor_loop() {
  check_brightness_sensor_input();
  distribute_brightness_sensor_input();
}

int brightness_input;
bool brightness_input_changed = false;

void check_brightness_sensor_input() {
  static unsigned long prev_millis = 0;
  const unsigned long interval_millis = 500;

  unsigned long curr_millis = millis();
  if(curr_millis - prev_millis >= interval_millis) {
    prev_millis = curr_millis;
  
    brightness_input = analogRead(A0);
    brightness_input_changed = true;
    
  }  
}

enum {
  DIM = 300,
  BRIGHT = 700,
};

int light_onoff_by_brightness = ALLLIGHTOFF;

void distribute_brightness_sensor_input() {  
  if(brightness_input_changed) {
    brightness_input_changed = false;

    if(light_onoff_by_brightness == ALLLIGHTOFF) {
      if(brightness_input <= DIM) {
        light_onoff_by_brightness = ALLLIGHTON;
        light_onoff = ALLLIGHTON;
        light_onoff_changed = true;
      }
    } else if(light_onoff_by_brightness == ALLLIGHTON) {
      if(brightness_input >= BRIGHT) {
        light_onoff_by_brightness = ALLLIGHTOFF;
        light_onoff = ALLLIGHTOFF;
        light_onoff_changed = true;
      }
    }
  }
}

const int horn_pin = 17; // A3

void horn_setup() {
  pinMode(horn_pin, OUTPUT);
}

void horn_loop() {
  process_horn_output();
}

void process_horn_output() {
  static unsigned long prev_millis = 0;
  const unsigned long interval_millis = 1; 
  static int horn_cnt = 0; 

  if(horn_state_changed) {
    unsigned long curr_millis = millis();
    if(curr_millis - prev_millis >= interval_millis) {
      prev_millis = curr_millis;
  
      horn_cnt++;
      if(horn_cnt == 1000) {
        horn_cnt = 0;
        horn_state_changed = false;
      } else if(horn_cnt<200) {
        digitalWrite(horn_pin, horn_cnt%2);
      } else if((200 <= horn_cnt) && (horn_cnt < 300)) {
         ; // do nothing
      } else if((300<=horn_cnt) && (horn_cnt<600)) {
        digitalWrite(horn_pin, horn_cnt%2);
      } else if((600<=horn_cnt) && (horn_cnt<1000)) {
        ; // do nothing
      }
    }
  }
}

#include <Servo.h>

Servo face_direction_servo; 
const int servo_pin = 9;

void servo_setup() {
  face_direction_servo.attach(servo_pin);
  facing_front();
}

void facing_front() {
  face_direction_servo.write(90);
}

void facing_left() {
  face_direction_servo.write(150);
}

void facing_right() {
  face_direction_servo.write(30);
}

#include <PinChangeInterrupt.h>

const int trig_pin = 11;
const int echo_pin = 12;

void distance_sensor_setup() {
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  attachPCINT(digitalPinToPCINT(echo_pin), echoIsr, CHANGE);
}

void distance_sensor_loop() {
  triggering_distance_sensor();
  check_distance_sensor_input();
}

unsigned long distance_input = 60; // cm
bool distance_input_changed = false;

void echoIsr(void) {    
  static volatile unsigned long echo_begin = 0;
  static volatile unsigned long echo_end = 0;

  unsigned int echo_pin_state = digitalRead(echo_pin);
  
  if(echo_pin_state == HIGH) {
    echo_begin = micros();
  } else {
    echo_end = micros();
    unsigned long echo_duration = echo_end - echo_begin;
    distance_input = echo_duration / 58;
    distance_input_changed = true;
  }
}

void triggering_distance_sensor() {
  static unsigned long prev_millis = 0;
  const unsigned long interval_millis = 20;

  unsigned long curr_millis = millis();
  if(curr_millis - prev_millis >= interval_millis) {
    prev_millis = curr_millis;

    // 비상시 거리 센서 입력 막기
    extern bool EMERGENCY_STATE_ENABLE;
    if(EMERGENCY_STATE_ENABLE) return;

    ultrasonic_sensor_triggering();
  } 
}

void ultrasonic_sensor_triggering() {
  digitalWrite(trig_pin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trig_pin, LOW);
}

enum {
  SOMETHING_NEAR = 30, //cm
};

bool EMERGENCY_STATE_ENABLE = false;

void check_distance_sensor_input() {
  // 비상시 거리 센서 입력 막기
  extern bool EMERGENCY_STATE_ENABLE;
  if(EMERGENCY_STATE_ENABLE) return;

  if(distance_input_changed) {
    distance_input_changed = false;

    static int emergency_level = 0;
    if(distance_input <= SOMETHING_NEAR) {
      emergency_level ++;
      if(emergency_level == 7) {
        emergency_level = 0;
        


        EMERGENCY_STATE_ENABLE = true;
      }
    } else {
      if(emergency_level>0) {
        emergency_level --;
      }
    }

  }  
}

void emergency_loop() {
  static unsigned long prev_millis = 0;
  const unsigned long interval_millis = 1;
  static int process_emergency_state = 0;
  
  if(EMERGENCY_STATE_ENABLE) {
    unsigned long curr_millis = millis();
    if(curr_millis - prev_millis >= interval_millis) {
      prev_millis = curr_millis;

      process_emergency_state++;
      // 비상 상태 처리
      if(process_emergency_state == 1) {
        // 1. 정지한다.
        driving_action = STOP;
        driving_action_changed = true;
        // 2. 불을 켠다.
        light_onoff = ALLLIGHTON;
        light_onoff_changed = true;
        // 3. 경적을 울린다.
        horn_state_changed = true;
      } else if(process_emergency_state == 500) {
        // 4. 후진한다.        
        driving_action = GOBACKWARD;
        driving_action_changed = true;
      } else if(process_emergency_state == 2000) {
        // 멈춘다.
        driving_action = STOP;
        driving_action_changed = true;
        // 5. 좌우를 살핀다.
        facing_left();
      } else if(process_emergency_state == 3000) {
        ultrasonic_sensor_triggering();
      } else if(process_emergency_state == 3030) {
        check_left_distance();
      } else if(process_emergency_state == 4000) {
        facing_right();
      } else if(process_emergency_state == 5000) {
        ultrasonic_sensor_triggering();
      } else if(process_emergency_state == 5030) {
        check_right_distance();
      } else if(process_emergency_state == 6000) {
        facing_front();
        // 6. 물체가 없는 쪽으로 회전한다.
        turn_left_or_right();
      } else if(process_emergency_state == 8000) {
        // 전진한다.
        driving_action = GOFORWARD;
        driving_action_changed = true;
      } else if(process_emergency_state == 9000) {
        // 멈춘다.
        driving_action = STOP;
        driving_action_changed = true;
      } else if(process_emergency_state == 10000) {
        process_emergency_state = 0;     
        // 7. 비상 상태를 해제한다.
        EMERGENCY_STATE_ENABLE = false;
        // 8. 불을 끈다.
        light_onoff = ALLLIGHTOFF;
        light_onoff_changed = true;
      // 9. 주행 명령을 기다린다.
      }
    }
  }  
}

long left_distance = 0;
long right_distance = 0;

void check_left_distance() {
  left_distance = distance_input;
  Serial.print("#left(cm):");
  Serial.println(left_distance);
}

void check_right_distance() {
  right_distance = distance_input;
  Serial.print("right(cm):");
  Serial.println(right_distance);
}

void turn_left_or_right() {
  if(left_distance >= right_distance) {
    //좌회전
    driving_action = GOFORWARDLEFT;
    driving_action_changed = true;
  } else {
    //우회전
    driving_action = GOFORWARDRIGHT;
    driving_action_changed = true;
  }
}

void automatic_driving_loop() {
  process_automatic_driving();
}

void process_automatic_driving() {
  static unsigned long prev_millis = 0;
  const unsigned long interval_millis = 100;

  // 비상시 자동 입력 막기
  extern bool EMERGENCY_STATE_ENABLE;
  if(EMERGENCY_STATE_ENABLE) return;    
  
  if(AUTOMATIC_DRIVING_ENABLE) {
    unsigned long curr_millis = millis();
    if(curr_millis - prev_millis >= interval_millis) {
      prev_millis = curr_millis;

      driving_action = GOFORWARD;
      driving_action_changed = true;
    }
  }

}
