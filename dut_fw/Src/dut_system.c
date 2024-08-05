#include "dut_system.h"

#include "main.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#define LED1_PORT LD2_GPIO_Port
#define LED1_PIN  LD2_Pin

#define KB_PORT B1_GPIO_Port
#define KB_PIN  B1_Pin

// Queue handlers
const UBaseType_t QUEUE_PC_REQUESTS_SIZE = 10;
QueueHandle_t     xQueuePcRequests;

// Timer handlers
const TickType_t led1_blinky_interval = pdMS_TO_TICKS(5000);
TimerHandle_t    xTimerLed1Blink;

const TickType_t kb_interval = pdMS_TO_TICKS(10000);
TimerHandle_t    xTimerKeyButtonRead;

// Task handlers
TaskHandle_t xTaskLedBlink;
TaskHandle_t xTaskKeyBoardRead;

// Hardcoded serial number
const char my_serial_number[] = {"test_dut_1"};

// Hardcoded DUT replies
const uint8_t reply_ok   = 0;
const uint8_t reply_fail = 1;

// Temporal data buffer for USART receiver
uint8_t uart_rx_data;

// Temporal data buffer for KB readings
uint8_t kb_press_counter = 0;

// External link to UART2 handler
extern UART_HandleTypeDef huart2;

void vLed1TimerCallback(TimerHandle_t xTimer)
{
    // Stop blinky
    vTaskDelete(xTaskLedBlink);
    // LED off
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
    // Send "OK"
    HAL_UART_Transmit(
        &huart2, (uint8_t*)&reply_ok, sizeof(reply_ok), HAL_MAX_DELAY
    );
}

void vKbTimerCallback(TimerHandle_t xTimer)
{
    // Stop kb read
    vTaskDelete(xTaskKeyBoardRead);
    // Send number of KB press
    HAL_UART_Transmit(
        &huart2, &kb_press_counter, sizeof(uint8_t), HAL_MAX_DELAY
    );
}

void vTaskBlinkLed1(void* pvParameters)
{
    xTimerReset(xTimerLed1Blink, 0);

    for (;;) {
        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET);
        vTaskDelay(250);
        HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
        vTaskDelay(250);
    }

    vTaskDelete(NULL);
}

void vTaskKeyButton(void* pvParameters)
{
    kb_press_counter = 0;
    xTimerReset(xTimerKeyButtonRead, 0);

    for (;;) {
        /* Read key status in polling mode */
        if (HAL_GPIO_ReadPin(KB_PORT, KB_PIN) == GPIO_PIN_RESET) {
            vTaskDelay(20);

            while (HAL_GPIO_ReadPin(KB_PORT, KB_PIN) == GPIO_PIN_RESET) {};

            vTaskDelay(20);

            kb_press_counter++;
        }
    }

    vTaskDelete(NULL);
}

void vTaskTerminal(void* pvParameters)
{
    RequestsTypedef pc_msg;
    for (;;) {
        if (xQueueReceive(xQueuePcRequests, &pc_msg, portMAX_DELAY) == pdPASS) {
            switch (pc_msg) {
                case REQUEST_SERIAL_NUMBER:
                    HAL_UART_Transmit(
                        &huart2,
                        (uint8_t*)my_serial_number,
                        sizeof(my_serial_number),
                        HAL_MAX_DELAY
                    );
                    break;

                case REQUEST_LINE_STATUS:
                    if (HAL_GPIO_ReadPin(GPIO_IN1_GPIO_Port, GPIO_IN1_Pin) ==
                        GPIO_PIN_SET) {
                        // Do blinky
                        xTaskCreate(
                            vTaskBlinkLed1,
                            "LedTask",
                            configMINIMAL_STACK_SIZE,
                            NULL,
                            1,
                            &xTaskLedBlink
                        );
                    }
                    else {
                        // Send "FAIL"
                        HAL_UART_Transmit(
                            &huart2,
                            (uint8_t*)&reply_fail,
                            sizeof(reply_fail),
                            HAL_MAX_DELAY
                        );
                    }
                    break;

                case REQUEST_KB_PRESS:
                    xTaskCreate(
                        vTaskKeyButton,
                        "KbTask",
                        configMINIMAL_STACK_SIZE,
                        NULL,
                        1,
                        &xTaskKeyBoardRead
                    );
                    break;

                default: break;
            }
        }
    }
}

void dut_init(void)
{
    // Queue init
    xQueuePcRequests =
        xQueueCreate(QUEUE_PC_REQUESTS_SIZE, sizeof(RequestsTypedef));

    // Timers init
    xTimerLed1Blink = xTimerCreate(
        "TimLed1Blink", led1_blinky_interval, pdFALSE, 0, vLed1TimerCallback
    );
    xTimerKeyButtonRead =
        xTimerCreate("TimKbRead", kb_interval, pdFALSE, 0, vKbTimerCallback);

    // Tasks init
    xTaskCreate(
        vTaskTerminal, "TerminalTask", configMINIMAL_STACK_SIZE, NULL, 1, NULL
    );

    // Start listen requests from PC
    HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);

    // Start FreeRTOS scheduler
    vTaskStartScheduler();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    BaseType_t xHigherPriorityTaskWoken;

    if (huart->Instance == USART2) {
        RequestsTypedef msg = uart_rx_data - 0x30;
        xQueueSendFromISR(xQueuePcRequests, &msg, &xHigherPriorityTaskWoken);
        HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}