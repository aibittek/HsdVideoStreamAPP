#include <zephyr/kernel.h>
#include <delay.h>
#include <led_core.h>

void led_test()
{
    led.on(GREEN_LED);
    delay_ms(2000);
    
    led.off(GREEN_LED);
    delay_ms(2000);

    // 绿色LED每隔100ms闪烁一次，共闪烁5秒钟的时间
    led.blink(GREEN_LED, 100, 5000);
    delay_ms(5000);
    // 绿色LED每隔1000ms呼吸一次，共呼吸5秒钟的时间
    led.breathing(GREEN_LED, 1000, 0);
}
