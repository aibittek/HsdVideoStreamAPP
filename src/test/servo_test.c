
#include <delay.h>
#include <servo_core.h>

void servo_test()
{
    int i;
    int count = 10;
    int speed = 10;
    
    // 电机启动
    start_servo();
    
    // 设置电机按照固定角度转动
    servo_angle(0);
    delay_ms(1000);
    servo_angle(45);
    delay_ms(1000);
    servo_angle(90);
    delay_ms(1000);
    servo_angle(135);
    delay_ms(1000);
    servo_angle(180);
    delay_ms(1000);
    
    while (count) {
        // 电机启动
        start_servo();
        // 正向转动180度
        for (i = 0; i <= 180; i++) {
            servo_angle(i);
            delay_ms(speed);
        }
        // 反向转动180度
        for (i = 180; i >= 0; i--) {
            servo_angle(i);
            delay_ms(speed);
        }
        count--;        ///< 控制转动次数
        speed--;        ///< 控制转动速度
        
        // 电机停止
        stop_servo();
    }
}
