/*
 * 西红柿武士作品
 * FreeRTOS系列视频示例--串口通讯控制伺服电机
 */

#include <Arduino_FreeRTOS.h>
#include <Servo.h>

//定义任务句柄
TaskHandle_t TaskSerial_Handler;
TaskHandle_t TaskServo_Handler;

//定义任务调用函数
//void TaskBlink( void *pvParameters );
void TaskSerial(void* pvParameters);
void TaskServo(void* pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {
  // 初始化串口通讯窗口，设置波特率为9600
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  
  // 创立两个任务
  //伺服电机任务
   xTaskCreate(
    TaskServo //任务函数指针（函数名）
    ,  "Servo" //任务名字
    ,  128  // 栈尺寸
    ,  NULL // 传入给任务函数的参数（这里为空，不需要参数）
    ,  1  // 优先级
    ,  &TaskServo_Handler );  //任务句柄
 //串口通讯任务
  xTaskCreate(
    TaskSerial //任务函数指针（函数名）
    ,  "Serial" //任务名字
    ,  128  // 栈尺寸
    ,  NULL //传入给任务函数的参数（这里为空，不需要参数）
    ,  1  // 优先级
    ,  &TaskSerial_Handler );  //任务句柄
    
}
    

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskSerial(void* pvParameters){
/*
 串口通讯任务，读取输入字符，'s'挂起任务，'r'重启任务
 
*/
  (void) pvParameters;
  
   for (;;) // A Task shall never return or exit.
   {
    while(Serial.available()>0){
      switch(Serial.read()){
        case 's':
          //vTaskSuspend(TaskBlink_Handler); 
          vTaskSuspend(TaskServo_Handler);
          Serial.println("Suspend!");
          break;
        case 'r':
          //vTaskResume(TaskBlink_Handler);
          vTaskResume(TaskServo_Handler);
          Serial.println("Resume!");
          break;
        
      }
      vTaskDelay(1);
    }
   }
}

//伺服电机任务
void TaskServo(void *pvParameters)  // This is a task.
{
  (void) pvParameters; 
  Servo myservo; //初始化伺服电机对象
  myservo.attach(9); //和9引脚绑定
  int val01 = 45; //设置转动角度变量
  int val02 =0; //设置转动角度变量
  for (;;) // A Task shall never return or exit.
  {
    //Serial.println(11);
    myservo.write(val01); //转动角度
    Serial.print("Turning"); //打印信息
    Serial.println(val01);
    vTaskDelay(100); //延迟让转动完成
    myservo.write(val02); //转动角度
    Serial.print("Turning");//打印信息
    Serial.println(val02);
    vTaskDelay(100); //延迟让转动完成
  }
}
