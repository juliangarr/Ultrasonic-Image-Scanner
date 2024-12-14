#include <HCSR04.h>

// Definir los pines para el sensor ultrasónico y el pin de salida
const int triggerPin = 2;     // Pin de Trigger del HC-SR04
const int echoPin = 3;       // Pin de Echo del HC-SR04
const int outputPin = 11;     // Pin de salida que activaremos

// Definir el umbral de distancia en centímetros
const int distanceThreshold = 50; // Umbral de 50 cm

// Variables para almacenar la distancia calculada
int distance;

HCSR04 hc(triggerPin, echoPin);// (trig pin, echo pin)

void setup() {
  // Inicializar la comunicación serie para monitoreo en el Serial Monitor
  Serial.begin(9600);

  // Configurar los pines
  pinMode(outputPin,OUTPUT);
}

void loop() {
  distance = hc.dist();

  // Imprimir la distancia en el monitor serie
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Comparar la distancia medida con el umbral
  if (distance > distanceThreshold) {
    // Si la distancia es mayor que el umbral, activar el pin de salida
    digitalWrite(outputPin, HIGH);
    Serial.println("HIGH");
  } else {
    // Si la distancia es menor o igual al umbral, desactivar el pin de salida
    digitalWrite(outputPin, LOW);
    Serial.println("LOW");
  }

  // Esperar un breve momento antes de la siguiente medición
  delay(1000);
}
