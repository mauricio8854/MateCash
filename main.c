/**
 * @file main.c
 * @brief Implementación principal de MateCash
 * 
 * Contiene la función `main` que inicializa el cajero, maneja la interfaz de entrada y
 * el flujo del programa.
 * 
 * El sistema utiliza un teclado matricial para ingresar cuentas y claves
 * 
 * @author: Nelson Mauricio García López
 *          Yeiner Alexander Martinez Barrera
 *          Pablo Francisco Muñoz Burbano
 *          Yeison Esteban Jimenez Zabala
 * @date 13/12/2024
 */

#include "tcl.h"
#include "lcd.h"

/**
 * @brief Punto de entrada principal del programa.
 *
 * Configura los subsistemas necesarios, inicializa el teclado matricial y maneja las interacciones
 * del usuario para el cajero "Matecash".
 *
 * @return int Valor de salida del programa.
 */
int main() {
    printf("Cajero Matecash\n");
    printf("Ingrese ID de 6 dígitos:\n");
    stdio_init_all();           /**< Inicializa el subsistema */
    initLCD();
    displayMessage("MATECASH",2,0);
    init_keypad();                   /**< Inicializa el teclado matricial y configura los pines GPIO correspondientes */
    last_key_time = get_absolute_time();  /**< Registra el tiempo de la última tecla presionada */
    input_start_time = get_absolute_time();   /**< Registra el tiempo de inicio del input */
    

    
    while (true) {
    
    if (key_pressed) {
            process_key(last_key);   /**< Procesa la última tecla presionada */
            key_pressed = false;     /**< Reinicia la bandera de tecla presionada */
        }
        
    if (current_state == STATE_ENTER_PASSWORD &&
        absolute_time_diff_us(input_start_time, get_absolute_time()) > (MAX_INPUT_TIME_MS * 1000)) {
        handle_timeout(); /**< Maneja el tiempo límite */
    }

        sleep_ms(500);          /**< retraso de 500 ms  */
    }
    return 0;
}
