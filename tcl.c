/**
 * @file tcl.c
 * @brief Implementación de las funcionalidades del sistema "MateCash".
 *
 * Este archivo contiene las funciones principales y auxiliares para manejar
 * el sistema del cajero automático "MateCash". Incluye la lógica para el teclado
 * matricial, gestión de usuarios, consulta de saldo, cambio de contraseña
 * y retiro de dinero.
 */
#include "tcl.h"
#include"pwm.h"
#include "lcd.h"

/**
 * @brief Pines correspondientes a las filas del teclado matricial.
 */
const uint8_t ROW_PINS[4] = {2, 3, 4, 5};

/**
 * @brief Pines correspondientes a las columnas del teclado matricial.
 */
const uint8_t COL_PINS[4] = {6, 7, 8, 9};

/**
 * @brief Mapa de teclas del teclado matricial.
 */
const char KEYPAD[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

/**
 * @brief Estructura que define los usuarios del sistema.
 * 
 * Incluye ID, contraseña, nombre, intentos fallidos y estado de bloqueo.
 */
User users[NUM_USERS] = {
    {"123456", "1234", "Juan Pérez", 220000, 0, false},
    {"234567", "2345", "María García", 350000, 0, false},
    {"345678", "3456", "Carlos López", 10000, 0, false},
    {"456789", "4567", "Ana Martínez", 200000, 0, false},
    {"567890", "5678", "Pedro Sánchez", 100000, 0, false}
};

/**
 * @brief Configuración de las denominaciones disponibles y sus pines.
 */
Denomination denominations[] = {
    {10000, 5, 16},  // 5 billetes de 10,000
    {20000, 5, 17},  // 2 billetes de 20,000
    {50000, 5, 18},  // 2 billetes de 50,000
    {100000, 5, 19}, // 2 billetes de 100,000
};
/**
 * @brief Variable que indica si se ha presionado una tecla.
 */
volatile bool key_pressed = false;

/**
 * @brief Fila actual del teclado que está siendo escaneada.
 */
volatile uint8_t current_row = 0;

/**
 * @brief Última tecla presionada.
 */
volatile char last_key = 0;

/**
 * @brief Último tiempo en que se presionó una tecla.
 */
volatile absolute_time_t last_key_time;

/**
 * @brief Almacena el ID ingresado por el usuario.
 */
char input_id[ID_LENGTH + 1] = {0};

/**
 * @brief Almacena la contraseña ingresada por el usuario.
 */
char input_password[PASSWORD_LENGTH + 1] = {0};

/**
 * @brief Almacena una nueva contraseña cuando el usuario desea cambiarla.
 */
char new_password[PASSWORD_LENGTH + 1] = {0};

/**
 * @brief Índice actual de entrada de datos (ID o contraseña).
 */
int input_index = 0;

/**
 * @brief Estado actual del sistema.
 */
SystemState current_state = STATE_ENTER_ID;

/**
 * @brief Tiempo de inicio de la entrada de datos actual.
 */
absolute_time_t input_start_time;

/**
 * @brief Puntero al usuario actualmente autenticado.
 */
User* current_user = NULL;

int selected_index = 0;



/**
 * @brief timer_Callback para el temporizador que escanea las filas del teclado matricial.
 * 
 * @param alarm_num Número del temporizador.
 */
void timer_callback(uint alarm_num) {
    gpio_put(ROW_PINS[current_row], 1);
    current_row = (current_row + 1) % 4;
    gpio_put(ROW_PINS[current_row], 0);
    hardware_alarm_set_target(alarm_num, make_timeout_time_ms(5));
}

/**
 * @brief gpio_Callback para manejar la interrupción de un pin de GPIO (tecla presionada).
 * 
 * @param gpio Pin de GPIO que generó la interrupción.
 * @param events Eventos generados por el pin.
 */
void gpio_callback(uint gpio, uint32_t events) {
    if (!key_pressed) {
        absolute_time_t current_time = get_absolute_time();
        if (absolute_time_diff_us(last_key_time, current_time) > DEBOUNCE_DELAY) {
            for (int col = 0; col < 4; col++) {
                if (gpio == COL_PINS[col]) {
                    last_key = KEYPAD[current_row][col];
                    key_pressed = true;
                    last_key_time = current_time;
                    break;
                }
            }
        }
    }
}

/**
 * @brief Inicializa el teclado matricial configurando los pines de filas y columnas.
 */
void init_keypad() {
    for (int i = 0; i < 4; i++) {
        gpio_init(ROW_PINS[i]);
        gpio_set_dir(ROW_PINS[i], GPIO_OUT);
        gpio_put(ROW_PINS[i], 1);
        
        gpio_init(COL_PINS[i]);
        gpio_set_dir(COL_PINS[i], GPIO_IN);
        gpio_pull_up(COL_PINS[i]);
        gpio_set_irq_enabled_with_callback(COL_PINS[i], GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    }
    
    hardware_alarm_claim(0);
    hardware_alarm_set_callback(0, timer_callback);
    hardware_alarm_set_target(0, make_timeout_time_ms(5));
}

/**
 * @brief Busca un usuario en la lista de usuarios por su ID.
 * 
 * @param id ID del usuario a buscar.
 * @return User* Puntero al usuario encontrado, o NULL si no existe.
 */
User* find_user(const char* id) {     
    for (int i = 0; i < NUM_USERS; i++) {
        if (strcmp(users[i].id, id) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

/**
 * @brief Reinicia el estado del sistema para un nuevo intento de inicio de sesión.
 */
void reset_state() {
    memset(input_id, 0, sizeof(input_id));
    memset(input_password, 0, sizeof(input_password));
    memset(new_password, 0, sizeof(new_password));
    input_index = 0;
    current_state = STATE_ENTER_ID;
    current_user = NULL;
    input_start_time = get_absolute_time();
            displayMessage("Bienvenido MateCash ",0,0);
            displayMessage("                    ",1,0);
            displayMessage("Ingrese # cuenta:   ",2,0);
            displayMessage("                    ",3,0);
            printf("Bienvenido a CashMate");
            printf("\nIngrese su ID (6 digitos):\n");
}

/**
 * @brief Maneja el caso en que el tiempo para ingresar el ID o la contraseña ha sido excedido.
 */
void handle_timeout() {
    printf("\n¡Tiempo excedido! Por favor, intente de nuevo.\n");
    reset_state();
}

/**
 * @brief Muestra el menú de opciones del usuario una vez que ha iniciado sesión.
 */
void show_menu() {
    printf("\nMateCash:\n");
    printf("\nMenú de Usuario:\n");
    displayMessage("    Menú de Usuario ",0,0);   
    displayMessage("A-Retirar B-Revisar ",1,0);
    displayMessage("C - Cambiar Clave   ",2,0);
    displayMessage("D - Cerrar sesión   ",3,0);
    printf("A - Retirar Dinero\n");
    printf("B - Consultar Saldo\n");
    printf("C - Cambiar Clave\n");
    printf("D - Cerrar sesión\n");
    
    
  
}

/**
 * @brief Muestra las opciones de montor a retirar
*/
void amount_menu() {
    printf("\nMateCash:\n");
    printf("\nCuanto Dinero Desea retirar?:\n");
    printf("A - 10.000\n");
    printf("B - 20.000\n");
    printf("C - 50.000\n");
    printf("D - 100.000\n");
    //displayMessage("                    ",0,0);  
    displayMessage("Cuanto Dinero Desea ",0,0);
    displayMessage("      retirar?      ",1,0);
    displayMessage("A-10.000 B-20.000   ",2,0);
    displayMessage("C-50.000 D-100.000  ",3,0);
}
/**
 * @brief Procesa las acciones del usuario cuando ha iniciado sesión.
 * 
 * @param key Tecla presionada por el usuario.
 */
void process_logged_in_state(char key) {
    switch (key) {
        case 'A': // Retirar dinero
            amount_menu();
            current_state = STATE_WITHDRAW_MONEY;
            break;

            break;
        case 'B': // Consultar saldo
            printf("\nConsultando saldo...\n");
            current_state = STATE_CHECK_BALANCE;
            check_balance();
            break;
        case 'C':
            printf("\nIngrese nueva contraseña de 4 dígitos:\n");
            current_state = STATE_CHANGE_PASSWORD;
            input_index = 0;
            input_start_time = get_absolute_time();
            break;
        case 'D':
            printf("\nCerrando sesión...\n");
            reset_state();
            break;
        default:
            printf("\nOpción no válida\n");
            show_menu();
            break;
    }
}

/**
 * @brief Procesa la selección de una denominación de billetes para retiro.
 * 
 * Esta función se encarga de manejar la entrada del usuario cuando se encuentra
 * en el estado de selección de monto a retirar. Según la tecla presionada,
 * selecciona la denominación correspondiente y llama a la función `withdraw_money()`.
 * 
 * @param key Tecla presionada por el usuario, que indica la opción seleccionada:
 * - 'A': Retirar billetes de 10,000.
 * - 'B': Retirar billetes de 20,000.
 * - 'C': Retirar billetes de 50,000.
 * - 'D': Retirar billetes de 100,000.
 */
void amount_selection(char key) {
    switch (key) {
        case 'A': // Retirar dinero
            selected_index=0;
            withdraw_money();
            break;
        case 'B': // Consultar saldo
            selected_index=1;
            withdraw_money();
            break;
        case 'C':
             selected_index=2;
            withdraw_money();
            break;
        case 'D':
            selected_index=3;
            withdraw_money();
            break;
        default:
            printf("\nOpción no válida\n");
           amount_menu();
            break;
    }
}

/**
 * @brief Realiza el proceso de retiro de dinero de la cuenta del usuario.
 * 
 * Esta función verifica las condiciones necesarias para permitir un retiro de dinero. 
 * Valida si la cuenta está bloqueada, si hay billetes disponibles de la denominación seleccionada 
 * y si el usuario tiene suficiente saldo. Si todas las condiciones se cumplen, 
 * efectúa el retiro actualizando el saldo del usuario y la cantidad de billetes disponibles.
 */


void withdraw_money() {
    if (current_user->is_blocked) {
        printf("\nError: Su cuenta está bloqueada.\n");
        reset_state();
        return;
    }

    // Seleccionar la denominación
    Denomination* selected = &denominations[selected_index];

    // Verificar disponibilidad de billetes
    if (selected->quantity < 1) {
        printf("\nError: No hay billetes de %d disponibles. Intente con otra denominación.\n", selected->amount);
        amount_menu();
        return;
    }

    // Verificar saldo suficiente
    if (selected->amount > current_user->balance) {
        printf("\nError: Fondos insuficientes. Su saldo actual es %.2f\n", current_user->balance);
        amount_menu();
        return;
    }


    // Realizar el retiro
    mov_motors(selected->pinselect);
    current_user->balance -= selected->amount;
    selected->quantity -= 1;

    printf("\nÉxito: Retiró %d. Retiro con el valor de %.2f\n", selected->amount); 

    // Mostrar balance actualizado
    current_state = STATE_CHECK_BALANCE;
    check_balance();
}
/**
 * @brief Muestra el saldo actual de la cuenta del usuario.
 * 
 * Esta función verifica si la cuenta del usuario está bloqueada. 
 * Si no lo está, imprime el saldo actual del usuario y proporciona la opción de finalizar la operación.
 */

void check_balance() {
    if (current_user->is_blocked) {
        printf("\nError: Su cuenta está bloqueada.\n");
        return;
    }

    printf("\nSu saldo actual es: %.2f\n", current_user->balance);
    printf("\nPresione '#' para finalizar");
}
/**
 * @brief Procesa la tecla presionada por el usuario según el estado actual del sistema.
 * 
 * @param key Tecla presionada por el usuario.
 */
void process_key(char key) {
    absolute_time_t current_time = get_absolute_time();
    
    if (absolute_time_diff_us(input_start_time, current_time) > (MAX_INPUT_TIME_MS * 1000) &&
        current_state == STATE_ENTER_PASSWORD) {
        handle_timeout();
        return;
    }

    switch (current_state) {
        case STATE_ENTER_ID:
            if (input_index < ID_LENGTH) {
                input_id[input_index++] = key;
                printf("%c", key);
                if (input_index == ID_LENGTH) {
                    input_id[ID_LENGTH] = '\0';
                    current_user = find_user(input_id);
                    if (current_user == NULL || current_user->is_blocked) {
                        if (current_user && current_user->is_blocked) {
                            printf("\n¡Usuario bloqueado! Contacte al administrador.\n");
                        } else {
                            printf("\nID de usuario no existe.\n");
                        }
                        reset_state();
                    } else {
                        printf("\nIngrese contraseña de 4 dígitos:\n");
                        /*/displayMessage("                    ",0,0);
                        displayMessage("    Ingrese clave:  ",1,0);
                        displayMessage("                    ",2,0);
                        displayMessage("                    ",3,0);/*/
                        input_start_time = get_absolute_time();                                                     
                        current_state = STATE_ENTER_PASSWORD;
                        input_index = 0;
                    }
                }
            }
            break;

        case STATE_ENTER_PASSWORD:
            if (input_index < PASSWORD_LENGTH) {
                input_password[input_index++] = key;
                printf("*");
                if (input_index == PASSWORD_LENGTH) {
                    input_password[PASSWORD_LENGTH] = '\0';
                    if (strcmp(current_user->password, input_password) == 0) {
                        printf("\n\n¡Bienvenido, %s!\n", current_user->name);
                        current_user->failed_attempts = 0;
                        current_state = STATE_LOGGED_IN;
                        show_menu();
                    } else {
                        current_user->failed_attempts++;
                        if (current_user->failed_attempts >= MAX_FAILED_ATTEMPTS) {
                            current_user->is_blocked = true;
                            printf("\n\n¡Usuario bloqueado! Demasiados intentos fallidos.\n");
                        } else {
                            printf("\n\nContraseña incorrecta. Intentos restantes: %d\n", 
                                   MAX_FAILED_ATTEMPTS - current_user->failed_attempts);
                        }
                        reset_state();
                    }
                }
            }
            break;

        case STATE_LOGGED_IN:
            process_logged_in_state(key);
            break;
        
        case STATE_CHECK_BALANCE:  // Maneja la consulta de saldo
        
            if (key == '#') { // Confirma para salir del estado
                printf("\nGracias por utilzar nuestros serivicos\n");
                reset_state();

            } else {
                check_balance();
            }
        
            break;

        case STATE_WITHDRAW_MONEY:  // Maneja el retiro de dinero
            amount_selection(key);
            break;

        case STATE_CHANGE_PASSWORD:
            if (input_index < PASSWORD_LENGTH) {
                new_password[input_index++] = key;
                printf("*");
                if (input_index == PASSWORD_LENGTH) {
                    printf("\nConfirme la nueva contraseña:\n");
                    current_state = STATE_CONFIRM_PASSWORD;
                    input_index = 0;
                    memset(input_password, 0, sizeof(input_password));
                }
            }
            break;

        case STATE_CONFIRM_PASSWORD:
            if (input_index < PASSWORD_LENGTH) {
                input_password[input_index++] = key;
                printf("*");
                if (input_index == PASSWORD_LENGTH) {
                    if (strcmp(new_password, input_password) == 0) {
                        strcpy(current_user->password, new_password);
                        printf("\n¡Contraseña cambiada exitosamente!\n");
                    } else {
                        printf("\nLas contraseñas no coinciden. Intente de nuevo.\n");
                    }
                    current_state = STATE_LOGGED_IN;
                    show_menu();
                }
            }
            break;

    }
}
