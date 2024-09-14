#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char seconds=0; //Defining the seconds' counter
unsigned char minutes=0; //Defining the minutes' counter
unsigned char hours=0; //Defining the hours' counter
unsigned char Countdown_Enable=0; //Defining Count-down flag (0:Normal counter,1:Count-down)
unsigned char Pause_Timer=0; //Defining Pause flag
unsigned char Hold_Press_Toggle=0; //Defining toggle flag (to avoid holding button's action)
unsigned char Hold_Press_Seconds_Increment=0; //Defining second increment's flag (to avoid holding button's action)
unsigned char Hold_Press_Seconds_Decrement=0; //Defining second decrement's flag (to avoid holding button's action)
unsigned char Hold_Press_Minutes_Increment=0; //Defining minutes increment's flag (to avoid holding button's action)
unsigned char Hold_Press_Minutes_Decrement=0; //Defining minutes decrement's flag (to avoid holding button's action)
unsigned char Hold_Press_Hours_Increment=0; //Defining hours increment's flag (to avoid holding button's action)
unsigned char Hold_Press_Hours_Decrement=0; //Defining hours decrement's flag (to avoid holding button's action)

void Timer1_Initalization (void); //Defining the Timer 1 initalization's prototype
void Seven_Segment_Initalization (void); //Defining the Seven Segment's prototype
void Display_Time (unsigned char Segment_Number); //Defining the function responsible for displaying the numbers on the 7 segment
void Reset_Button_Initializtion (void); //Defining the Reset button's prototype
void Pause_Button_Initializtion (void); //Defining the Pause button's prototype
void Resume_Button_Initializtion (void); //Defining the Resume button's prototype
void Toggle_Button_And_LEDs_Initializtion (void); //Defining the Toggle and LED button's prototype
void Toggle_Mode (void); //Defining the toggle mode button's prototype
void Adjusting_Time_Buttons_Initializtion (void); //Defining the Adjusting time's buttons' prototype
void Buzzer_Initialization (void); //Defining the buzzer's initialization prototype
void Hour_Increment (void); //Defining the Hour increment's initialization prototype
void Hour_Decrement (void); //Defining the Hour decrement's initialization prototype
void Minute_Increment (void); //Defining the Minute increment's initialization prototype
void Minute_Decrement (void); //Defining the Minute decrement's initialization prototype
void Second_Increment (void); //Defining the Second increment's initialization prototype
void Second_Decrement (void); //Defining the Second decrement's initialization prototype

