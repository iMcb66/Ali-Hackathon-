#include <virtuabotixRTC.h>
#include <SoftwareSerial.h>
#include<TinyGPS++.h>
#include <SoftwareSerial.h>



SoftwareSerial SerialComSim800(8, 7 ); // RX -> 11 , TX -> 12
SoftwareSerial ss(4,3);
TinyGPSPlus gps;
void print_fecha(void);

// SCLK -> 6, I/O -> 7, CE -> 8
// CLK -> 6 , DAT -> 7, Reset -> 8
virtuabotixRTC myRTC(6, 7, 8);



int sensorPin = A0;
int ledPin = 13;
double sensorValue = 0;
int umbral_inf = 170;
int umbral = 220;
int flag = 0;
void setup() {
  pinMode(ledPin, OUTPUT);
  ss.begin(9600);
  Serial.begin(9600);
  // Formato para la fecha:
  // segundos, minutos, horas, día de la semana, día del mes, mes, año
   myRTC.setDS1302Time(00, 24, 23, 6, 14, 5, 2022); //Configuración del tiempo actual
  // Recuerda comentar la línea anterior, una vez configurado
}

void loop() {
  Serial.println("Iniciando..."); 
  delay(2000);

  SerialComSim800.println("AT"); //check OK
  updateSerial();
  myRTC.updateTime();
  if(myRTC.hours>5 && myRTC.hours<19){
    digitalWrite(ledPin, HIGH);
    Serial.println(myRTC.hours);
    delay(1000);
  }
  else{
  sensorValue = analogRead(sensorPin);
  if (sensorValue < umbral_inf) {
    //digitalWrite(ledPin,HIGH);
    digitalWrite(ledPin, LOW);
  }
  if (sensorValue > umbral) {
    //digitalWrite(ledPin,HIGH);
    digitalWrite(ledPin, LOW);
    flag = 1;
      Serial.begin(9600);
        SerialComSim800.begin(9600);
      Serial.println("Iniciando..."); 
      delay(2000);
      SerialComSim800.println("AT+CMGF=1"); // Formato SMS texto 
      updateSerial();
      SerialComSim800.println("AT+CMGS=\"+529511236213\""); // Envía el sms al número especificado
      updateSerial();
      SerialComSim800.print("El familiar no esta en condiciones de conducir debido a que cuenta con grandes cantidades de alcohol, su locazacion es la siguiente:");
      updateSerial();
      SerialComSim800.write(26);  // Convencion ^Z -> 'Ctrl+z' -> 26DEC (1AHEX)  ASCII -> SUB.
      while(ss.available() > 0){
      gps.encode(ss.read());
      if(gps.location.isUpdated()){
    //condicional que ayuda a verificar si los datos estan actualizados
    Serial.print("Longitud: ");
    Serial.print(gps.location.lng(),10);
    Serial.print("Latitud: ");
    Serial.println(gps.location.lat(),10);
  }
  }

  }
  if (flag == 1) {
    if (sensorValue < umbral_inf) {
      flag = 0;
    }
  }
  if (sensorValue > umbral_inf && sensorValue < umbral && flag == 0) {
    //digitalWrite(ledPin,LOW);
    digitalWrite(ledPin, HIGH);
  }
  Serial.println(sensorValue);
  print_fecha();
  delay(700);
  }
}

void print_fecha(void){
  Serial.print("Fecha y hora actual: ");
  Serial.print(myRTC.dayofmonth); // Se puede cambiar entre día y mes si se utiliza el sistema Americano
  Serial.print("/");
  Serial.print(myRTC.month);
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print(" ");
  Serial.print(myRTC.hours);
  Serial.print(":");
  Serial.print(myRTC.minutes);
  }
