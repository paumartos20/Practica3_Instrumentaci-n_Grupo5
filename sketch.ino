#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Servo.h>
#include <IRremote.h>

// ----- Pines y constantes -----
#define DHTPIN 7
#define DHTTYPE DHT22
#define LDR_PIN A0
#define VIENTO_PIN A1
#define AIRE_PIN A2

#define LATCH_PIN 9
#define CLOCK_PIN 8
#define DATA_PIN 10

#define SERVO_PIN 6
#define IR_RECEIVER_PIN 2

// ----- Objetos -----
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo servoValvula;
IRrecv irrecv(IR_RECEIVER_PIN);

// ----- Variables -----
byte ledNivel = 0;
float tempUltima = 0.0;
float tempAnterior = 0.0;
String estadoActual = "Zona muerta";
String estadoAnterior = "Zona muerta";
float setpoint = 25.0;

// ----- Buffers para promedio -----
const int bufferSize = 5;
float tempBuffer[bufferSize] = {0};
float luzBuffer[bufferSize] = {0};
int bufferIndex = 0;

// ----- Funciones Auxiliares -----

// Actualiza LEDs con 74HC595
void actualizarIluminacion(byte nivel) {
  byte salida = 0;
  for (int i = 0; i < nivel; i++) {
    salida |= (1 << i);
  }
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, salida);
  digitalWrite(LATCH_PIN, HIGH);
}

// Calcula promedio móvil
float promedio(float arr[], float nuevoValor) {
  arr[bufferIndex] = nuevoValor;
  float suma = 0.0;
  for (int i = 0; i < bufferSize; i++) {
    suma += arr[i];
  }
  return suma / bufferSize;
}

// Verifica si la lectura del sensor es válida
bool checkSensor(float valor) {
  if (isnan(valor) || valor < -30 || valor > 70) {
    return false;
  } else {
    return true;
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);

  servoValvula.attach(SERVO_PIN);
  servoValvula.write(90);

  irrecv.begin(IR_RECEIVER_PIN);

  lcd.setCursor(0, 0);
  lcd.print("Monitorizacion Clima");
  delay(2000);
  lcd.clear();
}

void loop() {
  delay(2000);

  // ----- Lecturas -----
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();
  int ldrValor = analogRead(LDR_PIN);
  int vientoValor = analogRead(VIENTO_PIN);
  int aireValor = analogRead(AIRE_PIN);

  // ----- Autodiagnóstico -----
  bool sensorOk = checkSensor(temperatura);

  if (!sensorOk) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ERROR SENSOR");
    delay(2000);
    temperatura = tempUltima; // Usa último valor válido
  }

  // ----- Promedio -----
  float promedioTemp = promedio(tempBuffer, temperatura);
  float luz = map(ldrValor, 0, 1023, 0, 100);
  float promedioLuz = promedio(luzBuffer, luz);

  bufferIndex++;
  if (bufferIndex >= bufferSize) bufferIndex = 0;

  float viento = map(vientoValor, 0, 1023, 0, 150);
  float calidad = map(aireValor, 0, 1023, 0, 100);

  // ----- Control remoto IR con impresión de valor recibido -----
  if (irrecv.decode()) {
    unsigned long value = irrecv.decodedIRData.command;

    Serial.print("Botón pulsado: ");
    Serial.println(value, HEX);

    switch (value) {
      case 0x90: // Ajusta por tu valor real ↑
        setpoint += 1.0;
        Serial.println("Setpoint +1");
        break;
      case 0xE0: // Ajusta por tu valor real ↓
        setpoint -= 1.0;
        Serial.println("Setpoint -1");
        break;
      case 0xA8: // Ajusta por tu valor real OK
        setpoint = 25.0;
        Serial.println("Setpoint reiniciado");
        break;
    }
    irrecv.resume();
  }

  // ----- LEDs -----
  ledNivel = map(promedioLuz, 0, 100, 0, 8);
  actualizarIluminacion(ledNivel);

  // ----- Control Temperatura -----
  tempAnterior = tempUltima;
  tempUltima = promedioTemp;

  if (promedioTemp > setpoint + 2) {
    servoValvula.write(180);
    estadoActual = "Enfriando";
  } else if (promedioTemp < setpoint - 2) {
    servoValvula.write(0);
    estadoActual = "Calentando";
  } else {
    servoValvula.write(90);
    estadoActual = "Zona muerta";
  }

  // ----- LCD -----  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(promedioTemp, 1);
  lcd.print((char)223);
  lcd.print("C S:");
  lcd.print(setpoint, 1);
  lcd.print((char)223);

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(humedad, 0);
  lcd.print("% L:");
  lcd.print(promedioLuz, 0);
  lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("Vnt:");
  lcd.print(viento, 0);
  lcd.print("km/h A:");
  lcd.print(calidad, 0);
  lcd.print("%");

  lcd.setCursor(0, 3);
  lcd.print("Estado: ");
  lcd.print(estadoActual);

  // ----- Serial Monitor -----
  Serial.print("Temp: "); Serial.print(promedioTemp); Serial.print(" C | ");
  Serial.print("Setpoint: "); Serial.print(setpoint); Serial.print(" C | ");
  Serial.print("Hum: "); Serial.print(humedad); Serial.print(" % | ");
  Serial.print("Luz: "); Serial.print(promedioLuz); Serial.print(" % | ");
  Serial.print("Viento: "); Serial.print(viento); Serial.print(" km/h | ");
  Serial.print("Calidad: "); Serial.print(calidad); Serial.print(" % | ");
  Serial.print("LEDs: "); Serial.print(ledNivel); Serial.print(" | ");
  Serial.print("Servo: "); Serial.println(estadoActual);

  estadoAnterior = estadoActual;
}
