//*****************************************************************************
//  技能五輪全国大会 電子機器組立て職種
//  LCD display Library for PIC18F6722/4620/4520  ====>  filename [ lcdlib_xc8_v03.h ]
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
#define PORT_CHANGE
#ifndef _LCDLIB_XC8_V02_H
#define _LCDLIB_XC8_V02_H

/*PICのOSCに合わせて変更する*/
#ifndef	_XTAL_FREQ
#define _XTAL_FREQ 4000000UL
#endif

#ifdef PORT_CHANGE
#define LCD_DATA_PORT	PORTA			//D :Data bus line RF7～RF4(4bits)
#define	LCD_DATA_LAT	LATA			//D :Data bus line output
#define	TRIS_DB4	TRISAbits.TRISA4	//DB4 :Data bit4 RF4
#define	TRIS_DB5	TRISAbits.TRISA5	//DB5 :Data bit5 RF5
#define	TRIS_DB6	TRISAbits.TRISA6	//DB6 :Data bit6 RF6
#define	TRIS_DB7	TRISAbits.TRISA7	//DB7 :Data bit7 RF7

#else
#define	LCD_DATA_PORT	PORTD			//D :Data bus line RD7～RD4(4bits)
#define	LCD_DATA_LAT	LATD			//D :Data bus line output
#define	TRIS_DB4	TRISDbits.TRISD4	//DB4 :Data bit4 RD4
#define	TRIS_DB5	TRISDbits.TRISD5	//DB5 :Data bit5 RD5
#define	TRIS_DB6	TRISDbits.TRISD6	//DB6 :Data bit6 RD6
#define	TRIS_DB7	TRISDbits.TRISD7	//DB7 :Data bit7 RD7
#endif

#define RS			LATEbits.LATE2	//RS:Register select signal
#define RW			LATEbits.LATE0	//RW:Read/Write signal
#define E			LATEbits.LATE1	//E :Enable signal

unsigned int LCD_Busy();
void _InstOut(unsigned char cmd);
void LCD_Cmd(unsigned char cmd);
void _DataOut(unsigned char cmd);
void LCD_Init(void);
void LCD_Clear(void);
void LCD_CursorOn(void);
void LCD_CursorOff(void);
void LCD_Home(void);
void LCD_Locate(unsigned int row,unsigned int column);
void LCD_Character(char asci);
void LCD_String(char *code);
void LCD_String_p(char *code);
void LCD_Number(signed int number);
void LCD_FNumber(float fnumber,unsigned char left,unsigned char right);
void LCD_HNumber(unsigned int number, signed int keta);
void LCD_BNumber(unsigned int number, signed int keta);

#endif
