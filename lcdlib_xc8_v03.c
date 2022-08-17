//*****************************************************************************
//  技能五輪全国大会 電子機器組立て職種
//  LCD display Library for PIC18F6722/4620/4520  ====>  filename [ lcdlib_xc8_v03.c ]
//
//  変更履歴
//  (lcdlib_c18_v01)
//      2009. 8. 20 : C18コンパイラに適応 lcdlib_c18_v01.h 	by N.T
//  (lcdlib_c18_v02)
//      2009. 9. 25 : 構成分割・インクルードガード追加 				by K.T
//  (lcdlib_c18_v03)
//      2010. 5. 19 : LCD_FNumber関数の改定 	by K.M
//  (lcdlib_c18_v04)
//      2013. 3. 27 : LCD_Busy関数の改定 	by H.K
//  (lcdlib_c18_v05)
//      2015. 3. 31 : PIC18F6722に対応 	by H.K
//  (lcdlib_xc8_v01)
//      2016. 4. 07 : MPLAB XC8コンパイラに対応 	by H.K
//  (lcdlib_xc8_v02)
//      2018. 4. 11 : MPLAB XC8コンパイラ v1.45 に対応 	by H.K
//  (lcdlib_xc8_v03)
//      2019. 7. 17 : C言語規格 C99 に対応 	by H.K
//
//                        Copyright(C) Yasuharu Yajima 2007
//*****************************************************************************

#include <xc.h>
#include <math.h>
#include "lcdlib_xc8_v03.h"

//-----------------------------------------------------------------------------
//  ＬＣＤ入出力関数
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

//command wirte function by 8-bit output(命令書込:8bit出力)
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

//command wirte function by 4-bit(命令書込:4bit×2回出力)
void LCD_Cmd(unsigned char cmd)
{
	_InstOut(cmd & 0xf0);           //set command upper
	_InstOut(cmd<<4 & 0xf0);        //set command lower
	while(LCD_Busy()&0x80);  //end check
	return;
}


//data wirte function by 8-bit output(データ書込:8bit出力)
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
//  ＬＣＤ制御関数
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
//  ＬＣＤ表示関数
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
			case '\n':	line = ~LCD_Busy() & 0x40;  //CR+LF:復帰改行
						LCD_Cmd(0x80 | line);
						break;
			case '\r':	line = LCD_Busy() & 0x40;   //CR:復帰
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
	char w, num[5+1];       //num[]:数値列の入る文字配列

	if(number < 0){         //負の数の処理
		LCD_Character('-');
		number *= -1;
	}
	for(i=0; i<5; i++){     //下位の桁からnum[]に格納
		num[i] = (char)(0x30 + number % 10);
		number /= 10;
		if (number==0) break;
	}
	length = i;
	num[length+1] = '\0';   //文字配列の最後に'\0'を格納
	for(i=0; i<(length+1)/2; i++){  //上位→下位へ,桁の入れ替え.
		w = num[i];
		num[i] = num[length-i];
		num[length-i] = w;
	}
	LCD_String_p(num);
	return;
}

void LCD_FNumber(float fnumber,unsigned char left,unsigned char right)
{	
	signed char i;					//ループ用		
	char num[16];			//表示配列
	char *str = num;			//表示配列先頭アドレス
	char *count = num;		//表示配列先頭アドレス
	unsigned char sign = 0;			//符号判定
	unsigned long integer;			//整数部格納
	unsigned long decimal;			//小数部格納
	
	//符号判定
	if(fnumber<0.0){
		sign=1;			
		fnumber *= -1.0;		
	}
	//小数部の倍数計算
	for(i=0,decimal=1; i<right; i++){
		decimal *= 10;
	}
	//整数部の倍数計算
	for(i=0,integer=1; i<left; i++){
		integer *= 10;
	}	
	//四捨五入
	fnumber = fnumber+(0.5/(float)decimal);
	//整数部の取り出し
	integer = (unsigned long)fnumber%integer;
	//小数部の取り出し
	decimal = (unsigned long)((fnumber-(float)integer)*(float)decimal);
	
	//小数部ASCII変換
	for(i=0; i<right; i++,str++){					
		*str = (signed char)(decimal%10)+'0';
		decimal /= 10;
	}
	//ドット挿入
	if(right){										
		*str = '.'; 
		str++;
	}
	//整数部ASCII変換
	for(i=0; i<left; i++,str++){					
		if(integer){		//数値ASCII変換						
			*str = (signed char)(integer%10)+'0';	
			integer /= 10;
		}
		else if(!i){		//整数部=0				
			*str = '0';								
		}
		else if(sign){		//マイナス						
			*str = '-';
			sign = 0;
		}
		else{				//空白				
		 	*str = ' ';
		} 												
	}
	if(sign){				//表示桁範囲外のマイナス						
		*str = '-';	
	}						
	else{					//マイナスなし
		str--;
	}
	//文字の表示
	while(str >= count){				
		LCD_Character(*str);
		str--;
	}
}

void LCD_HNumber(unsigned int number, signed int keta)
{
	unsigned int   i, n;
	char num[4+2];	//int型16ビット(最大4桁)
	i = 0;
	for(--keta; keta>=0; keta--){
		n = (int)(number >> (keta * 4)) & 0x0f;
		if(n<10)
			num[i++] = (char)(0x30 + n);        //0h?9hまで
		else
			num[i++] = (char)(0x41 + n - 10);   //Ah?Fhまで
	}
	num[i++] = 'h';
	num[i] = '\0';  //文字列配列の最後に'\0'を格納
	LCD_String_p(num);
	return;
}

void LCD_BNumber(unsigned int number, signed int keta)
{
	unsigned int   i, n;
	char num[16+2]; //int型16ビット(最大16桁)
	i = 0;
	for(--keta; keta>=0; keta--){
		n = (number >> keta) & 0x01;
		num[i++] = (char)(0x30 + n);
	}
	num[i++] = 'b';	//16桁で表示する場合,表示位置に注意!
	num[i] = '\0';  //文字列配列の最後に'\0'を格納
	LCD_String_p(num);
	return;
}