int main (void)
{
	Timer1_Initalization(); //Calling the timer initialization's function
	Seven_Segment_Initalization (); //Calling the seven segment initialization's function
	Reset_Button_Initializtion (); //Calling the reset button initialization's function
	Pause_Button_Initializtion (); //Calling the pause button initialization's function
	Resume_Button_Initializtion (); //Calling the resume button initialization's function
	Toggle_Button_And_LEDs_Initializtion (); //Calling the Toggle button and led initialization's function
	Adjusting_Time_Buttons_Initializtion (); //Calling the Adjusting time buttons initialization's function

	while (1)
	{
		Display_Time (1); //Displaying the first seven segment (Second's units digit)
		_delay_ms (2);
		Display_Time (2); //Displaying the second seven segment (Second's tens digit)
		_delay_ms (2);
		Display_Time (3); //Displaying the third seven segment (Minute's units digit)
		_delay_ms (2);
		Display_Time (4); //Displaying the fourth seven segment (Minute's tens digit)
		_delay_ms (2);
		Display_Time (5); //Displaying the fifth seven segment (Hour's units digit)
		_delay_ms (2);
		Display_Time (6); //Displaying the sixth seven segment (Hour's tens digit)
		_delay_ms (2);

		if (!(PINB & (1<<PB7))) //Check if the toggle button is pressed
		{
			_delay_ms (30);
			if (!(PINB & (1<<PB7))) //Double check due to bouncing
			{
				Toggle_Mode(); //Toggle the mode
			}
			else //If the button is released, the hold press flag for toggle is cleared
			{
				Hold_Press_Toggle = 0;
			}
		}

		/* If any adjusting button is pressed while the stop-watch is on the count-down mode, then the time will be adjusted*/
		if (Countdown_Enable && Pause_Timer)
		{
			{
				if (!(PINB & (1<<PB1))) //Check if the hours increment button is pressed
				{
					_delay_ms (30);
					if (!(PINB & (1<<PB1))) //Double check due to bouncing
					{
						Hour_Increment ();
					}
					else //If the button is released, the hold press flag for hours increment is cleared
					{
						Hold_Press_Hours_Increment = 0;
					}
				}

				if (!(PINB & (1<<PB0))) //Check if the hours decrement button is pressed
				{
					_delay_ms (30);
					if (!(PINB & (1<<PB0))) //Double check due to bouncing
					{
						Hour_Decrement ();
					}
					else //If the button is released, the hold press flag for hours decrement is cleared
					{
						Hold_Press_Hours_Decrement = 0;
					}
				}

				if (!(PINB & (1<<PB4))) //Check if the minutes increment button is pressed
				{
					_delay_ms (30);
					if (!(PINB & (1<<PB4))) //Double check due to bouncing
					{
						Minute_Increment ();
					}
					else //If the button is released, the hold press flag for minutes increment is cleared
					{
						Hold_Press_Minutes_Increment = 0;
					}
				}

				if (!(PINB & (1<<PB3))) //Check if the minutes decrement button is pressed
				{
					_delay_ms (30);
					if (!(PINB & (1<<PB3))) //Double check due to bouncing
					{
						Minute_Decrement ();
					}
					else //If the button is released, the hold press flag for minutes decrement is cleared
					{
						Hold_Press_Minutes_Decrement = 0;
					}
				}

				if (!(PINB & (1<<PB6))) //Check if the seconds increment button is pressed
				{
					_delay_ms (30);
					if (!(PINB & (1<<PB6))) //Double check due to bouncing
					{
						Second_Increment ();
					}
					else //If the button is released, the hold press flag for seconds increment is cleared
					{
						Hold_Press_Seconds_Increment = 0;
					}
				}

				if (!(PINB & (1<<PB5))) //Check if the seconds decrement button is pressed
				{
					_delay_ms (30);
					if (!(PINB & (1<<PB5))) //Double check due to bouncing
					{
						Second_Decrement ();
					}
					else //If the button is released, the hold press flag for hours decrement is cleared
					{
						Hold_Press_Seconds_Decrement = 0;
					}
				}
			}

		}

	}
	return 0;
}

void Timer1_Initalization (void)
{
	/*Initializing Timer1
	 * 1. Non PWM mode (FOC1A=1, FOC1B=1)
	 * 2. Choose Compare mode (WGM12=1)
	 * 3. Pre-scaler = 1024 (CS12=1,CS10=1)
	 * 4. Initial value for timer = 0
	 * 5. Set compare value equal=15625 (OCR1A=15625)
	 * 6. Output Compare A Match Interrupt Enable (OCIE1A=1)
	 * 7. Enable interrupt (I-bit in SREG=1)
	 */

	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
	TCNT1 = 0;
	OCR1A = 15625;
	TIMSK = (1<<OCIE1A);
	SREG = (1<<7);
}

