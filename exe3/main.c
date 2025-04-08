#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"

QueueHandle_t xQueueData;

void data_task(void *p)
{
    vTaskDelay(pdMS_TO_TICKS(400)); // Espera o sistema inicializar

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++)
    {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(10)); // Evita saturar a fila
    }

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p)
{
    vTaskDelay(pdMS_TO_TICKS(500)); // Dá tempo da fila encher com os primeiros dados

    int data = 0;
    int window[5] = {0};
    int index = 0;
    int sum = 0;
    int count = 0;

    while (true)
    {
        if (xQueueReceive(xQueueData, &data, pdMS_TO_TICKS(100)))
        {
            sum -= window[index];
            window[index] = data;
            sum += window[index];
            index = (index + 1) % 5;

            if (count < 5)
                count++;

            int filtered_value = sum / count;

            printf("%d\n", filtered_value); // saída que o teste espera

            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main()
{
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));
    if (xQueueData == NULL)
    {
        while (true);
    }

    xTaskCreate(data_task, "Data task", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}