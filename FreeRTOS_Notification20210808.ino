/**
 * 西红柿武士作品
 * 任务直接通知视频示例
*/

// 包含Arduino FreeRTOS 库
#include <Arduino_FreeRTOS.h>
/*
 * 定义不同的位的值对应不同的设置
 */
#define BUTTON 0x01 
#define KNOB   0x02 
#define OPEN   0x03
#define CLEAN  0x00
#ifndef ULONG_MAX
  #define ULONG_MAX 0xffffffff
#endif  
#define THRESHOLD_VALUE 300 //定义内置LED点亮的阀值

TaskHandle_t taskNotificationHandler; //定义任务句柄
/*
 * 定义不同延迟的长度
 */
const TickType_t xDelay = pdMS_TO_TICKS( 500 );
const TickType_t xAnalogReadDelay = pdMS_TO_TICKS( 3000 );
const TickType_t xCleanUPDelay = pdMS_TO_TICKS( 5000 );
//函数声明
void TaskHandler( void *pvParameters );
void TaskReadAnalog( void *pvParameters );
void prvBlink( void );
void interruptHandler( void );
void TaskCleanUP(void *pvParameters);
//声明事件接收变量
volatile uint32_t ulEventsToProcess=0;

/*
 * 中断回调函数
 * 每次按下按钮设置事件BUTTON位值为1
 */
void interruptHandler(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyFromISR(taskNotificationHandler, BUTTON, eSetBits, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) {
    taskYIELD();
  }
}
/*
 * 内置LED等闪烁函数
 * 每次闪烁一次，点亮和熄灭间隔为0。5秒
 */
void prvBlink( void )
{
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for 0.5 second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for 0.5 second
}

/*
 * 关键任务，根据事件状态做出相应的动作
 */
void TaskHandler(void *pvParameters)
{
  //(void) pvParameters;
  BaseType_t xResult;
  for (;;) {
     

      xResult = xTaskNotifyWait( pdFALSE, //进入函数后不清除原来的位的值   
                           BUTTON, //函数退出前BUTTON事件位的值清零
                           &ulEventsToProcess, //事件通知值的存储变量
                           portMAX_DELAY );//函数永久等待事件的更新

      if( xResult == pdPASS ) //表明通知状态被更新，接收到了通知事件信息
      {
         
         if( ( ulEventsToProcess & BUTTON ) != 0 )
         {
          Serial.println("Button"); //如果BUTTON位的值为1，打印"BUTTON"信息
         }
          //如果事件同时满足两个条件即1和2位同时为1，打印位的值和"WOW"信息
         if( ( ulEventsToProcess & OPEN ) == OPEN ) 
         {
            Serial.print(ulEventsToProcess);
            Serial.println("WOW!");
         }
      }
      else
      {
         //如果出错，打印错误信息
         Serial.println("Error occur!");
      }

  }
}

/*
 * 电位器模拟信号读取函数
 * 
 */
void TaskReadAnalog(void *pvParameters)
{
  int sensorPin = A0;
  int sensorValue = 0;
    //配置内置LED
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) {
     
      sensorValue = analogRead(sensorPin);//读取电位器的值
      //如果电位器的值大于阀值
      if (sensorValue > THRESHOLD_VALUE) {
        prvBlink();//闪烁内置LED等
        xTaskNotify(taskNotificationHandler, KNOB, eSetValueWithoutOverwrite);//向事件处理任务发送KNOB事件信息
        Serial.print("Blink! Sensor: ");
        Serial.println(sensorValue);
        vTaskDelay( xAnalogReadDelay );
      }else{
        Serial.print("Sensor: ");
        Serial.println(sensorValue);
        vTaskDelay( xAnalogReadDelay );
      }
  }
}
/*
 * 事件清除函数
 * 清除事件位上的设置为0，清除代码用CRITICAL区间保护，确保清除期间事件变量不会被更新
 */
void TaskCleanUP(void *pvParameters)
{
  for(;;){
    taskENTER_CRITICAL();
    xTaskNotify(taskNotificationHandler, CLEAN, eSetValueWithOverwrite);//eAction设置为强制覆盖
    Serial.print("Clean UP ...");
    Serial.println(ulEventsToProcess);//此时的事件值为清除前的值
    taskEXIT_CRITICAL();
    vTaskDelay(xCleanUPDelay);
    
  }
}
/*
 * 主函数
 */
void setup() {
  //设置串口通许波特率9600
  Serial.begin(9600);
  // Configure pin 2 as an input and enable the internal pull-up resistor.
  pinMode(2, INPUT_PULLUP);

  // 创立主事件处理任务
  xTaskCreate(TaskHandler, // 调用函数
              "Notification", // 任务名
              128, // 栈尺寸
              NULL,//不带参数
              2, // 优先级
              &taskNotificationHandler ); // 任务句柄

  xTaskCreate(TaskReadAnalog, // 调用函数
              "AnalogRead", // 任务名
              128, // 栈尺寸
              NULL,//不带参数
              1, // 优先级
              NULL ); // TaskHandle
              
   xTaskCreate(TaskCleanUP, // 调用函数
              "Clean UP", // 任务名
              128, // 栈尺寸
              NULL,//不带参数
              3, // 优先级
              NULL ); // TaskHandle
   int digitalPin = 2;
   attachInterrupt(digitalPinToInterrupt(digitalPin), interruptHandler, CHANGE); //配置中断引脚为2，电平改变为触发
   Serial.println("Waiting for inputs!"); 
              
}
/*
 * 循环函数已经在RTOS任务中执行，此循环为空
 */
void loop() {

}
