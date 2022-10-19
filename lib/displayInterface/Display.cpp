#include "Display.h"

#define SCL 23
#define SDA 22
#define DC 19
#define CS 18
#define RESET 21

Display :: Display(){
  this -> lcd = U8G2_SSD1309_128X64_NONAME0_1_4W_SW_SPI(U8G2_R0, SCL, SDA, CS, DC, RESET);
}

void Display :: begin(){
  this -> lcd.begin();
  this -> lcd.setContrast(255);
}

void Display :: init(){
  this -> lcd.setContrast(255);
  this -> lcd.setDrawColor(1);
  this -> lcd.setFontPosTop();
  this -> lcd.setFontDirection(0);
}
    
void Display :: printTest() {
  String text;
  this -> lcd.firstPage();
  do {
    int x_offset = 10;
    int y_offset = 2;      

    text = "teste";
    lcd.setCursor(x_offset , y_offset);
    lcd.setFont(u8g2_font_helvR12_tf);
    lcd.print(text.c_str());
    
  } while ( this -> lcd.nextPage() ); 
}

void Display :: clear(){
  //lcd.clearDisplay();
  this -> lcd.clearBuffer();
}

void Display :: print(String message, int dx, int dy){
    this -> lcd.setFont(u8g2_font_missingplanet_tf);
    lcd.firstPage();
    do {
      lcd.setCursor(dx, dy);
      lcd.print(message.c_str());
    } while ( lcd.nextPage() );
  }










// class Display{
//   public:
//   //-- Instancia o Painel OLED    
//   U8G2_SSD1309_128X64_NONAME0_1_4W_SW_SPI lcd;

//   Display(){
//     this->lcd = U8G2_SSD1309_128X64_NONAME0_1_4W_SW_SPI(U8G2_R0, SCL, SDA, CS, DC, RESET);
//   }
  

//   void init(){
//     lcd.setContrast(255);
//     lcd.setDrawColor(1);
//     lcd.setFontPosTop();
//     lcd.setFontDirection(0);
//   }

    
//   void printTest() {
//     String text;
//     lcd.firstPage();
//     do {
//       int x_offset = 10;
//       int y_offset = 2;      

//       text = "teste";
//       lcd.setCursor(x_offset , y_offset);
//       lcd.setFont(u8g2_font_helvR12_tf);
//       lcd.print(text.c_str());


//       // text = "test";
//       // lcd.setCursor(55, 28 + y_offset);
//       // lcd.setFont(u8g2_font_missingplanet_tf);
//       // lcd.print(text.c_str());
      
//     } while ( lcd.nextPage() ); 
//   }

//   void clear(){
//     //lcd.clearDisplay();
//     lcd.clearBuffer();
//   }


//   void print(String message, int dx, int dy){
//     lcd.setFont(u8g2_font_missingplanet_tf);
//     lcd.firstPage();
//     do {
//       lcd.setCursor(dx, dy);
//       lcd.print(message.c_str());
//     } while ( lcd.nextPage() );
//   }
// };
