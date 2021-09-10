/*	Author: Tyler Pastor tpast001@ucr.edu
 *  Partner(s) Name: N/A
 *	Lab Section: 023
 *	Assignment: Lab #10  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=kwebjDOmGzQ
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

char code[] = {'1','2','3','4','5'};
int num = -1;
int flag;

enum keypadInput_States{userInput, wait};

int checkCode(int state){
	char input;

	switch(state){
		case userInput:
		        input = GetKeypadKey();

			if(num == 5){
				PORTB = 0x01;
				state = userInput;
			}
			else{
				if(input == '#'){
					num = 0;
					state = userInput;		
				}
				else if(input != '\0'){
					if(num == -1){
						state = userInput;
					}
					else{
						state = wait;
					}
				}
				else{
					state = userInput;
				}
			}
			break;

		case wait:
			input = GetKeypadKey();

			if(input == code[num]){
				state = wait;	
				flag = 0;
			}
			else if(input != code[num] && input != '\0'){
				flag = 1;
				state = wait;	
			}	       
			else if(input == '\0'){
				if(flag == 1){
					num = -1;
				}
				else{
					num++;
				}
				state = userInput;
			}
			break;


		default:
			state = userInput;
			break;
	}

	switch(state){
		case userInput:
			break;

		case wait:
			break;
	}

	return state;
}

///////////////////////////////////////

enum locking_States{buttonPress};

int lockButton(int state){
        switch(state){
                case buttonPress:
                  	if((~PINB & 0x80) == 0x80){
			   PORTB = 0x00;
			   num = -1;
			}
	     		else{
				PORTB = PORTB;
			}		

                        state = buttonPress;
                        break;

                default:
                        state = buttonPress;
                        break;
        }

        switch(state){
                case buttonPress:
                        break;
        }

        return state;
}

int main(void) {
    	DDRC = 0xF0; PORTC = 0x0F;
    	DDRB = 0x0F; PORTB = 0xF0;	
	
    	static task task1, task2;
    	task *tasks[] = {&task1, &task2};
    	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    	const char start = -1;

    	task1.state = start;
    	task1.period = 50;
    	task1.elapsedTime = task1.period;
    	task1.TickFct = &checkCode;

	task2.state = start;
	task2.period = 50;
	task2.elapsedTime = task2.period;
	task2.TickFct = &lockButton;

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
