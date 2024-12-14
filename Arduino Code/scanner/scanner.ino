#include <HCSR04.h>
#include <Servo.h>
#include "Vrekrer_scpi_parser.h"

// CONSTANTES
int const GRADOS_SERVO_180 = 180;

// Definir los pines de la placa Arduino
const int triggerPin = 2;     // Pin de Trigger del HC-SR04
const int echoPin = 3;       // Pin de Echo del HC-SR04
const int servo_1_pin = 9;    // Pin del servo 1
const int servo_2_pin = 7;    // Pin del servo 2

// Variable para definir el número de píxeles de la imagen a Generar. Mínimo 1, Máximo 30 debido a las restricciones de memoria de la placa arduino
int dim = 30;

// Variables para almacenar los valores de grises calculados
//byte imagen[GRADOS_SERVO_180][GRADOS_SERVO_180];
byte imagen[30][30];

// Instrumento SCPI
SCPI_Parser scanner;

// Sección de objetos (dispositivos físicos, sensores, actuadores...)
Servo servo_1;  // Servo para el MOVIMIENTO HORIZONTAL de la plataforma
Servo servo_2;  // Servo para el MOVIMIENTO VERTICAL de la plataforma
HCSR04 hc(triggerPin, echoPin); // Variable para el sensor de ultrasonido (trig pin, echo pin)


/* --------------------------------------------------------------------- 
                SETUP
------------------------------------------------------------------------*/
void setup() {
  // Inicializar la comunicación serie para monitoreo en el Serial Monitor
  Serial.begin(115200);

  // Configuración de los Servos
  servo_1.attach(servo_1_pin);
  servo_2.attach(servo_2_pin);

  // Inicializar la matriz de pixeles a cero
  for (int i=0; i<30; i++){                    // Nota: No se usa la constante GRADOS_SERVO_180 por velocidad en la comparación (evitar acceder a memoria)
    for (int j=0; j<30; j++){
      imagen[i][j] = 0;
    }
  }

  // Configuracion SCPI
  scanner.RegisterCommand(F("*IDN?"), &Identify);  // Identificar
  scanner.SetCommandTreeBase(F("SYStem:ESC"));
  scanner.RegisterCommand(F(":SCANner"), &Scan);  // Comando SCPI para recibir orden de escanear
  scanner.RegisterCommand(F(":PRInt"), &Print);   // Comando SCPI para imprimir la imagen por Serial 
}

/* --------------------------------------------------------------------- 
                LOOP
------------------------------------------------------------------------*/
void loop() {
  // Recibir comandos
  if(Serial.available()>0){
    scanner.ProcessInput(Serial, "\n");
  }
}

/* --------------------------------------------------------------------- 
                FUNCIONES DE ARDUINO
------------------------------------------------------------------------*/
// Función para enviar la matriz escaneada por Serial
void enviarImagen(int dimension){
  // Enviar la matriz completa por serial
  for (int i = 0; i < dim; i++) {
    Serial.write(imagen[i], dimension);  // Enviar una fila completa
  }
}

// Función para realizar el escaneo del entorno con los servos y generar la matriz de escala de grises
void imageScan(int dimension = 10, float threshold = 400.0){ // Umbral por defecto de 400 cm (MAX librería)
  // Configuramos la dimension de la imagen
  dim = dimension;

  // Calculamos el incremento del angulo de forma entera cogiendo el entero por debajo de la division decimal resultante de 180º/dimension
  int paso = floor(GRADOS_SERVO_180/dimension);

  // Configuramos la posición inicial del servo para que la VISIÓN sea "CENTRADA"
  int begin = round((GRADOS_SERVO_180 - paso*(dimension-1))/2);

  // Escribir la posicion inicial de los servos
  servo_1.write(begin);
  servo_2.write(begin);

  // Esperar al movimiento de los servos
  delay(360); // Maximo 360 ms debido a la velocidad ordinaria (aprox. 2ms por grado)

  // Registrar la distancia de cada scanneo y convertirlo a un valor en [0-255]
  float medida = 0.0;
  for(int i=0; i<dimension; i++){
    for(int j=0; j<dimension; j++){
      // Medimos la distancia
      medida = hc.dist();     
      delay(120);                          // Añadimos un retardo de 60 ms para realizar la medición según recomienda la libreria

      // Calcular el tono de gris correspondiente
      
      if(medida > threshold || medida == 0.0){
        //imagen[i][j] = 0;                 // Si la medida es mayor que el threshold, el punto es muy lejano y ponemos un 0 (equuivalente a negro, ya que no le llegaría luz)
        imagen[i][j] = 255;                 // Si la medida es mayor que el threshold o un punto tan lejano que no vuelve la señal (distancia 0.0), se pone el pixel blanco (NO OBSTACULO)
      }
      else{
        imagen[i][j] = round(255*medida/threshold);  // Fórmula para llevar el intervalo [0.1, threshold] al rango de valores [0, 255]
        //imagen[i][j] = round(255 * (1 - abs(medida - threshold) / threshold));
        //imagen[i][j] = round((-255*(medida - threshold))/threshold);  // Fórmula para llevar el intervalo [0.0, threshold] al rango de valores [0, 255]
      }
      /*
      if(medida > threshold || medida < 10.0){
        imagen[i][j] = 255;
      }
      else
        imagen[i][j] = 0;
      */

      // Mover la plataforma en horizontal
      servo_1.write((begin+(j+1)*paso)%180);   // Modificamos la posición del servo (se usa i+1 ya que la primera medidición fue en "begin")
      delay(2*paso);                           // Añadimos un retardo de 2 ms por grado ya que la velocidad del servo es de 0'1s/60º a 4'8 V (5 V)
    }
    servo_2.write((begin+(i+1)*paso)%180);     // Mover Servo 2 (VERTICAL)
    delay(2*paso);                             // Añadimos un retardo de 2 ms por grado ya que la velocidad del servo es de 0'1s/60º a 4'8 V (5 V)
    
    delay(begin+(paso*dimension-1)-4*paso);    // Añadimos un delay adicional para cuando debe regresar a begin el Servo 1
  }

  enviarImagen(dimension);
}

/* --------------------------------------------------------------------- 
                DEBUGGING
------------------------------------------------------------------------*/
void printImagen(){
  /*
  if(height > 10 || width > 10)
    Serial.println("ERROR: Dimensión demasiado grande..");
  else{
    for(int i=0; i<height; i++){
      for(int j=0; j<width; j++){
        Serial.print(imagen[i][j]);
        Serial.print(" - ");
      }
      Serial.println();
    }
  }
  */
  for(int i=0; i<dim; i++){
    for(int j=0; j<dim; j++){
      Serial.print(imagen[i][j]);
      Serial.print(" - ");
    }
    Serial.println();
  }
}

/* --------------------------------------------------------------------- 
                FUNCIONES SCPI
------------------------------------------------------------------------*/
void Identify(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  interface.println(F("PROYECTO LANAI,SCPI Image Scanner Controller,#00," VREKRER_SCPI_VERSION));
}

void Scan(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  if(parameters.Size() > 1){
    int dimension = atoi(parameters[0]);
    int umbral = atoi(parameters[1]);
    imageScan(dimension, umbral);
  }
  else if(parameters.Size() > 0){
    int dimension = atoi(parameters[0]);
    int umbral = atoi(parameters[1]);
    imageScan(dimension);
  }
  else{
    imageScan();
  }
}

void Print(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  printImagen();
}


