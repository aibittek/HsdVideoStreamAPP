#include <zephyr/zephyr.h>
#include <zephyr/kernel.h>
#include <delay.h>

int32_t delay_ms(int32_t ms)
{
    return k_msleep(ms);
}


int32_t delay_us(int32_t us)
{
    return k_usleep(us);
}
