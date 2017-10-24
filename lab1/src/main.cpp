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
char stateEmergency = 0;

ISR(INT0_vect) {
    stateEmergency = 0;
    timerCount = 0;
    TCNT1 = 0; // Reset timer value
    while (1) {
        if (16000 <= TCNT1) {
            timerCount += 0.001;
            TCNT1 = 0; // Reset timer value
        }

        //STATE MACHINE
        if (((4 == stateNormal || 5 == stateNormal) || (1 == stateNormal || 2 == stateNormal)) &&
            0 == stateEmergency) {
            stateEmergency = 1;
            timerCount = 0; // Start counting 2 seconds for the yellow light
        } else if (((0 == stateNormal) || 3 == stateNormal) &&
                   0 == stateEmergency) {
            stateEmergency = 2;
            timerCount = 0;
        } else if (1 == stateEmergency && 2 <= timerCount) {
            stateEmergency = 2;
            timerCount = 0;
        } else if (2 == stateEmergency && 15 <= timerCount) {
            break;
        }

        //set output values
        if (1 == stateEmergency) {
            if (4 == stateNormal || 5 == stateNormal) {
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
        } else if (2 == stateEmergency) {
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
    if (4 == stateNormal || 5 == stateNormal || 0 == stateNormal)
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
        if (16000 <= TCNT1) {
            timerCount += 0.001;
            TCNT1 = 0; // Reset timer value
        }

        // STATE MACHINE
        if (0 == stateNormal && 2 <= timerCount) {
            stateNormal = 1;
            timerCount = 0; // Reset counter
        } else if (1 == stateNormal && 10 <= timerCount) {
            stateNormal = 2;
            timerCount = 0;
        } else if (2 == stateNormal && 2 <= timerCount) {
            stateNormal = 3;
            timerCount = 0;
        } else if (3 == stateNormal && 2 <= timerCount) {
            stateNormal = 4;
            timerCount = 0;
        } else if (4 == stateNormal && 15 <= timerCount) {
            stateNormal = 5;
            timerCount = 0;
        } else if (5 == stateNormal && 2 <= timerCount) {
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
