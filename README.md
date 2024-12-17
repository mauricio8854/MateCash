# MateCash
Diseño de un cajero electrónico 
**Descripción:** El proyecto se basa en un cajero electrónico, el cuál contara con una interfaz grafica por medio de una Pantalla Lcd Tft 2.4 Pulgadas 240x320 Spi, la que le permitirá al usuario realizar tres tareas: consultar saldo, retirar dinero o cambiar clave, recibiendo los datos de un teclado matricial 4x4.

La programación se realizara por medio de leguaje de programación C, en la raspberry pi pico, lo que contendrá toda la lógica del proceso .

El compartimiento de los billetes solo será de tres cajas, (billetes de 50.000, 20.000 y 10.0000; para facilidad del diseño del software; los compartimientos se pretenden trabajar a presión por medio de base de resortes, que se toparan con rodillos de goma que serán accionados por tres motores paso a paso, que serán activados por el microcontrolador.

La parte superior del proyecto contará con la pantalla lcd y el teclado matricial, mientras que la parte inferior va a contar con las cajas de los billetes, los motores, la alimentación del sistema (baterías) y una bandeja para el retiro del dinero.

**Requisitos Funcionales**
1.	Consulta de saldo:
1.1. El sistema debe permitir a los usuarios consultar su saldo actual. El saldo será mostrado en la pantalla TFT.
1.2. Entrada: El usuario selecciona la opción "Consultar saldo" en el teclado 4x4.
1.3. Salida: Se muestra el saldo en la pantalla TFT.
2.	Cambio de clave:
2.1. El cajero debe permitir que los usuarios cambien su clave de acceso mediante la interfaz del teclado.
2.2. Entrada: El usuario selecciona la opción "Cambiar clave", ingresa la clave actual y la nueva clave.
2.3. Salida: Mensaje de confirmación o error en la pantalla TFT.
3.	Retiro de dinero:
3.1. El sistema debe permitir al usuario retirar una cantidad específica de dinero en múltiplos de 10.000, 20.000 y 50.000.
3.2. Entrada: El usuario selecciona "Retirar dinero", ingresa el monto deseado entre las opciones predeterminada.
3.3. Salida: El sistema acciona los motores paso a paso que controlan los rodillos para liberar la cantidad exacta de billetes desde las bandejas de billetes.
4.	Pantalla TFT para visualización:
4.1. La pantalla debe mostrar opciones de menú, resultados de operaciones y mensajes de error.
4.2. Debe responder a las entradas del usuario dentro de 2 segundos después de presionar las teclas.
5.	Teclado matricial 4x4:
5.1. El teclado debe permitir la entrada de opciones y claves. Cada tecla debe ser reconocida correctamente y generar una respuesta en pantalla.
6.	Seguridad en la entrada de datos:
6.1. El sistema debe solicitar y verificar la clave antes de permitir consultar saldo, cambiar la clave o retirar dinero.
7.	Control de stock de billetes:
7.1 El cajero debe verificar la disponibilidad de billetes antes de realizar una transacción de retiro. Si no hay suficientes billetes, debe mostrar un mensaje de error.

Escenarios de pruebas 
Consulta de saldo: Se realizarán varias pruebas introduciendo diferentes claves y validando que el saldo se muestre correctamente en la pantalla TFT.
Prueba de tiempo de respuesta de la pantalla.
Cambio de clave: Se realizarán pruebas para verificar que el sistema acepta claves válidas y muestra errores cuando la clave es incorrecta.
Retiro de dinero: Pruebas con diferentes cantidades para verificar que el sistema dispensa la cantidad correcta de billetes.
Simulación de faltante de billetes para verificar el mensaje de error.
Simulación de errores: Pruebas de desconexión durante el retiro para asegurarse de que los billetes no se pierdan.


