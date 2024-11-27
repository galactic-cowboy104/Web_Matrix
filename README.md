# Control de Matriz LED MAX7219 mediante Servidor HTTP en ESP32

Este repositorio contiene un proyecto para controlar una matriz LED basada en MAX7219 utilizando un servidor HTTP en una ESP32. Al acceder a la página web servida por la ESP32, los usuarios pueden interactuar directamente con la matriz LED para encender y apagar luces en tiempo real.

## Materiales Necesarios
- **ESP32**: Microcontrolador utilizado para ejecutar el servidor y controlar la matriz LED.
- **Matriz LED MAX7219**: Utilizada para mostrar los patrones definidos por el usuario.
- **Cables de conexión**
- **Fuente de alimentación adecuada**
- **Conexión WiFi**: Para que la ESP32 se conecte y sirva la página web interactiva.

## Contenido del Repositorio
1. **Código del Servidor ESP32**: Se proporciona el código necesario para configurar el servidor HTTP en la ESP32, controlar la matriz LED y servir la página web al cliente.
2. **Página HTML Interactiva**: Contiene la interfaz de usuario para encender y apagar LEDs en la matriz desde cualquier navegador web conectado.
3. **Documentación y Diagramas**: Instrucciones detalladas para ensamblar el proyecto y conectar los componentes correctamente.

## Instrucciones de Uso

### Conexiones Físicas:
1. Conecta la matriz LED MAX7219 a la ESP32 utilizando los pines especificados:
   - **CS_PIN**: GPIO 5 (Chip Select).
   - **CLK_PIN**: GPIO 18 (Clock).
   - **MOSI_PIN**: GPIO 23 (Datos).
2. Alimenta la matriz LED y la ESP32 con una fuente adecuada.
3. Configura las credenciales de tu red WiFi en el código (`ssid` y `password`).

### Cargar el Programa en la ESP32:
1. Abre el archivo `.ino` en el IDE de Arduino.
2. Selecciona la placa **ESP32** y el puerto correspondiente en el IDE.
3. Sube el programa a la ESP32.

### Control de la Matriz LED:
1. Una vez cargado el programa, la ESP32 se conectará a la red WiFi.
2. Obtén la dirección IP mostrada en el monitor serie.
3. Ingresa la dirección IP en un navegador web conectado a la misma red.
4. Usa la interfaz para encender, apagar y personalizar el patrón de la matriz LED.

## Consejos Adicionales
- **Modificaciones de Hardware**: Si utilizas más de una matriz MAX7219, ajusta la constante `NUMBER_OF_DEVICES` en el código.
- **Personalización del Patrón**: Modifica la lógica en la función `drawPattern` para crear patrones dinámicos.
- **Optimización**: Revisa la conexión WiFi para asegurar estabilidad, especialmente si planeas usar múltiples dispositivos conectados simultáneamente.

## Disfruta y Experimenta
Este proyecto es ideal para aprender sobre:
- Control de hardware mediante WiFi.
- Manejo de servidores HTTP en microcontroladores.
- Interacción en tiempo real con dispositivos embebidos.

¡Diviértete creando patrones únicos y explorando las posibilidades de la ESP32 y la matriz LED MAX7219!

¡Buena suerte y que disfrutes controlando tu matriz LED!
