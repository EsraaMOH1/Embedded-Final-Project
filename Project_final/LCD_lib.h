/*
 * LCD_lib.h
 *
 *  Created on: 13 Dec 2024
 *      Author: abutalb
 */


#ifndef LCD_LIB_H_
#define LCD_LIB_H_


#define LCD_CLEAR                1
#define LCD_CURSOR_OFF           2
#define LCD_UNDERLINE_ON         3
#define LCD_BLINK_CURSOR_ON      4



void lcd_init(void);    //lcd init

void lcd_cmd(unsigned int cmd);  // lcd commands

void lcd_char(int row, int col, char out_char);  // prints 1 character in spacific position

void lcd_char_cp(char out_char);  //current position

void lcd_text(int row, int col, char *text); //prints more than one character

void lcd_text_cp(char *text);

void ftoa(float num, char *str, int precision);


#endif /* LCD_LIB_H_ */
