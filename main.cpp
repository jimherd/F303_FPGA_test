/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

//
// F302_comm_test : Test bus connection to FPGA
//
#include "mbed.h"
#include "FPGA_bus.h"
#include "SerialDriver.h"

enum {TEST_PWM_1, TEST_PWM_2, TEST_SERVO_1, TEST_SERVO_2, TEST_SPEED_MEASURE, TEST_H_BRIDGE_1};

uint32_t test_number;

FPGA_bus  bus(1,1,8);

//SerialDriver pc(USBTX, USBRX);
SerialDriver pc(PB_6, PB_7);

void prog_init(void)
{
    pc.baud(115200);
    pc.printf("F302 comms test.\n");
}

//
// Main code

int main() {

uint32_t    channel, pulse_width_uS;

    prog_init();
    bus.initialise();
    ThisThread::sleep_for(10000);
    pc.printf("Started.....\n");
    test_number = TEST_PWM_1;

    //
    // read system data from FPGA board

    uint32_t sys_data = bus.get_SYS_data();

    pc.printf("Major version number   = %d\n", sys_data & 0x0000000F);
    pc.printf("Minor version number   = %d\n", ((sys_data >> 4) & 0x0000000F));
    pc.printf("Number of PWM channels = %d\n", ((sys_data >> 8) & 0x0000000F));
    pc.printf("Number of QE channels  = %d\n", ((sys_data >> 12) & 0x0000000F));
    pc.printf("Number of RC channels  = %d\n", ((sys_data >> 16) & 0x0000000F));

    switch (test_number) {
        case TEST_PWM_1 : {
                bus.set_PWM_period(0, 5.0 /* KHz */);  // PWM_ch0
                bus.set_PWM_duty(0, 50 /* % */);
                bus.PWM_config(0, (PWM_ON + INT_H_BRIDGE_ON + MOTOR_FORWARD));

                if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    pc.printf(" PWM Test 1 : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    pc.printf(" PWM Test 1 : complete with no errors\n");
                }
                break;
        }
        case TEST_PWM_2 : {
                bus.set_PWM_period(0, 5.0 /* KHz */);
                bus.set_PWM_duty(0, 0 /* % */);
                bus.PWM_config(0, (PWM_ON + INT_H_BRIDGE_ON + MOTOR_FORWARD));
                for (int i=0 ; i < 101 ; i++) {
                   bus.set_PWM_duty(0, i /* % */);
                   bus.PWM_config(0, (PWM_ON + INT_H_BRIDGE_ON + MOTOR_FORWARD));
                   ThisThread::sleep_for(1000);
                }
                for (int i=100 ; i >= 0 ; i--) {
                   bus.set_PWM_duty(0, i /* % */);
                   bus.PWM_config(0, (PWM_ON + INT_H_BRIDGE_ON + MOTOR_FORWARD));
                   ThisThread::sleep_for(1000);
                }
                bus.PWM_config(0, PWM_OFF);
                if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    pc.printf(" PWM Test 2 : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    pc.printf(" PWM Test 2 : complete with no errors\n");
                }
                break;
        }
        case TEST_SERVO_1 : {
                bus.set_RC_period();
                channel = 0; pulse_width_uS = 1500;
                bus.set_RC_pulse(channel, pulse_width_uS);
                bus.enable_RC_channel(channel); pc.printf("config = %d\n", bus.global_FPGA_unit_error_flag);
                ThisThread::sleep_for(20);
                bus.disable_RC_channel(channel);
                 if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    pc.printf(" SERVO Test 1 : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    pc.printf(" SERVO Test 1 : complete with no errors\n");
                }
                break;
        }
        case TEST_SERVO_2 : {
                bus.set_RC_period();
                channel = 0;
                bus.enable_RC_channel(channel);
                for (int step=0; step<=10; step++) {
                    pulse_width_uS = 1000 + (100 * step);
                    bus.set_RC_pulse(channel, pulse_width_uS);
                    ThisThread::sleep_for(1000);
                }
                if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    pc.printf(" SERVO Test 2 : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    pc.printf(" SERVO Test 2 : complete with no errors\n");
                }
                break;
        }
        case TEST_SPEED_MEASURE : {
                channel = 0;
                // bus.enable_speed_measure(channel);
                if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    pc.printf(" Speed Test : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    pc.printf(" Speed Test : complete with no errors\n");
                }
                break;
        default :
                pc.printf("Unknown test\n");
                break;
        }
    }
    LOOP_HERE;
}