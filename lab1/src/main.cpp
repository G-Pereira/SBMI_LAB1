/*
 * lab1.c
 *
 *  Created on: Oct 2, 2017
 *      Author: Gonçalo Pereira, Ricardo Silva
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define NS_GREEN PB5
#define NS_YELLOW PB4
#define NS_RED PB3
#define EO_GREEN PB2
#define EO_YELLOW PB1
#define EO_RED PB0

char stateNormal = 0;
float timerCount = 0;

ISR(INT0_vect) {
    char stateEmergency = 0;
    timerCount = 0;
    TCNT1 = 0; // Reset timer value
    while (1) {
        if (TCNT1 >= 16000) {
            timerCount += 0.01;
            TCNT1 = 0; // Reset timer value
        }

        //STATE MACHINE
        if (((stateNormal == 4 || stateNormal == 5) || (stateNormal == 1 || stateNormal == 2)) && stateEmergency == 0) {
            stateEmergency = 1;
            timerCount = 0; // Start counting 2 seconds for the yellow light
        } else if (((stateNormal == 0) || stateNormal == 3) &&
                   stateEmergency == 0) {
            stateEmergency = 2;
            timerCount = 0;
        } else if (stateEmergency == 1 && timerCount >= 2) {
            stateEmergency = 2;
            timerCount = 0;
        } else if (stateEmergency == 2 && timerCount >= 15) {
            break;
        }

        //set output values
        if (stateEmergency == 1) {
            if (stateNormal == 4 || stateNormal == 5) {
                PORTB &= ~(1 << EO_GREEN);
                PORTB &= ~(1 << NS_GREEN);
                PORTB &= ~(1 << NS_YELLOW);
                PORTB &= ~(1 << EO_RED);
                PORTB |= (1 << NS_RED);
                PORTB |= (1 << EO_YELLOW);
            } else {
                PORTB &= ~(1 << EO_GREEN);
                PORTB &= ~(1 << NS_RED);
                PORTB &= ~(1 << NS_GREEN);
                PORTB &= ~(1 << EO_YELLOW);
                PORTB |= (1 << NS_YELLOW);
                PORTB |= (1 << EO_RED);
            }
        } else if (stateEmergency == 2) {
            PORTB &= ~(1 << EO_YELLOW);
            PORTB &= ~(1 << EO_GREEN);
            PORTB &= ~(1 << NS_YELLOW);
            PORTB &= ~(1 << NS_GREEN);
            PORTB |= (1 << EO_RED);
            PORTB |= (1 << NS_RED);
        }
    }
    timerCount = 0;
    // Exit states
    if (stateNormal == 4 || stateNormal == 5 || stateNormal == 0)
        stateNormal = 4;
    else stateNormal = 1;
    TCNT1 = 0; // Reset timer value
}

int main() {
    // Set up timer
    TCCR1B |= (1 << CS10);

    // Semafores
    DDRB |= (1 << NS_GREEN); // EO green
    DDRB |= (1 << NS_YELLOW); // EO yellow
    DDRB |= (1 << NS_RED); // EO red
    DDRB |= (1 << EO_GREEN); // NS green
    DDRB |= (1 << EO_YELLOW); // NS yellow
    DDRB |= (1 << EO_RED); // NS red

    // Emergency Button
    DDRD &= ~(1 << PD2); // set PD2 as input
    PORTD |= (1 << PD2); // turn on internal pull-up

    EICRA |= (1 << ISC01); // configure trigger to the rising edge of the signal

    EIMSK |= (1 << INT0); // activate interrupt function on PD2 (INT0)
    sei(); // turn on interruptions

    while (1) {
        // Count how many times the timer reaches 16000 cicles and count 0.001s
        // since the microcontroller has a 16MHz timer
        if (TCNT1 >= 16000) {
            timerCount += 0.01;
            TCNT1 = 0; // Reset timer value
        }

        // STATE MACHINE
        if (stateNormal == 0 && timerCount >= 2) {
            stateNormal = 1;
            timerCount = 0; // Reset counter
        } else if (stateNormal == 1 && timerCount >= 15) {
            stateNormal = 2;
            timerCount = 0;
        } else if (stateNormal == 2 && timerCount >= 2) {
            stateNormal = 3;
            timerCount = 0;
        } else if (stateNormal == 3 && timerCount >= 2) {
            stateNormal = 4;
            timerCount = 0;
        } else if (stateNormal == 4 && timerCount >= 15) {
            stateNormal = 5;
            timerCount = 0;
        } else if (stateNormal == 5 && timerCount >= 2) {
            stateNormal = 0;
            timerCount = 0;
        }

        // Set Output's values
        switch (stateNormal) {
            case 0:
                PORTB &= ~(1 << NS_GREEN);
                PORTB &= ~(1 << NS_YELLOW);
                PORTB &= ~(1 << EO_GREEN);
                PORTB &= ~(1 << EO_YELLOW);
                PORTB |= (1 << NS_RED);
                PORTB |= (1 << EO_RED);
                break;
            case 1:
                PORTB &= ~(1 << NS_YELLOW);
                PORTB &= ~(1 << NS_RED);
                PORTB &= ~(1 << EO_GREEN);
                PORTB &= ~(1 << EO_YELLOW);
                PORTB |= (1 << NS_GREEN);
                PORTB |= (1 << EO_RED);
                break;
            case 2:
                PORTB &= ~(1 << NS_GREEN);
                PORTB &= ~(1 << NS_RED);
                PORTB &= ~(1 << EO_GREEN);
                PORTB &= ~(1 << EO_YELLOW);
                PORTB |= (1 << NS_YELLOW);
                PORTB |= (1 << EO_RED);
                break;
            case 3:
                PORTB &= ~(1 << NS_GREEN);
                PORTB &= ~(1 << NS_YELLOW);
                PORTB &= ~(1 << EO_GREEN);
                PORTB &= ~(1 << EO_YELLOW);
                PORTB |= (1 << NS_RED);
                PORTB |= (1 << EO_RED);
                break;
            case 4:
                PORTB &= ~(1 << NS_YELLOW);
                PORTB &= ~(1 << NS_GREEN);
                PORTB &= ~(1 << EO_YELLOW);
                PORTB &= ~(1 << EO_RED);
                PORTB |= (1 << EO_GREEN);
                PORTB |= (1 << NS_RED);
                break;
            case 5:
                PORTB &= ~(1 << NS_GREEN);
                PORTB &= ~(1 << NS_YELLOW);
                PORTB &= ~(1 << EO_GREEN);
                PORTB &= ~(1 << EO_RED);
                PORTB |= (1 << NS_RED);
                PORTB |= (1 << EO_YELLOW);
                break;
            default:
                stateNormal = 0;
                break;
        }
    }
}
