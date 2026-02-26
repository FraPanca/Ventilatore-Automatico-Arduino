#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

#define MOTOR 3
#define BUTTON 4
#define ALIMENTATORE 6

#define TEMPTHRESHOLD 27

const int analogPin = A0;

const int seriesResistor = 10000, adcMax = 1023;
const float nominalResistance = 10000, nominalTemperature = 25.0, betaCoefficient = 3950;

int analogValue, lastButtonValue = LOW;
float resistance, kelvin, temp;
unsigned long currentMillis = 0, lastMillis = 0;
bool stateButton = false, stateMotor = false;
String tempStr;


void getTemp() {
  analogValue = analogRead(analogPin);
  
  resistance = (float)seriesResistor * (adcMax / (float)analogValue - 1.0);
  kelvin = 1.0 / (1.0 / (nominalTemperature + 273.15) + (1.0 / betaCoefficient) * log(resistance / nominalResistance)); 
  temp = kelvin - 273.15;

  tempStr = String(temp, 1);
  /*Serial.print("Temperatura: ");
  Serial.print(tempStr);
  Serial.println(" °C");*/
}

void uploadLCD() {
  currentMillis = millis();

  if(currentMillis - lastMillis >= 1000) {
    lcd.setCursor(5, 1);
    lcd.print(tempStr);
    lcd.write(byte(223));
    lcd.print("C");

    lastMillis = currentMillis;
  }  
}


void setup() {
  Serial.begin(9600);

  pinMode(MOTOR, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(ALIMENTATORE, OUTPUT);

  digitalWrite(ALIMENTATORE, HIGH);
  analogWrite(MOTOR, 0);

  lcd.begin(16, 2);
  lcd.setCursor(2,0);
  lcd.print("Temperatura:");
}

void loop() {

  if(digitalRead(BUTTON) == LOW && lastButtonValue == HIGH) stateButton = !stateButton;

  getTemp();  
  uploadLCD();

  if(stateButton && !stateMotor && temp > TEMPTHRESHOLD) {
    stateMotor = true;
    analogWrite(MOTOR, 150);
  } else if(stateMotor && temp <= TEMPTHRESHOLD) {
    stateMotor = false;
    analogWrite(MOTOR, 0);
  }

  lastButtonValue = digitalRead(BUTTON);
  delay(100);
}
