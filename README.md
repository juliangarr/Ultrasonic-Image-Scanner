# Ultrasonic-Image-Scanner
Generador de imágenes ultrasónicas con un sensor HC-SR04 y Servo Motores de Arduino. Control y visualización desde una app web generada con flask.

Este código escanea el entorno utilizando servos para mover una plataforma y un sensor de distancia para medir las distancias en una cuadrícula 360 grados. Dado un número de píxeles de dimension [1-30 px] y un umbral de distancia [1-400 cm], se dividen los 180 grados de rango de los servos para simular un escaneo equivalente a una matriz cuadrada con la correspondiente dimenision. Para cada división en el rango de los servos, se mide la distancia con el sensor ultrasónico. Los valores de distancia se convierten en tonos de gris despreciando los valores por encima del umbral, generando una representación de la escena a partir de las mediciones realizadas en diferentes ángulos. El resultado es una matriz de escala de grises que simula una imagen basada en las distancias detectadas.

## Control y descripcion de funcionamiento
Para realizar el escaneo de imágenes, acceder a la web corriendo el código de python, completar los campos requeridos y pulsar "Escanear".

## Montaje Hardware
![Diagrama del circuito montado](images/circuito.png)
**Nota:** Los servos están acoplados en una plataforma pan-and-tilt para mover el sensor de ultrasonidos en horizontal y en vertical.

## Ejecución software
1. Cargar el código `scanner.ino` en la placa de arduino
2. Para correr el código y acceder a la dirección web, ejecutar el fichero de python app.py `python app.py`
3. Acceder a la ip local mostrada por terminal

## Código adicional - Pruebas
Para poder verificar el correcto funcionamiento del sensor de ultrasonidos y el movimiento de los servo motores se incluyen los códigos `prueba_servos.ino` y `prueba_ultrasonido.ino`
