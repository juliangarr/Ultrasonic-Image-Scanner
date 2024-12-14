from flask import Flask, render_template, request, jsonify, send_file
from PIL import Image
import numpy as np
import io
import time
import serial

# Configurar puerto serial
ser = serial.Serial('/dev/ttyUSB0', 115200)
ser.reset_input_buffer()
ser.reset_output_buffer()
time.sleep(2)

app = Flask(__name__)

# Función para recibir datos del puerto serial e introducirlos en un vector (lista)
def recibir_datos(dim):
    total_bytes = dim * dim
    datos = []

    while len(datos) < total_bytes:
        if ser.in_waiting > 0:
            datos.extend(ser.read(ser.in_waiting))  # Leer los bytes disponibles en el buffer e introducirlos en la lista
    
    return datos

# Función para generar una imagen en escala de grises a partir de un vector de valores [0-255]
# Asume que el tamaño de la imagen es cuadrado o lo más cercano posible
def generate_grayscale_image(vector, width):
    # Generar matriz cuadrada a partir del vector (con padding por si no es cuadrado)
    height = len(vector) // width
    if len(vector) % width != 0:
        height += 1
    padded_vector = vector + [0] * (width * height - len(vector))  # Padding con ceros
    array = np.array(padded_vector, dtype=np.uint8).reshape((height, width))
    
    # Aplicar trasposición (espejo en el eje vertical)
    mirrored_array = np.fliplr(array)

    # Convertir la matriz en imagen
    image = Image.fromarray(mirrored_array, mode='L')
    return image

# Ruta principal de la aplicación web
@app.route('/')
def index():
    return render_template('index.html')

# Ruta para simular un escaneo y devolver una imagen en escala de grises
@app.route('/scan', methods=['POST'])
def scan():
    try:
        # Leer parámetros del cuerpo de la solicitud
        data = request.get_json()
        dimension = data.get('dimension', 10)  # Valor por defecto: 10
        threshold = data.get('threshold', 100)  # Valor por defecto: 100

        # Validar los valores recibidos
        if not (1 <= dimension <= 30):
            return jsonify({'error': 'Dimensión fuera de rango (1-30)'}), 400
        if not (1 <= threshold <= 400):
            return jsonify({'error': 'Distancia máxima fuera de rango (1-400)'}), 400

        # Enviar comando de escaneo
        ser_command = f"SYS:ESC:SCAN ,{dimension} ,{threshold}\n"
        #print(f"Enviando comando: {ser_command}")
        ser.write(ser_command.encode('utf-8'))

        # Generar datos simulados basados en los parámetros
        vector = recibir_datos(dimension)
        '''
        for i in range(dimension * threshold // 400):       # Simular datos de escaneo
            vector.append((i * 255) // (dimension * threshold // 400))
            time.sleep(0.01)  # Simular tiempo de procesamiento
        '''

        # Print de los datos recibidos en forma de matriz cuadrada (DEBUG)
        '''
        print("Datos recibidos:")
        for i in range(dimension):
            print(vector[i * dimension:(i + 1) * dimension])
        '''
        # Generar imagen en escala de grises a partir de los datos
        image = generate_grayscale_image(vector, dimension)

        # Convertir la imagen en un archivo PNG y enviarlo como respuesta
        buffer = io.BytesIO()
        image.save(buffer, format="PNG")
        buffer.seek(0)

        # Devolver la imagen en la respuesta
        return send_file(buffer, mimetype='image/png', download_name='scan.png')

    # Capturar cualquier excepción y devolver un mensaje de error
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)
