#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p)
{
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++)
    {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p)
{
    int data = 0;
    int window[5] = {0}; // Janela de tamanho 5
    int index = 0;
    int sum = 0;
    int count = 0;

    while (true)
    {
        if (xQueueReceive(xQueueData, &data, 100))
        {
            // Atualiza a soma e a janela
            sum -= window[index];
            window[index] = data;
            sum += window[index];
            index = (index + 1) % 5;

            // Incrementa o contador até preencher a janela
            if (count < 5)
            {
                count++;
            }

            // Calcula a média móvel
            int filtered_value = sum / count;

            // Imprime o valor filtrado na UART
            printf("%d\n", filtered_value);

            // Deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main()
{
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
