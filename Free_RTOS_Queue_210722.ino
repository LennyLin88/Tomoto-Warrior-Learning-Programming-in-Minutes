/*
 * 西红柿武士作品
 * FreeRTOS队列示例
 */

// Include Arduino FreeRTOS library
#include <Arduino_FreeRTOS.h>

// Include queue support
#include <queue.h>

void TaskPrint(void *pvParameters);
void TaskSend(void * pvParameters);
void interruptHandler();

//声明队列变量
QueueHandle_t integerQueue;

void setup() {
  //创立队列
  integerQueue = xQueueCreate(5,sizeof(unsigned char));
  //配置引脚2为中断接收端
  pinMode(2, INPUT_PULLUP);
  
  if (integerQueue != NULL) {
    
    // 创立发送任务1
    xTaskCreate(TaskSend, // 任务函数
                "Task1", // 任务名
                128,  // 任务栈尺寸
                (void*)100, //参数变量
                1, // 优先权为1
                NULL); //无须任务句柄

    // 创立发送任务2
    xTaskCreate(TaskSend, // 任务函数
                "Task2", // 任务名
                128,  // 任务栈尺寸
                (void*)200, //参数变量
                1, // 优先权为1
                NULL); //无须任务句柄
  }
    // 创立打印任务
  xTaskCreate(TaskPrint, // 任务函数
              "TaskPrint", // 任务名
              128, // 任务栈尺寸
              NULL, //参数为空
              2, // 优先权为2
              NULL ); //无须任务句柄

  //配置中断函数，绑定2号引脚，和回调函数，设定低电平出发中断信号            
  attachInterrupt(digitalPinToInterrupt(2), interruptHandler, LOW);
}

void loop() {}

//打印函数定义
void TaskPrint(void *pvParameters)
{
  //启动串口通讯窗口，设定波特率9600
  Serial.begin(9600);
  (void) pvParameters;
  //设定函数等待时间
  const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  for (;;)
  {
    //从队列接收数字的变量
    uint8_t lValueReceived;
    /*
     * 从队列读取数据并存入lValueReceived变量中，阻塞等待时间为100毫秒
     * 因为有两个任务轮流往队列输入数据，所以队列总有数据
     */
   if(xQueueReceive(integerQueue, &lValueReceived, xTicksToWait) == pdPASS)
   {
      Serial.print("Receiving: ");
      Serial.println(lValueReceived,DEC);
      vTaskDelay(1);
   }
    
  }
}

//发送数据任务
void TaskSend(void * pvParameters) {

  uint8_t lValueToSend = (uint8_t) pvParameters;

  for (;;) 
  {
  //往队列填充数据，阻塞等待时间为0，因为队列读取任务优先权为2，队列总
  //能迅速被清空
  xQueueSend(integerQueue, &lValueToSend, 0);
  vTaskDelay(50);
  }
}

//中断回调函数
void interruptHandler() {

  uint8_t value = 50; 
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdFALSE;
  //中断函数往队列前端插入数据
  xQueueSendToFrontFromISR(integerQueue, &value, &xHigherPriorityTaskWoken );

}
