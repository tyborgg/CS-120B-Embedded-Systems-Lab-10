/*	Author: Tyler Pastor tpast001@ucr.edu
 *  Partner(s) Name: N/A
 *	Lab Section: 023
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "PWM.h"
#include "bit.h"
#include "scheduler.h"
#include "keypad.h"
#endif

enum buttonPress_States{userInput};

int checkState(int state){
	char input;

	switch(state){
		case userInput:
		        input = GetKeypadKey();
			
			if(input != '\0'){
				if(input == '1'){
					PORTB = 0x81;
				}
				else if(input == '1'){
					PORTB = 0x82;
                                }	
				else if(input == '2'){
					PORTB = 0x82;
                                }
				else if(input == '3'){
                                        PORTB = 0x83;
                                }
                                else if(input == '4'){
					PORTB = 0x84;
                                }
				else if(input == '5'){
                                        PORTB = 0x85;
                                }
                                else if(input == '6'){
					PORTB = 0x86;
                                }
				else if(input == '7'){
                                        PORTB = 0x87;
                                }
                                else if(input == '8'){
					PORTB = 0x88;
                                }
				else if(input == '9'){
                                        PORTB = 0x89;
                                }
                                else if(input == 'A'){
					PORTB = 0x8A;
                                }
				else if(input == 'B'){
                                        PORTB = 0x8B;
                                }
                                else if(input == 'C'){
					PORTB = 0x8C;
                                }
				else if(input == 'D'){
                                        PORTB = 0x8D;
                                }
                                else if(input == '*'){
                                        PORTB = 0x8E;
                                }
                                else if(input == '0'){
                                        PORTB = 0x80;
                                }
				else if(input == '#'){
					PORTB = 0x8F;
				}
			}
			else{
				PORTB = 0x1F;
			}
			state = userInput;
			break;

		default:
			state = userInput;
			break;
	}

	switch(state){
		case userInput:
			break;
	}

	return state;
}

int main(void) {
    	DDRC = 0xF0; PORTC = 0x0F;
    	DDRB = 0xFF; PORTB = 0x00;	
	
    	static task task1;
    	task *tasks[] = {&task1};
    	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    	const char start = -1;

    	task1.state = start;
    	task1.period = 50;
    	task1.elapsedTime = task1.period;
    	task1.TickFct = &checkState;

    	TimerSet(50);
    	TimerOn();

	unsigned short i;
    	while (1) {
		for(i = 0; i < numTasks; i++){
 			if(tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 50;
		}	


		while(!TimerFlag);
		TimerFlag = 0;
    	}

    	return 0;
}
