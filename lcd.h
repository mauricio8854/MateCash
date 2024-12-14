/**
 * @file LCD.h
 * @brief Definiciones y prototipos para el control de una pantalla LCD 20x4 mediante I2C.
 */
#ifndef LCD_H
#define LCD_H

#include "hardware/i2c.h"

/**
 * @brief Dirección I2C del módulo LCD.
 */
#define LCD_ADDRESS 0x27

/**
 * @brief Número de columnas del LCD.
 */
#define LCD_COLUMNS 20

/**
 * @brief Número de filas del LCD.
 */
#define LCD_ROWS 4

/**
 * @brief Puerto I2C utilizado para la comunicación con el LCD.
 */
#define I2C_PORT i2c1

/**
 * @brief Inicializa el LCD y el bus I2C.
 *
 * Configura el bus I2C, inicializa las resistencias pull-up y
 * realiza los pasos de configuración inicial del LCD.
 */
void initLCD();

/**
 * @brief Muestra un mensaje en una ubicación específica del LCD.
 *
 * @param message Cadena de texto que se desea mostrar.
 * @param row Fila donde se desea mostrar el mensaje (0 a 3).
 * @param col Columna inicial donde se desea mostrar el mensaje (0 a 19).
 */
void displayMessage(const char *message, int row, int col);

#endif // LCD_H