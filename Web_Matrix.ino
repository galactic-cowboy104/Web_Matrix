#include <WiFi.h>                // Librería para conectividad WiFi en el ESP32.
#include "esp_http_server.h"     // Librería para manejar un servidor HTTP.
#include <SPI.h>                 // Librería para comunicación SPI.
#include "LedMatrix.h"           // Librería para controlar matrices de LEDs basadas en controladores MAX7219.

// Definimos constantes para la configuración del hardware.
#define NUMBER_OF_DEVICES 1      // Número de matrices de LED conectadas.
#define CS_PIN 5                 // Pin Chip Select para SPI.
#define CLK_PIN 18               // Pin de reloj SPI.
#define MISO_PIN 19              // Pin MISO (aunque no se usa en este caso).
#define MOSI_PIN 23              // Pin MOSI para transmisión de datos SPI.

// Inicializamos la matriz de LEDs con los pines configurados.
LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CLK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);

// Array para almacenar el patrón que se dibujará en la matriz de LEDs.
uint8_t pattern[8] = {0}; 

// Credenciales de la red WiFi (se deben completar).
const char* ssid = "";           // Nombre de la red WiFi.
const char* password = "";       // Contraseña de la red WiFi.

// Página HTML que se servirá a los clientes para interactuar con la matriz.
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
  <!DOCTYPE html>
  <html lang="es-MX">

    <head>

      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Control Matriz LED</title>

      <style>

        * {
          -webkit-user-select: none;
          -moz-user-select: none;
          -ms-user-select: none;
          user-select: none;
        }

        h1 {
          color: #ADFAE7;
          font-family: Arial, Helvetica, sans-serif;
          font-size: 40px;
          font-weight: 700;
        }

        body {
          background-color: #131B18;
          height: 100vh;
          width: 100vw;
          margin: 0;
          display: flex;
          align-items: center;
          justify-content: center;
          flex-direction: column;
          touch-action: none;
          overflow: hidden;
        }

        html, body {
          touch-action: manipulation;
          -ms-touch-action: manipulation;
        }

        .matrix-container {
          display: flex;
          align-items: center;
          justify-content: center;
        }

        table {
          margin: 15px;
        }

        .led {
          display: inline-block;
          width: 42px;
          height: 42px;
          border-radius: 50%;
          background: linear-gradient(0deg, rgb(211, 150, 150) 0%, rgb(179, 100, 100) 100%);
          cursor: pointer;
          touch-action: none;
          user-select: none;
        }

        .led:after {
          display: block;
          content: "";
          width: 26px;
          height: 18px;
          border-radius: 100%;
          margin: 3px auto;
          background: linear-gradient(0deg, rgba(255, 255, 255, 0.2) 0%, rgba(255, 255, 255, 0.8) 100%);
        }

        .led.active {
          background: linear-gradient(0deg, rgb(207, 58, 58) 0%, rgb(230, 32, 32) 100%);
        }

        .button {
          width: 120px;
          height: 50px;
          font-size: x-large;
          font-weight: bolder;
          text-align: center;
          margin: 5px;
          touch-action: none;
          user-select: none;
        }

        @media (max-width: 900px) {
          meta[name="viewport"] {
            content: "width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no";
          }
        }

      </style>

    </head>

    <body>

      <h1>Control Matriz LED</h1>

      <div class="matrix-container" id="matrix"></div>
      <div class="inputs-container">
        <button class="button clear-button" onclick="clearLeds()">Limpiar</button>
      </div>

      <script>

        const rows = 8;
        const cols = 8;

        window.onload = function() {
          
          const matrixContainer = document.getElementById('matrix');
          const table = document.createElement('table');

          for (let i = 0; i < rows; i++) {
              
              const row = document.createElement('tr');

              for (let j = 0; j < cols; j++) {

                  const cell = document.createElement('td');
                  const led = document.createElement('div');

                  led.className = 'led';
                  led.addEventListener('mousedown', toggleLed);
                  led.addEventListener('touchstart', toggleLed);

                  cell.appendChild(led);
                  row.appendChild(cell);

              }

              table.appendChild(row);

          }

          matrixContainer.appendChild(table);
      
        };

        function toggleLed(e) {
          e.preventDefault();
          this.classList.toggle('active');
          getPattern();
        }

        function clearLeds() {
          const leds = document.querySelectorAll('.led');
          leds.forEach(led => led.classList.remove('active'));
          getPattern();
        }

        function getPattern() {

          let hexArray = [];

          for (let col = 0; col < cols; col++) {

            let binValue = 0;

            for (let row = 0; row < rows; row++) {
              const led = document.querySelectorAll('tr')[row].children[col].firstElementChild;
              if (led.classList.contains('active')) {
                  binValue |= (1 << row);
              }
            }

            hexArray.push(binValue.toString(16).toUpperCase().padStart(2, '0'));

          }

          const compactString = hexArray.join("");
          sendMatrix(compactString);

        }

        function sendMatrix(matrix) {

          const encodedMatrix = encodeURIComponent(matrix);

          var xhr = new XMLHttpRequest();
          xhr.open("GET", "/action?display=" + encodedMatrix, true);
          xhr.send();

        }

      </script>

    </body>

  </html>
)rawliteral";