ISR (TIMER1_COMPA_vect) //Timer's 1 interrupt
{
	if (!Countdown_Enable) //If the normal counter mode is enabled
	{
		if (seconds == 59) //Check if the seconds reached its maximum value to increase the minutes
		{
			seconds = 0; //Reset the seconds to zero
			if (minutes == 59) //Check if the minutes reached its maximum value to reset it and increase the hours
			{
				minutes = 0; //Reset the minutes
				hours++; //Increase the hours by one
			}
			else //If the minutes' counter smaller than 59, increase the minutes only
			{
				minutes++;
			}
		}
		else //If the seconds' counter smaller than 59, increase the seconds only
		{
			seconds++;
		}
	}
	else if (Countdown_Enable) //If the count-down mode is enabled
	{
		if (seconds == 0) //Check if the seconds reached its minimum value to decrease the minutes
		{
			if (minutes == 0) //Check if the minutes reached its minimum value
			{
				if (hours == 0) //Check if the hours equals zero, hence the count-down reaches zero
				{
					TCCR1B |= (1<<CS10) | (1<<CS12); //Stop the timer
					TCNT1 = 0; //Resetting the timer's counter in the register
					PORTD |= (1<<PD0); //Turn the buzzer on
				}
				else //If the hours not equal zero, set the minutes and seconds to be equal 59 and decrease the hours by one
				{
					hours --;
					minutes = 59;
					seconds = 59;
				}
			}
			else //If the minutes' counter greater than 0, decrease the minutes and set seconds to be equal 59
			{
				minutes --;
				seconds = 59;
			}
		}
		else //If the seconds' counter greater than 0, decrease the seconds only
		{
			seconds --;
		}
	}
}


void Seven_Segment_Initalization (void)
{
	DDRA |= 0x3F; //Set the first 6-pins in PORTA as output pins
	PORTA &= 0x80; //Initial values for the 6-pins equal zero (disable the 7-segment)
	DDRC |= 0x0F; //Set the first 4-pins in PORTC as output pins
	PORTC &= 0xC0; //Initial values for the 4-pins equal zero
}

void Display_Time (unsigned char Segment_Number) //Receiving the required segment to be displayed on
{
	unsigned char Displayed_Number=0; //Define unsigned character to hold the number required to be displayed (seconds, minutes, hours)
	switch (Segment_Number) //Choosing the required segment
	{
	case 1: //Displaying on the first seven segment
	{
		PORTA = (PORTA & 0x80) | (0x20); //Disable all the 7-segments and enable the first one only
		Displayed_Number = seconds % 10; //Get the unit digit of the seconds
		PORTC = (PORTC & 0xF0) | (Displayed_Number); //Send the number to the decoder to be displayed on the 7 segment
		break;
	}
	case 2: //Displaying on the second seven segment
	{
		PORTA = (PORTA & 0x80) | (0x10); //Disable all the 7-segments and enable the second one only
		Displayed_Number = (seconds/10) % 10; //Get the ten digit of the seconds
		PORTC = (PORTC & 0xF0) | (Displayed_Number); //Send the number to the decoder to be displayed on the 7 segment
		break;
	}
	case 3: //Displaying on the third seven segment
	{
		PORTA = (PORTA & 0x80) | (0x08); //Disable all the 7-segments and enable the third one only
		Displayed_Number = minutes % 10; //Get the unit digit of the minutes
		PORTC = (PORTC & 0xF0) | (Displayed_Number); //Send the number to the decoder to be displayed on the 7 segment
		break;
	}
	case 4: //Displaying on the fourth seven segment
	{
		PORTA = (PORTA & 0x80) | (0x04); //Disable all the 7-segments and enable the fourth one only
		Displayed_Number = (minutes/10) % 10; //Get the ten digit of the minutes
		PORTC = (PORTC & 0xF0) | (Displayed_Number); //Send the number to the decoder to be displayed on the 7 segment
		break;
	}
	case 5: //Displaying on the fifth seven segment
	{
		PORTA = (PORTA & 0x80) | (0x02); //Disable all the 7-segments and enable the fifth one only
		Displayed_Number = hours % 10; //Get the unit digit of the hours
		PORTC = (PORTC & 0xF0) | (Displayed_Number); //Send the number to the decoder to be displayed on the 7 segment
		break;
	}
	case 6: //Displaying on the sixth seven segment
	{
		PORTA = (PORTA & 0x80) | (0x01); //Disable all the 7-segments and enable the sixth one only
		Displayed_Number = (hours/10) % 10; //Get the ten digit of the hours
		PORTC = (PORTC & 0xF0) | (Displayed_Number); //Send the number to the decoder to be displayed on the 7 segment
		break;
	}
	}
}
void Reset_Button_Initializtion (void)
{
	/*Initializing Reset Button
	 * 1. Set the PD2 pin as input (PD2=0)
	 * 2. Enable the internal pull-up resistance
	 * 3. The falling edge of INT0 generates an interrupt request (ISC01=1,ISC00=0)
	 * 3. External Interrupt Request 0 Enable
	 */
	DDRD &= ~(1<<PD2);
	PORTD |= (1<<PD2);
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
	GICR |= (1<<INT0);
}

