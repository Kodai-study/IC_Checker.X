//*****************************************************************************
//  ?Z?\???S????? ?d?q?@??g????E??
//  LCD display Library for PIC18F6722/4620/4520  ====>  filename [ lcdlib_xc8_v03.c ]
//
//  ??X????
//  (lcdlib_c18_v01)
//      2009. 8. 20 : C18?R???p?C????K?? lcdlib_c18_v01.h 	by N.T
//  (lcdlib_c18_v02)
//      2009. 9. 25 : ?\???????E?C???N???[?h?K?[?h??? 				by K.T
//  (lcdlib_c18_v03)
//      2010. 5. 19 : LCD_FNumber???????? 	by K.M
//  (lcdlib_c18_v04)
//      2013. 3. 27 : LCD_Busy???????? 	by H.K
//  (lcdlib_c18_v05)
//      2015. 3. 31 : PIC18F6722???? 	by H.K
//  (lcdlib_xc8_v01)
//      2016. 4. 07 : MPLAB XC8?R???p?C?????? 	by H.K
//  (lcdlib_xc8_v02)
//      2018. 4. 11 : MPLAB XC8?R???p?C?? v1.45 ???? 	by H.K
//  (lcdlib_xc8_v03)
//      2019. 7. 17 : C????K?i C99 ???? 	by H.K
//
//                        Copyright(C) Yasuharu Yajima 2007
//*****************************************************************************

#include <xc.h>
#include <math.h>
#include "lcdlib_xc8_v03.h"

//-----------------------------------------------------------------------------
//  ?k?b?c???o????
//-----------------------------------------------------------------------------
//busy check function
unsigned int LCD_Busy()
{
	unsigned int high, low;
	/*LCD_READ*/
	TRIS_DB4=1;
	TRIS_DB5=1;
	TRIS_DB6=1;
	TRIS_DB7=1;
	RW=1;        //RW=1:read
	RS=0;         //RS=0:instruction
	E=1;         //E =1:enable
	__delay_us(4);
	high = LCD_DATA_PORT & 0xf0;   //set upper
	E=0;          //read upper
	E=1;         //E =1:enable
	__delay_us(4);
	low = LCD_DATA_PORT & 0xf0;    //set lower
	E=0;          //read lower
	RW=0;        //RW=0:write//nt
	/*LCD_WRITE*/
	TRIS_DB4=0;
	TRIS_DB5=0;
	TRIS_DB6=0;
	TRIS_DB7=0;
	return (high | low>>4);
}

//command wirte function by 8-bit output(???????:8bit?o??)
void _InstOut(unsigned char cmd)
{
	RS=0;         //RS=0:instruction
	RW=0;         //RW=0:write 
	E=1;         //E =1:enable
	LCD_DATA_LAT=(cmd & 0xf0) | (LCD_DATA_PORT & 0x0f);//set command
	E=0;          //E= 0:write
	__delay_us(100);
	return;
}

//command wirte function by 4-bit(???????:4bit?~2??o??)
void LCD_Cmd(unsigned char cmd)
{
	_InstOut(cmd & 0xf0);           //set command upper
	_InstOut(cmd<<4 & 0xf0);        //set command lower
	while(LCD_Busy()&0x80);  //end check
	return;
}


//data wirte function by 8-bit output(?f?[?^????:8bit?o??)
void _DataOut(unsigned char cmd)
{
	RS=1;        //RS=1:data
	RW=0;         //RW=0:write
	E=1;         //E =1:enable
	LCD_DATA_LAT=(cmd & 0xf0) | (LCD_DATA_PORT & 0x0f);//set data
	E=0;          //E= 0:write
	__delay_us(100);
	return;
}

//-----------------------------------------------------------------------------
//  ?k?b?c??????
//-----------------------------------------------------------------------------
//initialize function
void LCD_Init(void)
{
	TRISEbits.TRISE0=0;
	TRISEbits.TRISE1=0;
	TRISEbits.TRISE2=0;
	__delay_ms(10);
	/*LCD_WRITE*/
	TRIS_DB4=0;
	TRIS_DB5=0;
	TRIS_DB6=0;
	TRIS_DB7=0;
	__delay_ms(15);
	_InstOut(0x30);		//8bit mode set
	__delay_ms(4);
	_InstOut(0x30);     //8bit mode set
	_InstOut(0x30);     //8bit mode set
	_InstOut(0x20);     //4bit mode set
	LCD_Cmd(0x28);      //function set
	LCD_Cmd(0x08);      //display off
	LCD_Cmd(0x0D);      //display on
	LCD_Cmd(0x14);      //cousor shift
	LCD_Cmd(0x06);      //entry mode
	LCD_Cmd(0x01);      //clear display
	__delay_ms(2);
	return;
}

//display clear function
void LCD_Clear(void)
{
	LCD_Cmd(0x01);      //clear display
	__delay_ms(2);
	return;
}

//corsor on/off function
void LCD_CursorOn(void)
{
	LCD_Cmd(0x0E);
	return;
}

