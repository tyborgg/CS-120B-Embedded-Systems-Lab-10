/*	Author: Tyler Pastor tpast001@ucr.edu
 *  Partner(s) Name: N/A
 *	Lab Section: 023
 *	Assignment: Lab #10  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=qcDdojWmn1A
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

char code[5];
int num = -1;
unsigned char A;
int flag;
char input;

enum keypadInput_States{userInput, Wait};

int checkCode(int state){
	input = GetKeypadKey();

	switch(state){
		case userInput:
			if(num == 5){
				PORTB = PORTB | 0x01;
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
						state = Wait;
					}
				}
				else if(num == 4 && code[4] == '#'){
					state = Wait;
				}
				else{
					state = userInput;
				}
			}
			break;

		case Wait:
			if(num == 4 && code[4] == '#'){
				num++;
				state = userInput;
			}
			else if(input == code[num]){
				state = Wait;	
				flag = 0;
			}
			else if(input != code[num] && input != '\0'){
				flag = 1;
				state = Wait;	
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

		case Wait:
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
			   PORTB = PORTB & 0x40;
			   num = -1;
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

///////////////////////////////////////

int num2 = 0;
int melody;

enum doorBell_States{melodyWait, play};

int ring(int state){

        switch(state){
		case play:
			if(A == 0x80 && melody != 1){
				num2 = 1;
				melody = 1;
				state = play;
			}
			else if(num2 >= 1 && num2 <= 3){
				if(num2 == 1){
					set_PWM(500);
				}
				num2++;
				state = play;
			}
			else if(num2 >= 4 && num2 <= 6){
				if(num2 == 4){
                                        set_PWM(200);
                                }
                                num2++;
				state = play;
			}
			else if(num2 >= 7 && num2 <= 9){
				if(num2 == 7){
					set_PWM(600);
				}
                                num2++;
				state = play;
			}
			else if(num2 >= 10 && num2 <= 12){
				if(num2 == 10){
					set_PWM(300);
				}
                                num2++;
				state = play;
			}
			else{
				state = melodyWait;
			}
			break;

		case melodyWait:
			set_PWM(0.0);

			if(A == 0x80){
				state = melodyWait;
			}
			else if(A == 0x00){
				melody = 0;
				state = play;
			}
			break;

      		default:
         		state = play;
         		break;
   	}

   	switch(state) {
      		case melodyWait:
        		break;

		case play:
			break;
	}
	


	return state;
}
 
/////////////////////////////////////////

int val = 0;

enum passwordChange{newCode, enter, hold, release};

int password(int state){
	input = GetKeypadKey();

        switch(state){
		case enter:
                        if((~PINB & 0x80) == 0x80 && input == '*'){
                           state = hold;
                        }
			else{
				state = enter;
			}
                        break;

		case hold:
			if((~PINB & 0x80) == 0x80 && input == '*'){
                           state = hold;
                        }
			else if((~PINB & 0x80) != 0x80 && input == '\0'){	
				val = 0;
				state = newCode;
			}
			else{
				state = hold;
			}
			break;

		case newCode:
			if(val == 0 && input != '\0'){
				code[val] = input;
				state = release;
			}
			else if(val == 1 && input != '\0'){
                                code[val] = input;
				state = release;
                        }
			else if(val == 2 && input != '\0'){
                                code[val] = input;
				state = release;
                        }
			else if(val == 3 && input != '\0'){
                                code[val] = input;
				state = release;
                     	}
			else if(val == 4 && input == '#'){
				code[val] = '#';
				state = enter;
			}
			else if(val == 4 && input != '#' && input != '\0'){
				val = 0;
				state = newCode;
			}
			else{
				state = newCode;
			}
			break;

		case release:
			if(input != '\0'){
				state = release;
			}
			else{
				val++;
				state = newCode;
			}
			break;

                default:
                        state = enter;
                        break;
        }

        switch(state){
                case newCode:
                        break;

		case enter:
			break;

		case hold:
			break;

		case release:
			break;
        }

        return state;
}

///////////////////////////////////////

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
    	DDRC = 0xF0; PORTC = 0x0F;
    	DDRB = 0x4F; PORTB = 0xB0;	
	
	code[0] = '1';
	code[1] = '2';
	code[2] = '3';
        code[3] = '4';
	code[4] = '5';

    	static task task1, task2, task3, task4;
    	task *tasks[] = {&task1, &task2, &task3, &task4};
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

	task3.state = start;
        task3.period = 200;
        task3.elapsedTime = task3.period;
        task3.TickFct = &ring;

	task4.state = start;
        task4.period = 50;
        task4.elapsedTime = task4.period;
        task4.TickFct = &password;
	
	PWM_on();
    	TimerSet(50);
    	TimerOn();

	unsigned short i;
    	while (1) {
		A = ~PINA;

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
