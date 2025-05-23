#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"

const int PWM_0_PIN = 4; // LED R (80%)
const int PWM_1_PIN = 5; // LED G (20%)

void led_task(void *p)
{
    gpio_set_function(PWM_0_PIN, GPIO_FUNC_PWM);
    gpio_set_function(PWM_1_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(PWM_0_PIN);

    // Diminui a frequência para facilitar detecção no Wokwi
    pwm_set_clkdiv(slice_num, 250.0f);  // divisor maior = frequência menor
    pwm_set_wrap(slice_num, 100);       // 100 "contagens" por ciclo

    pwm_set_chan_level(slice_num, PWM_CHAN_A, 80); // LED R - 80%
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 20); // LED G - 20%

    pwm_set_enabled(slice_num, true);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}