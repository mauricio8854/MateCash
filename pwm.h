/**
 * @file PWM.h
 * @brief Definiciones y prototipos para el control de motores mediante PWM.
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h> // Para tipos como uint

/**
 * @brief Controla el movimiento de un motor activado por un pin GPIO.
 *
 * @param motor_pin El pin GPIO utilizado para controlar el motor.
 */
void mov_motors(int motor_pin);

#endif // PWM_H