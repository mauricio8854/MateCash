/**
 * @file lcd.c
 * @brief Implementación de funciones para controlar una pantalla LCD 20x4 a través de I2C.
 */

#include "LCD.h"
#include "pico/stdlib.h"

/**
 * @brief Envía un byte al LCD (como comando o dato).
 *
 * @param data Byte a enviar.
 * @param is_data Si es true, se interpreta como un dato; si es false, como un comando.
 */
void lcd_write_byte(uint8_t data, bool is_data) {
    uint8_t byte_high = data & 0xF0;            // Parte alta
    uint8_t byte_low = (data << 4) & 0xF0;      // Parte baja

    uint8_t control_bits = is_data ? 0x01 : 0x00; // RS=1 para datos, RS=0 para comandos
    uint8_t enable_bit = 0x04;                   // Habilitar pulsos para Enable (E)

    uint8_t backlight_bit = 0x08; // Bit que activa la retroiluminación.
    uint8_t buffer[4] = {
    byte_high | control_bits | enable_bit | backlight_bit,
    byte_high | control_bits | backlight_bit,
    byte_low | control_bits | enable_bit | backlight_bit,
    byte_low | control_bits | backlight_bit              // Parte baja con E desactivado
    };

    i2c_write_blocking(I2C_PORT, LCD_ADDRESS, buffer, 4, false);
}

/**
 * @brief Inicializa la pantalla LCD.
 *
 * Configura la pantalla en modo 4 bits, establece las opciones iniciales
 * como encender el display, desactivar el cursor y limpiar la pantalla.
 */
void lcd_init() {
    sleep_ms(50);
    for (int i = 0; i < 3; i++) {    // Tres comandos de inicialización como dice la hoja de datos
        lcd_write_byte(0x30, false); 
        sleep_ms(5);
    }
    lcd_write_byte(0x20, false);  // Cambiar a modo 4 bits
    sleep_ms(10);

    lcd_write_byte(0x28, false);  // Configuración: 4 bits, 2 líneas, 5x8 caracteres
    lcd_write_byte(0x0C, false);  // Display ON, cursor OFF
    lcd_write_byte(0x06, false);  // Incremento automático, sin desplazamiento
    lcd_write_byte(0x01, false);  // Limpiar pantalla
    sleep_ms(10);
}

/**
 * @brief Configura e inicializa el bus I2C y la pantalla LCD.
 *
 * Establece las funciones GPIO para SDA y SCL, activa las resistencias pull-up
 * internas y llama a la función de inicialización del LCD.
 */
void initLCD() {
    i2c_init(I2C_PORT, 100 * 1000);                 // Inicializa I2C a 100kHz
    gpio_set_function(14, GPIO_FUNC_I2C);          // SDA en GPIO14
    gpio_set_function(15, GPIO_FUNC_I2C);          // SCL en GPIO15
    gpio_pull_up(14);                              // activo resistencias internas
    gpio_pull_up(15);

    lcd_init();
}
/**
 * @brief Muestra un mensaje en el LCD en una ubicación específica.
 *
 * @param message Cadena de texto a mostrar.
 * @param row Fila del LCD (0 a 3).
 * @param col Columna del LCD (0 a 19).
 */
void displayMessage(const char *message, int row, int col) {
    // Calcular dirección de memoria para la fila
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_write_byte(0x80 | (row_offsets[row] + col), false);

    while (*message) {
        lcd_write_byte(*message++, true);
    }
}