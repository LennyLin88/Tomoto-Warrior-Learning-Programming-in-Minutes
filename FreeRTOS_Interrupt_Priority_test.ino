/*
 * 西红柿武士视频配套演示案例
 * 打印任务和优先权测试
 */
#include <Arduino_FreeRTOS.h>

//定义打印任务函数
void TaskPrint( void *pvParameters );
void vApplicationIdleHook( void );
volatile uint32_t ulIdleCycleCount = 0UL;

//定义静态字符串指针数组
static const char*strlist[] {
  "Task1 is running.",
  "Task2 is running."
};

void setup() {
  // 初始化串口通讯，设置波特率9600
  Serial.begin(9600); 
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  
  // 创立两个任务
   xTaskCreate(
    TaskPrint  //调用同一个任务，打印参数输入的字符串
    ,  "Task1"   // 任务1名字
    ,  128  // 任务栈深度
    ,  strlist[0] //参数传入字符串数组指针0下标的指针
    ,  2  // 优先级2
    ,  NULL );//无须操作任务句柄，设置为NULL

   xTaskCreate(
    TaskPrint   //调用同一个任务，打印参数输入的字符串
    ,  "Task2" //任务2名字
    ,  128  // Stack size
    ,  strlist[1] //参数传入字符串数组指针1下标的指针
    ,  1  // 优先级1
    ,  NULL );  //无须操作任务句柄，设置为NULL

}
    

void loop()
{
  // Empty. Things are done in Tasks.
}

//打印函数
void TaskPrint(void* pvParameters){
/*
 * 打印参数传递的字符串
*/
  char* str = (char*) pvParameters;
   for (;;) //无限循环
   {
    Serial.print("Idle cycles: ");
    Serial.println(ulIdleCycleCount++);//打印空闲任务调用计数器
    Serial.println(str);  //打印传递来的参数字符串
    vTaskDelay(50);//任务延迟中断阻塞
    }
}

//空闲任务回调函数
void vApplicationIdleHook( void )
{
    ulIdleCycleCount++;
}
