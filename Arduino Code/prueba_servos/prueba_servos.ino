#include <Servo.h>

const int servo_1_pin = 9;    // Pin del servo 1 (horizontal)
const int servo_2_pin = 7;    // Pin del servo 2 (vertical)

// Sección de objetos (dispositivos físicos, sensores, actuadores...)
Servo servo_1;  // Servo para el MOVIMIENTO HORIZONTAL de la plataforma
Servo servo_2;  // Servo para el MOVIMIENTO VERTICAL de la plataforma
byte angle_1, angle_2;

void setup() {
  // Inicializar la comunicación serie para monitoreo en el Serial Monitor
  Serial.begin(9600);

  // Configuración de los Servos
  servo_1.attach(servo_1_pin);
  servo_2.attach(servo_2_pin);

  // Mover los servos a una posición inicial
  servo_1.write(90); // Coloca el servo 1 (horizontal) en el medio
  servo_2.write(90); // Coloca el servo 2 (vertical) en el medio
}

void loop() {
  // Recibir comandos a través del puerto serie
  if (Serial.available() > 0) {
    char command = Serial.read();  // Lee el comando

    // Ignorar caracteres de nueva línea o retorno de carro
    if (command == '\n' || command == '\r') {
      return;
    }

    if (command == 'h') {  // Si el comando es 'h', controlar el servo 1
      angle_1 = Serial.parseInt();  // Lee un valor numérico
      angle_1 = constrain(angle_1, 0, 180);  // Asegura que esté dentro del rango
      servo_1.write(angle_1);
    }
    else if (command == 'v') {  // Si el comando es 'v', controlar el servo 2
      angle_2 = Serial.parseInt();  // Lee un valor numérico
      angle_2 = constrain(angle_2, 0, 180);  // Asegura que esté dentro del rango
      servo_2.write(angle_2);
    }
  }
}
