#include <Servo.h>
#include <DHT.h>

// Definirea pinilor pentru semaforul mașinilor
int redCar = 13;     // LED roșu pentru mașini
int yellowCar = 12;  // LED galben pentru mașini
int greenCar = 11;   // LED verde pentru mașini

// Definirea pinilor pentru semaforul pietonilor
int redPedestrian = 3;  // LED roșu pentru pietoni
int greenPedestrian = 2; // LED verde pentru pietoni

// Definirea pinului pentru motor
int motorPin = 8; // Pin PWM pentru controlul vitezei motorului DC
int servoPin = 9; // Pinul la care este conectat servomotorul

Servo barieraServo;  // Creează un obiect servo pentru a controla bariera

// Definirea pinului pentru senzorul de umiditate
int humiditySensorPin = 7;
#define DHTTYPE DHT11
DHT dht(humiditySensorPin, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("Test DHTxx!");
  dht.begin();

  pinMode(redCar, OUTPUT);
  pinMode(yellowCar, OUTPUT);
  pinMode(greenCar, OUTPUT);
  
  pinMode(redPedestrian, OUTPUT);
  pinMode(greenPedestrian, OUTPUT);

  pinMode(motorPin, OUTPUT); // Setează pinul motorului ca ieșire

  barieraServo.attach(servoPin); // Atașează servomotorul la pinul corespunzător
}

void loop() {
  // Așteaptă câteva secunde între măsurători
  delay(2000);

  // Citirea umidității și temperaturii durează aproximativ 250 milisecunde
  float h = dht.readHumidity();
  // Citește temperatura în grade Celsius (implicit)
  float t = dht.readTemperature();
  // Citește temperatura în grade Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Verifică dacă citirile au eșuat și iese imediat (pentru a încerca din nou)
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Eroare la citirea de la senzorul DHT!");
    return;
  }
  // Calculează indexul de căldură în grade Celsius
  float hic = dht.computeHeatIndex(t, h, false);
  // Afișează rezultatele pe monitorul serial
  Serial.print("Umiditate: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatură: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Index de căldură: ");
  Serial.print(hic);
  Serial.println(" *C");

  // Definește durata semaforului verde pentru pietoni
  int greenPedestrianDuration = 8000; // 8 secunde
  
  // Dacă umiditatea este ridicată, extinde durata
  if (h > 80.0) { // Prag pentru umiditate ridicată (80%)
    greenPedestrianDuration = 14000; // 14 secunde
  }

  // Fază roșu pentru mașini, verde pentru pietoni
  digitalWrite(redCar, HIGH);
  digitalWrite(greenCar, LOW);
  digitalWrite(yellowCar, LOW);
  digitalWrite(redPedestrian, LOW);
  digitalWrite(greenPedestrian, HIGH);
  barieraServo.write(0); // Bariera coborata (presupunem că 0 grade este poziția inchisa)
  
  digitalWrite(motorPin, HIGH); // Pornire motor
  delay(greenPedestrianDuration);  // Verde la pietoni pentru durata definită
  
  // Începe palparea verde pentru pietoni
  for (int i = 0; i < 5; i++) {
    digitalWrite(greenPedestrian, LOW);
    delay(500);
    digitalWrite(greenPedestrian, HIGH);
    delay(500);
  }

  digitalWrite(motorPin, LOW); // Oprire motor
  
  // Fază verde pentru mașini, roșu pentru pietoni
  digitalWrite(redCar, LOW);
  digitalWrite(greenCar, HIGH);
  digitalWrite(yellowCar, LOW);
  digitalWrite(redPedestrian, HIGH);
  digitalWrite(greenPedestrian, LOW);
  
  barieraServo.write(90); // Ridica bariera (presupunem că 90 grade este poziția deschisa)
  delay(8000);  // Verde la mașini pentru 8 secunde
  
  // Fază galben pentru mașini, roșu pentru pietoni (repetare ciclu)
  digitalWrite(redCar, LOW);
  digitalWrite(greenCar, LOW);
  digitalWrite(yellowCar, HIGH);
  digitalWrite(redPedestrian, HIGH);
  digitalWrite(greenPedestrian, LOW);
  
  delay(2000);  // Galben la mașini pentru 2 secunde
}