ISR (INT0_vect) //External interrupt0 (Reset Button)
{
	TCNT1 = 0; //Resetting the timer's counter in the register
	seconds=0; //Setting the seconds=0
	minutes=0; //Setting the minutes=0
	hours=0; //Setting the hours=0
	PORTD &= ~(1<<PD0); //Turn the buzzer off
}

void Pause_Button_Initializtion (void)
{
	/*Initializing Reset Button
	 * 1. Set the PD3 pin as input (PD3=0)
	 * 2. The rising edge of INT1 generates an interrupt request (ISC11=1,ISC10=1)
	 * 3. External Interrupt Request 1 Enable
	 */
	DDRD &= ~(1<<PD3);
	MCUCR |= (1<<ISC10) | (1<<ISC11);
	GICR |= (1<<INT1);
}

ISR (INT1_vect) //External interrupt1 (Pause Button)
{
	/*Pausing the Timer by setting the CS11,CS12 equal zero in the TCCR1B register and set the pause flag*/
	TCCR1B &= ~(1<<CS10) & ~(1<<CS12);
	Pause_Timer = 1;
	PORTD &= ~(1<<PD0); //Turn the buzzer off
}

void Resume_Button_Initializtion (void)
{
	/*Initializing Reset Button
	 * 1. Set the PB2 pin as input (PB2=0)
	 * 2. Enable the internal pull-up resistance
	 * 3. The falling edge of INT2 generates an interrupt request (ISC2=0)
	 * 4. External Interrupt Request 2 Enable
	 */
	DDRB &= ~(1<<PB2);
	PORTB |= (1<<PB2);
	MCUCR &= ~(1<<ISC2);
	GICR |= (1<<INT2);
}

ISR (INT2_vect) //External interrupt2 (Resume Button)
{
	/*Resuming the Timer by setting the CS11,CS12 equal one in the TCCR1B register and clear the pause flag*/
	TCCR1B |= (1<<CS10) | (1<<CS12);
	Pause_Timer = 0;
}

void Toggle_Button_And_LEDs_Initializtion (void)
{
	/*Initializing Reset Button
	 * 1. Set the PB7 pin as input (PB7=0)
	 * 2. Enable the internal pull-up resistance
	 * 3. Set the PD4 pin as output and initially enable it (Count-up)
	 * 4. Set the PD5 pin as output and initially disable it (Count-down)
	 */
	DDRB &= ~(1<<PB7);
	PORTB |= (1<<PB7);
	DDRD |= (1<<PD4);
	DDRD |= (1<<PD5);
	PORTD |= (1<<PD4);
	PORTD &= ~(1<<PD5);
}
void Toggle_Mode (void)
{
	if (Pause_Timer && !(Hold_Press_Toggle)) //If the pause flag is set and the button is pressed and not held, then toggle the counting mode
	{
		PORTD ^= (1<<PD4); //Toggle the count-up LED
		PORTD ^= (1<<PD5); //Toggle the count-down LED
		seconds = minutes = hours = 0; //Reset the stop-watch
		Countdown_Enable ^= (1<<0); //Toggle the mode
		TCNT1 = 0; //Reseting the timer's counter in the register
		TCCR1B &= ~(1<<CS10) & ~(1<<CS12); //Pausing the timer
	}
	Hold_Press_Toggle = 1; //Set the Hold press flag = 1 to avoid toggling while holding on the toggling button
}