void LCD_CursorOff(void)
{
	LCD_Cmd(0x0C);
	return;
}

//cousor control function
void LCD_Home(void)
{
	LCD_Cmd(0x02);      //cousor at home
	__delay_ms(2);
	return;
}

void LCD_Locate(unsigned int row,unsigned int column)
{
	LCD_Cmd(0x80 + 0x40 * (row % 2) + column);//set DDRAM address
	return;
}

//-----------------------------------------------------------------------------
//  ?k?b?c?\?????
//-----------------------------------------------------------------------------
//display string function
void LCD_Character(char asci)
{
	_DataOut(asci & 0xf0);          //set command upper
	_DataOut((asci<<4) & 0xf0);     //set command lower
	while(LCD_Busy()&0x80);  //end check
	return;
}

void LCD_String(char *code)
{
	unsigned char line;
	while(*code){
		switch(*code){
			case '\n':	line = ~LCD_Busy() & 0x40;  //CR+LF:???A???s
						LCD_Cmd(0x80 | line);
						break;
			case '\r':	line = LCD_Busy() & 0x40;   //CR:???A
						LCD_Cmd(0x80 | line);
						break;
			default  :	LCD_Character(*code);
						break;
		}
		code++;
	}
	return;
}

void LCD_String_p(char *code)
{
	LCD_String(code);
	return;
}


//display number function
void LCD_Number(signed int number)
{
	unsigned int i, length;
	char w, num[5+1];       //num[]:???l?????????z??

	if(number < 0){         //??????????
		LCD_Character('-');
		number *= -1;
	}
	for(i=0; i<5; i++){     //??????????num[]??i?[
		num[i] = (char)(0x30 + number % 10);
		number /= 10;
		if (number==0) break;
	}
	length = i;
	num[length+1] = '\0';   //?????z??????'\0'???i?[
	for(i=0; i<(length+1)/2; i++){  //?????????,?????????.
		w = num[i];
		num[i] = num[length-i];
		num[length-i] = w;
	}
	LCD_String_p(num);
	return;
}

void LCD_FNumber(float fnumber,unsigned char left,unsigned char right)
{	
	signed char i;					//???[?v?p		
	char num[16];			//?\???z??
	char *str = num;			//?\???z????A?h???X
	char *count = num;		//?\???z????A?h???X
	unsigned char sign = 0;			//????????
	unsigned long integer;			//???????i?[
	unsigned long decimal;			//???????i?[
	
	//????????
	if(fnumber<0.0){
		sign=1;			
		fnumber *= -1.0;		
	}
	//????????{???v?Z
	for(i=0,decimal=1; i<right; i++){
		decimal *= 10;
	}
	//????????{???v?Z
	for(i=0,integer=1; i<left; i++){
		integer *= 10;
	}	
	//?l????
	fnumber = fnumber+(0.5/(float)decimal);
	//??????????o??
	integer = (unsigned long)fnumber%integer;
	//??????????o??
	decimal = (unsigned long)((fnumber-(float)integer)*(float)decimal);
	
	//??????ASCII???
	for(i=0; i<right; i++,str++){					
		*str = (signed char)(decimal%10)+'0';
		decimal /= 10;
	}
	//?h?b?g?}??
	if(right){										
		*str = '.'; 
		str++;
	}
	//??????ASCII???
	for(i=0; i<left; i++,str++){					
		if(integer){		//???lASCII???						
			*str = (signed char)(integer%10)+'0';	
			integer /= 10;
		}
		else if(!i){		//??????=0				
			*str = '0';								
		}
		else if(sign){		//?}?C?i?X						
			*str = '-';
			sign = 0;
		}
		else{				//??				
		 	*str = ' ';
		} 												
	}
	if(sign){				//?\???????O??}?C?i?X						
		*str = '-';	
	}						
	else{					//?}?C?i?X???
		str--;
	}
	//??????\??
	while(str >= count){				
		LCD_Character(*str);
		str--;
	}
}

void LCD_HNumber(unsigned int number, signed int keta)
{
	unsigned int   i, n;
	char num[4+2];	//int?^16?r?b?g(???4??)
	i = 0;
	for(--keta; keta>=0; keta--){
		n = (int)(number >> (keta * 4)) & 0x0f;
		if(n<10)
			num[i++] = (char)(0x30 + n);        //0h?`9h???
		else
			num[i++] = (char)(0x41 + n - 10);   //Ah?`Fh???
	}
	num[i++] = 'h';
	num[i] = '\0';  //??????z??????'\0'???i?[
	LCD_String_p(num);
	return;
}

void LCD_BNumber(unsigned int number, signed int keta)
{
	unsigned int   i, n;
	char num[16+2]; //int?^16?r?b?g(???16??)
	i = 0;
	for(--keta; keta>=0; keta--){
		n = (number >> keta) & 0x01;
		num[i++] = (char)(0x30 + n);
	}
	num[i++] = 'b';	//16????\???????,?\????u?????!
	num[i] = '\0';  //??????z??????'\0'???i?[
	LCD_String_p(num);
	return;
}
