/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

//
// F303_comm_test : Test bus connection to FPGA
//
#include "mbed.h"
#include "FPGA_bus.h"

enum {TEST_PWM_1, TEST_PWM_2, TEST_SERVO_1, TEST_SERVO_2, TEST_SPEED_MEASURE, TEST_H_BRIDGE_1};

uint32_t test_number;

FPGA_bus  bus(1,1,8);

//SerialDriver pc(USBTX, USBRX);FileHandle *mbed::mbed_override_console(int fd)

BufferedSerial pc(PB_6, PB_7);

FileHandle *mbed::mbed_override_console(int fd) {

    return &pc;
}


void prog_init(void) {
    pc.set_baud(115200);
    printf("F303 FPGA comms test.\n");
}

//
// Main code

int main() {

uint32_t    channel, pulse_width_uS, status;

    prog_init();
    status = bus.initialise();
    if (status != NO_ERROR) {
        printf("Init error = %d\n", status);
        LOOP_HERE;
    }
    ThisThread::sleep_for(1s);
    printf("Started.....\n");
    test_number = TEST_SERVO_2;

    switch (test_number) {
        case TEST_PWM_1 : {
                bus.set_PWM_period(0, 5.0 /* KHz */);  // PWM_ch0
                bus.set_PWM_duty(0, 50 /* % */);
                bus.PWM_config(0, (PWM_ON + INT_H_BRIDGE_ON + MOTOR_FORWARD));

                if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    printf(" PWM Test 1 : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    printf(" PWM Test 1 : complete with no errors\n");
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
                   ThisThread::sleep_for(1s);
                }
                for (int i=100 ; i >= 0 ; i--) {
                   bus.set_PWM_duty(0, i /* % */);
                   bus.PWM_config(0, (PWM_ON + INT_H_BRIDGE_ON + MOTOR_FORWARD));
                   ThisThread::sleep_for(1s);
                }
                bus.PWM_config(0, PWM_OFF);
                if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    printf(" PWM Test 2 : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    printf(" PWM Test 2 : complete with no errors\n");
                }
                break;
        }
        case TEST_SERVO_1 : {
                bus.set_RC_period();
                channel = 0; pulse_width_uS = 1500;
                bus.set_RC_pulse(channel, pulse_width_uS);
                bus.enable_RC_channel(channel); printf("config = %d\n", bus.global_FPGA_unit_error_flag);
                ThisThread::sleep_for(20ms);
                bus.disable_RC_channel(channel);
                 if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    printf(" SERVO Test 1 : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    printf(" SERVO Test 1 : complete with no errors\n");
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
                    ThisThread::sleep_for(1s);
                }
                if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    printf(" SERVO Test 2 : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    printf(" SERVO Test 2 : complete with no errors\n");
                }
                break;
        }
        case TEST_SPEED_MEASURE : {
                channel = 0;
                // bus.enable_speed_measure(channel);
                if (bus.global_FPGA_unit_error_flag != NO_ERROR) {
                    printf(" Speed Test : error :: %d\n", bus.global_FPGA_unit_error_flag);
                    bus.global_FPGA_unit_error_flag = NO_ERROR;
                } else {
                    printf(" Speed Test : complete with no errors\n");
                }
                break;
        default :
                printf("Unknown test\n");
                break;
        }
    }
    LOOP_HERE;
}