void Adjusting_Time_Buttons_Initializtion (void)
{
	DDRB &= ~(1<<PB0) & ~(1<<PB1) & ~(1<<PB3) & ~(1<<PB4) & ~(1<<PB5) & ~(1<<PB6); //Set the pins (0,1,3,4,5,6) in port B to be input pins
	PORTB |= (1<<PB0) | (1<<PB1) | (1<<PB3) | (1<<PB4) | (1<<PB5) | (1<<PB6); //Activate internal pull-up resistance for pins (0,1,3,4,5,6) in port B
}

void Buzzer_Initialization (void)
{
	DDRD |= (1<<PD0); //Set the pin 0 in port D as output pin
	PORTD &= ~(1<<PD0); //Initially, the pin PD0 will be equal 0
}

void Hour_Increment (void)
{
	if (!(Hold_Press_Hours_Increment)) //If the pause flag is set and the button is pressed and not held, then increase the hour by one
	{
		hours++;
		Hold_Press_Hours_Increment = 1; //Set the hold press flag by one to avoid increasing while holding on the button
	}
}

void Hour_Decrement (void)
{
	if (!(Hold_Press_Hours_Decrement)) //If the pause flag is set and the button is pressed and not held, then decrease the hour by one
	{
		if (hours == 0) //Check if the hours=0, so after decrement, the hours counter will be 99
		{
			hours = 99;
		}
		else //If the hours not equal zero, decrease the hours by one
		{
			hours--;
		}
		Hold_Press_Hours_Decrement = 1; //Set the hold press flag by one to avoid decreasing while holding on the button
	}
}

void Minute_Increment (void)
{
	if (!(Hold_Press_Minutes_Increment)) //If the pause flag is set and the button is pressed and not held, then increase the minute by one
	{
		if (minutes == 59) //Check if the minutes=59, so after increment, the minutes counter will be 0
		{
			minutes = 0;
		}
		else //If the minutes not equal 59, increase the minutes by one
		{
			minutes++;
		}
		Hold_Press_Minutes_Increment = 1; //Set the hold press flag by one to avoid increasing while holding on the button
	}
}

void Minute_Decrement (void)
{
	if (!(Hold_Press_Minutes_Decrement)) //If the pause flag is set and the button is pressed and not held, then decrease the hour by one
	{
		if (minutes == 0) //Check if the minutes=0, so after decrement, the minutes counter will be 59
		{
			minutes = 59;
		}
		else //If the minutes not equal zero, decrease the minutes by one
		{
			minutes--;
		}
		Hold_Press_Minutes_Decrement = 1; //Set the hold press flag by one to avoid decreasing while holding on the button
	}
}

void Second_Increment (void)
{
	if (!(Hold_Press_Seconds_Increment)) //If the pause flag is set and the button is pressed and not held, then increase the seconds by one
	{
		if (seconds == 59) //Check if the seconds=59, so after increment, the seconds counter will be 0
		{
			seconds = 0;
		}
		else //If the seconds not equal 59, increase the seconds by one
		{
			seconds++;
		}
		Hold_Press_Seconds_Increment = 1; //Set the hold press flag by one to avoid increasing while holding on the button
	}
}

void Second_Decrement (void)
{
	if (!(Hold_Press_Seconds_Decrement)) //If the pause flag is set and the button is pressed and not held, then decrease the seconds by one
	{
		if (seconds == 0) //Check if the seconds=0, so after decrement, the seconds counter will be 59
		{
			seconds = 59;
		}
		else //If the seconds not equal zero, decrease the seconds by one
		{
			seconds--;
		}
		Hold_Press_Seconds_Decrement = 1; //Set the hold press flag by one to avoid decreasing while holding on the button
	}
}
