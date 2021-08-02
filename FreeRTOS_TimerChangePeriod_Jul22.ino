/*
 * FreeRTOS改变时钟周期示例
 * 西红柿武士作品
 */

// Include Arduino FreeRTOS library and <timers.h>
#include <Arduino_FreeRTOS.h>
#include <timers.h>
//定义正常闪烁间隔3秒种
#define setup_NORMAL_TIMER_PERIOD pdMS_TO_TICKS(3000)
//定义快闪间隔0。5秒种
#define task_FAST_TIMER_PERIOD pdMS_TO_TICKS(500)
#define interrupt_MAX_COUNTS 20

//声明TimerCalback函数和中断函数
void prvTimerCallback( TimerHandle_t xTimer);
void interruptHandler(void); 

//声明计时器句柄
TimerHandle_t xTimer;
void setup() {
  //开启串口通讯配置
   Serial.begin(9600);
  //声明计时器启动变量
  BaseType_t xTimerStarted;
  //配置2号引脚为输入，并上拉电平
  pinMode(2, INPUT_PULLUP);

 // 创立定时器
  xTimer = xTimerCreate(
              "Normal Condition", // 名字
              setup_NORMAL_TIMER_PERIOD, // 时间周期
              pdTRUE, // 是否循环  
              0, // 计时器ID
              prvTimerCallback );//回调函数
              
  if ( xTimer != NULL)
  { 
    xTimerStarted = xTimerStart(xTimer,0);//启动计时器
    Serial.println("Start normal speed blink");
  }
  /**
   *配置中断函数，提供2号引脚和中断回调函数，低电平为中断信号
   */

    attachInterrupt(digitalPinToInterrupt(2), interruptHandler, LOW);

}

void loop() {}

/* 中断回调函数
 * 根据设定的中断最大次数值来判断是否发出计时器终止命令，如果在中断最大值内
 * 回调函数将调整计时器的周期，如果超出，则终止计时器，需要重置来重启程序
 * 利用了Timer ID来存储中断调用值
 */

void interruptHandler(void) {

/*  
 * 利用TimeID的数据存储功能  
  //中断次数计数器变量
  uint32_t ulExecutionCount; 
  //读取计时器的Timer ID中存储的中断计数器的值
  ulExecutionCount = ( uint32_t ) pvTimerGetTimerID( xTimer );
  //取值加一
  ulExecutionCount++; 
  //存储中断计数器的新值到计时器的Time ID中
  vTimerSetTimerID( xTimer, ( void * ) ulExecutionCount );
*/
  //设置静态变量实现TIMEID存储数据同样的功能

  static uint32_t ulExecutionCount = 0; 
  ulExecutionCount++; 

  //设置更高优先权变量为pdFALSE
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if ( ulExecutionCount < interrupt_MAX_COUNTS )
  {  
    //如果中断计数器的值小于设定的最大值，中断发出命令改变计时器周期为快速闪烁，
    //并打印中断的次数
    
     xTimerChangePeriodFromISR(xTimer, task_FAST_TIMER_PERIOD,NULL);
     Serial.print("Interrupt occurs: ");
     Serial.print(ulExecutionCount);
     Serial.println("times!");
  }else 
  {
     //如果中断次数超过设定最大值，将发出终止计时器任务，如果失败，打印输出
     if (xTimerStopFromISR( xTimer,xHigherPriorityTaskWoken) != pdPASS )
     {
      Serial.println("Timer stop failed");
     }else{
      Serial.println("Too many interrupts, please reset to restart!");
     
     } 
   }
}




/* 
 * 计时器回调函数
 * 闪烁LED一次，点亮和熄灭的间隔为0。5秒
 */
void prvTimerCallback( TimerHandle_t xTimer)
{
  //配置内置LED
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for 0.5 second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for 0.5 second

}
