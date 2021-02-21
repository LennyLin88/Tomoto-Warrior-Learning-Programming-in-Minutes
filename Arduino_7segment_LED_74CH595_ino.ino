//**************************************************************//
//  Name    : 7segment_LED_74HC595                               
//  Author  : 西红柿武士
//  Date    : 2021-02-03
//  Modified:                                  
//  Version : 0.1                                             
//  Notes   : Sketch to demonstrate how 7 Segment LED Displays works driven by  
//          : 74HC595N chip                          
//****************************************************************

// Globals
/* 利用74HC595N的8位寄存器芯片驱动LED数码管
*/

const int DS = 13;
const int STCP = 7;
const int SHCP = 6;


bool decPt = true;  // 设置小数点

void setup() {
    //初始化引脚输出 
    //Serial.begin(9600);移除注释调试输出
    pinMode(DS, OUTPUT);
    pinMode(STCP, OUTPUT);
    pinMode(SHCP, OUTPUT);
    digitalWrite(STCP, LOW);
    digitalWrite(SHCP, LOW);
    delay(500);
 
}

void loop() {
  decPt = !decPt; // 小数点每隔一个循环显示
  
  // 按十六进制 0 to F 的顺序显示数字
  for (int i = 0; i <=15; i++) {
    byte bits = NumToBits(i) ;
    //Serial.println(bits,BIN); 移除注释调试数据输出
    if (decPt) {
      bits = bits | B00000001; //添加小数点位
    }
    setData(bits);
    //STCP上升沿电平将寄存器数据导出到锁存器
    digitalWrite(STCP, LOW);
    digitalWrite(STCP, HIGH);
    delay(1000);
  }
}

//函数，将二进制的数码管显示区域逐位读取到寄存器
void setData(byte eightBits) {
 int bitNumber = 7;
  for (int i = 0; i <= 7; i++) {
    byte lightSegment = bitRead(eightBits, bitNumber--);
    //Serial.println(lightSegment);
    digitalWrite(DS, !lightSegment);
    //SHCP上升沿电平将数据移位
    digitalWrite(SHCP, LOW);
    digitalWrite(SHCP, HIGH);
  }
}

//函数，将数字转化为8位二进制的数码管显示区域
byte NumToBits(int someNumber) {
  switch (someNumber) {
    case 0:
      return B11111100;
      break;
    case 1:
      return B01100000;
      break;
    case 2:
      return B11011010;
      break;
    case 3:
      return B11110010;
      break;
    case 4:
      return B01100110;
      break;
    case 5:
      return B10110110;
      break;
    case 6:
      return B10111110;
      break;
    case 7:
      return B11100000;
      break;
    case 8:
      return B11111110;
      break;
    case 9:
      return B11110110;
      break;
    case 10:
      return B11101110; // Hexidecimal A
      break;
    case 11:
      return B00111110; // Hexidecimal B
      break;
    case 12:
      return B10011100; // Hexidecimal C or use for Centigrade
      break;
    case 13:
      return B01111010; // Hexidecimal D
      break;
    case 14:
      return B10011110; // Hexidecimal E
      break;
    case 15:
      return B10001110; // Hexidecimal F or use for Fahrenheit
      break;  
    default:
      return B10010010; // Error condition, displays three vertical bars
      break;   
  }
}
