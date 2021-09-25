//In: Pin2
//Out: Pin9
const byte acceleration = 60;
const byte control_interval = 10; // x0.1 (sec)

volatile unsigned int pwm_value = 0;
volatile unsigned int prev_time = 0;
unsigned int avr_pwm_value = 0;
int pre_pwm_value = 25;
unsigned int cnt = 0;
const byte OC1A_PIN = 9;
//const byte OC1B_PIN = 10;

const word PWM_FREQ_HZ = 25000; //Adjust this value to adjust the frequency
const word TCNT1_TOP = 16000000/(2*PWM_FREQ_HZ);

void setup() {
  Serial.begin(115200);

  attachInterrupt(0, rising, RISING);

  pinMode(OC1A_PIN, OUTPUT);

  // Clear Timer1 control and count registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << CS10);
  ICR1 = TCNT1_TOP;

  delay(1000);
}

void rising() {
  attachInterrupt(0, falling, FALLING);
  prev_time = micros();
}
 
void falling() {
  attachInterrupt(0, rising, RISING);
  pwm_value = micros()-prev_time;
}

void loop() {
  cnt++;
  avr_pwm_value += pwm_value;
  if (cnt%control_interval == 0) {
    if (avr_pwm_value/control_interval <= (1000000 / PWM_FREQ_HZ)) {
      int calcduty = calc_duty(avr_pwm_value/control_interval);
      if (calcduty - pre_pwm_value >= 5){ // fast up
        setPwmDuty(pre_pwm_value + 5);
      } else if (calcduty - pre_pwm_value <= -10) { // slow down
        setPwmDuty(pre_pwm_value - 10);
      } else {
        setPwmDuty(pre_pwm_value);
      }
//      setPwmDuty(calcduty); //Change this value 0-100 to adjust duty cycle
    } else {
      Serial.print("Wrong pwm value(");Serial.print(avr_pwm_value/control_interval);Serial.print(")");
      Serial.println();
      Serial.println("Default control(25)");
      setPwmDuty(25);
    }
    avr_pwm_value = 0;
  }
  delay(100);
}

byte calc_duty(unsigned int pwm_value) {
  byte get_duty = (byte)(pwm_value * 100 / (1000000 / PWM_FREQ_HZ));
  byte new_duty = get_duty * (100 + acceleration) / 100;
  
  Serial.print("OldDuty = "); Serial.print(get_duty); 
  Serial.print("\tCalcDuty = "); Serial.print(new_duty); Serial.print("\t");
  return new_duty;
}

void setPwmDuty(byte duty) {
  if (duty > 60) {
    duty = 60;
  }
  Serial.print("PreDuty = "); Serial.print(pre_pwm_value);Serial.print("\tApplyDuty = "); Serial.print(duty);
  Serial.println();
  // force speed test
//  duty = 60;
  pre_pwm_value = duty;
  OCR1A = (word) (duty*TCNT1_TOP)/100;
}


//float duty9 = 25;
//float duty10 = 75;
//
//void setup() {
//  Serial.begin(9600);
//  pinMode(9, OUTPUT);
//  pinMode(10, OUTPUT);
//
//  // 25kHz frequency
//  TCCR1A = bit(COM1A1)|bit(COM1B1);
//  TCCR1A |= bit(WGM11);
//  TCCR1B = bit(WGM12)|bit(WGM13);
//  TCCR1B |= bit(CS10);
//  ICR1 = 640;
//  
//  OCR1A = (int)((duty9 / 100) * (ICR1 + 1) - 1);
//  OCR1B = (int)((duty10 / 100) * (ICR1 + 1) - 1);
//  
//  TCNT1 = 0x0000;
//}
//
//void loop() {
//}
