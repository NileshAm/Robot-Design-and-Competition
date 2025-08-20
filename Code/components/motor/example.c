#include "encoder_pcnt.h"
#include "motor.h"

#define CPR_OUTPUT 11*75*4  // Counts per revolution for the motor

void app_main(void)
{
    // Initialize the encoder for the right motor
    encoder_pcnt_handle_t enc_r, enc_l;
    encoder_pcnt_create(GPIO_NUM_34, GPIO_NUM_35, &enc_r);
    encoder_pcnt_create(GPIO_NUM_32, GPIO_NUM_33, &enc_l);

    // Create motor handles for the right and left motors
    motor_handle_t m_r, m_l;
    motor_create(&(motor_config_t){ enc_r, CPR_OUTPUT }, &m_r);
    motor_create(&(motor_config_t){ enc_l, CPR_OUTPUT }, &m_l);

    while (1) {
        float rpm_r, rpm_l;
        int64_t t_r, t_l;

        // Get RPM and tick counts for both motors
        motor_get_speed_rpm(m_r, &rpm_r);
        motor_get_speed_rpm(m_l, &rpm_l);
        motor_get_tick_count_chA(m_r, &t_r);
        motor_get_tick_count_chB(m_r, &t_r);
        motor_get_tick_count_chA(m_l, &t_l);
        motor_get_tick_count_chB(m_l, &t_l);

        // Print the results
        printf("Right Motor: RPM=%.2f, Ticks A=%lld, Ticks B=%lld\n", rpm_r, t_r, t_r);
        printf("Left Motor: RPM=%.2f, Ticks A=%lld, Ticks B=%lld\n", rpm_l, t_l, t_l);

        delay_s(1);  // Delay for 1 second before the next iteration
    }
}