// Dibuja el patrón en la matriz de LEDs.
void drawPattern(int matrixIndex, const uint8_t pattern[8]) {
  int offset = matrixIndex * 8;   // Calcula el desplazamiento según el índice de la matriz.
  for (int col = 0; col < 8; col++) {
    ledMatrix.setColumn(offset + col, pattern[col]); // Envía los datos columna por columna.
  }
}

// Maneja la solicitud GET para la página principal.
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");            // Establece el tipo de contenido como HTML.
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML)); // Envía el HTML.
}

// Maneja comandos enviados al servidor, como patrones para la matriz.
static esp_err_t cmd_handler(httpd_req_t *req) {
  char variable[32] = {0};                          // Buffer para almacenar el comando recibido.

  // Verifica si se envió un comando con el prefijo "display=".
  if (httpd_req_get_url_query_str(req, variable, sizeof(variable)) == ESP_OK) {
    if (String(variable).startsWith("display=")) {
      String matrix = String(variable).substring(8); // Obtiene el valor del patrón en formato hexadecimal.
      for (int i = 0; i < 8; i++) {
        String hexPair = matrix.substring(i * 2, i * 2 + 2); // Extrae cada par de caracteres hexadecimales.
        pattern[i] = strtol(hexPair.c_str(), NULL, 16);      // Convierte el par hexadecimal a un valor entero.
      }
      ledMatrix.clear();                  // Limpia la matriz de LEDs.
      drawPattern(0, pattern);            // Dibuja el nuevo patrón.
      ledMatrix.commit();                 // Actualiza la matriz.
    }
  }

  // Permite el acceso desde cualquier origen (CORS).
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);  // Responde al cliente sin enviar contenido adicional.
}

// Configura y arranca el servidor HTTP.
void startServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();  // Configuración por defecto del servidor.
  httpd_handle_t server = NULL;

  // Define la URI para la página principal.
  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };

  // Define la URI para manejar comandos.
  httpd_uri_t cmd_uri = {
    .uri       = "/action",
    .method    = HTTP_GET,
    .handler   = cmd_handler,
    .user_ctx  = NULL
  };

  // Inicia el servidor y registra las URIs.
  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &cmd_uri);
  }
}

// Configuración inicial del ESP32.
void setup() {
  Serial.begin(115200);              // Inicializa la comunicación serial para depuración.

  // Conexión a la red WiFi.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);                      // Espera hasta que se establezca la conexión.
  }
  Serial.print("Dirección IP: ");    // Imprime la dirección IP asignada al ESP32.
  Serial.println(WiFi.localIP());

  startServer();                     // Inicia el servidor HTTP.

  ledMatrix.init();                  // Inicializa la matriz de LEDs.
  drawPattern(0, pattern);           // Dibuja el patrón inicial (vacío).
  ledMatrix.commit();                // Actualiza la matriz.
}

// Bucle principal (vacío, ya que el ESP32 actúa como servidor).
void loop() {}
