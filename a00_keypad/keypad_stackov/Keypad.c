#include <avr/io.h>
#include<util/delay.h>



//Keypad Information
#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define C0 4
#define C1 5
#define C2 6
#define C3 7

#define keypadPORT PORTA
#define keypadPIN PINA
#define keypadDDR DDRA


//Keypad functions and global variables
char getkey();
int keypadRow[] = {R0, R1, R2, R3}; //rows of the keypad
int keypadCol[] = {C0, C1, C2, C3};//columnd



int main()
{
   char key_pressed;

   keypadDDR |= (1<<R0)|(1<<R1)|(1<<R2)|(1<<R3);//set upper part of keypad port as output
                                                //this will be required for scanning the rows
   keypadDDR &= ~((1<<C0)|(1<<C1)|(1<<C2)|(1<<C3));//set lower part of keypad port as input.This is
                                                   //the part of the keypad port where the rows are connected.

   LCD_init(); //initialize LCD

   while(1)
   {
      key_pressed = getkey();
      switch(key_pressed)
      {
        case('A'):
            break;//do nothing if no key is pressed
        default:
            send_char(key_pressed);//send the key pressed to LCD
      }
   }     
   return 0;
}



char getkey()
{  
   int i, j;
   for(i = 0; i < 4; i++)
   {
    keypadPORT = 0x00;
    keypadPORT |= (1 << keypadRow[i]);//send a high to a particular row of the keypad

    for(j = 0; j < 4; j++)
    {
        if(bit_is_set(keypadPIN,keypadCol[j]))//check if key is pressed
        {
            while(bit_is_set(keypadPIN,keypadCol[j]));  //wait for key to be released
            switch(i)
            {
                case(0):
                {
                    if (j == 0) return '7';
                    else if (j == 1) return '8';
                    else if (j == 2) return '9';
                    else if (j == 3) return '/';
                    break;
                }
                case(1):
                {
                    if (j == 0) return '4';
                    else if (j == 1) return '5';
                    else if (j == 2) return '6';
                    else if (j == 3) return '*';
                       break;
                }
                case(2):
                {
                    if (j == 0) return '1';
                    else if (j == 1) return '2';
                    else if (j == 2) return '3';
                    else if (j == 3) return '-';
                       break;
                }
                case(3):
                {
                    if (j == 0) return '?';
                    else if (j == 1) return '0';
                    else if (j == 2) return '=';
                    else if (j == 3) return '+';
                       break;
                }
            }
        }
    }
   }
    return 'A';//Return 'A' if no key is pressed.
}