/* 
 * FPGA_bus  :  8-bit bi-directional bus between uP and FPGA
 *
 * Author : Jim Herd
 *
 * Version 0.1 : initial release
 */
#include "mbed.h"
 
#ifndef  FPGA_bus_H
#define  FPGA_bus_H
 
//
// Pin definitions

#define ASYNC_UP_RW_PIN           PB_0
#define ASYNC_UP_HANDSHAKE_1_PIN  PB_1
#define ASYNC_UP_HANDSHAKE_2_PIN  PB_2
#define ASYNC_UP_START_PIN        PB_3
#define ASYNC_UP_ACK_PIN          PB_4
#define ASYNC_UP_RESET_PIN        PB_5

#define LOG_PIN                   PB_8

//
// 
#define PWM_BASE            1
#define NOS_PWM_REGISTERS   4
#define NOS_PWM_CHANNELS    1

#define QE_BASE             ((NOS_PWM_REGISTERS * NOS_PWM_CHANNELS) + PWM_BASE)
#define NOS_QE_REGISTERS    7
#define NOS_QE_CHANNELS     2

#define RC_BASE             ((NOS_QE_REGISTERS * NOS_QE_CHANNELS) + QE_BASE)
#define NOS_RC_CHANNELS     8
#define GLOBAL_RC_ENABLE    0x80000000

#define PWM_ch0             (PWM_BASE + (0 * NOS_PWM_REGISTERS))
#define PWM_ch1             (PWM_BASE + (1 * NOS_PWM_REGISTERS))
#define PWM_ch3             (PWM_BASE + (3 * NOS_PWM_REGISTERS))

#define RC_0                RC_BASE

//
// System can be configured to return ONE or TWO 32-bit values from the FPGA.
//
//      first value  : 32-bit data value
//      second value : 32-bit status value
//
// In practice, the status word carries little or no information but consumes
// four 8-bit transactions between the FPGA and the uP.
//
// Uncomment following #define to enable status word to be returned.

//#define INCLUDE_32_BIT_STATUS_RETURN

#ifdef INCLUDE_32_BIT_STATUS_RETURN
    #define NOS_RECEIVED_PACKET_WORDS  2
#else
    #define NOS_RECEIVED_PACKET_WORDS  1
#endif

#define SET_BUS_INPUT             (GPIOC->MODER = (GPIOC->MODER & 0xFFFF0000))
#define SET_BUS_OUTPUT            (GPIOC->MODER = ((GPIOC->MODER & 0xFFFF0000) | 0x00005555))
#define OUTPUT_BYTE_TO_BUS(value) (GPIOC->ODR = ((GPIOC->ODR & 0x0000FF00) | (value & 0x000000FF)))
#define INPUT_BYTE_FROM_BUS       (GPIOC->IDR & 0x000000FF)
#define ENABLE_GPIO_SUBSYSTEM     (RCC->AHBENR |= RCC_AHBENR_GPIOCEN)

// 
// FPGA constants

#define     nS_IN_uS                1000
#define     FPGA_CLOCK_PERIOD_nS    20

//
// error codes

#define  NO_ERROR   0

#define LOOP_HERE     for(;;)

typedef struct {
    uint8_t     command;
    uint8_t     register_no;
    uint32_t    cmd_data;
    uint32_t    reply_data;
    uint32_t    reply_status;
} FPGA_packet_t;

typedef union {
    uint32_t word_data[2];    // NOS_RECEIVED_PACKET_WORDS];
    uint8_t  byte_data[8];    // NOS_RECEIVED_PACKET_WORDS << 2];
} received_packet_t;

enum {READ_REGISTER_CMD=0, WRITE_REGISTER_CMD=1};
enum {READ_BUS=0, WRITE_BUS=1};
enum {LOW=0, HIGH=1};

//
// SYS_data registers

enum {SYS_DATA_REG_ADDR=0};

// PWM registers
//
enum {PWM_PERIOD=0, PWM_ON_TIME=1, PWM_CONFIG=2, PWM_STATUS=3};

// QE registers
//
enum {QE_COUNT_BUFFER=0, QE_TURN_BUFFER=1, QE_SPEED_BUFFER=2, QE_SIM_PHASE_TIME=3,
      QE_COUNTS_PER_REV=4, QE_CONFIG=5, QE_STATUS=6};
//
// constants to define bits in QE config register

#define  QE_CONFIG_DEFAULT    0x00
enum {QE_SIG_EXT=0x00, QE_SIG_INT_SIM=0x02};
enum {QE_INT_SIM_DISABLE=0x0, QE_INT_SIM_ENABLE=0x04};
enum {QE_SIM_DIR_FORWARD=0x0, QE_SIM_DIR_BACKWARD=0x08};
enum {QE_NO_SWAP_AB=0x00, QE_SWAP_AB=0x10};
enum {QE_SPEED_CALC_DISABLE=0x00, QE_SPEED_CALC_ENABLE=0x10000};
enum {QE_SPEED_CALC_FILTER_DISABLE=0x00, QE_SPEED_CALC_FILTER_ENABLE=0x20000};
enum {QE_FILTER_SAMPLE_2=0x00, QE_FILTER_SAMPLE_4=0x100000, QE_FILTER_SAMPLE_8=0x200000,
      QE_FILTER_SAMPLE_16=0x300000, QE_FILTER_SAMPLE_32=0x400000};
     
// RC servo registers
//
enum {RC_SERVO_PERIOD=0, RC_SERVO_CONFIG=1, RC_SERVO_STATUS=2, RC_SERVO_ON_TIME=3};

//
// constants to define bits in PWM config register

#define  PWM_CONFIG_DEFAULT    0x00
enum {PWM_OFF=0x0, PWM_ON=0x1};
enum {INT_H_BRIDGE_OFF=0x0, INT_H_BRIDGE_ON=0x10000};
enum {EXT_H_BRIDGE_OFF=0x0, EXT_H_BRIDGE_ON=0x20000};
enum {MOTOR_COAST=0x0, MOTOR_FORWARD=0x40000, MOTOR_BACKWARD=0x80000, MOTOR_BRAKE=0xC0000};
enum {MODE_PWM_CONTROL=0x0, MODE_DIR_CONTROL=0x200000};
enum {NO_SWAP=0x0, YES_SWAP=0x800000};
enum {PWM_BRAKE_DWELL=0x0, PWM_COAST_DWELL=0x1000000};
enum {NO_INVERT = 0x0, H_BRIDGE_1_INVERT=0x2000000, H_BRIDGE_2_INVERT=0x4000000, ALL_INVERT=0x6000000};
enum {BACKWARD, FORWARD};

//DigitalOut async_uP_start(ASYNC_UP_START_PIN, LOW);
//DigitalOut async_uP_handshake_1(ASYNC_UP_HANDSHAKE_1_PIN, LOW);
//DigitalOut async_uP_RW(ASYNC_UP_RW_PIN, LOW);
//DigitalOut async_uP_reset(ASYNC_UP_RESET_PIN, HIGH);
//DigitalIn  uP_ack(ASYNC_UP_ACK_PIN, PullDown);
//DigitalIn  uP_handshake_2(ASYNC_UP_HANDSHAKE_2_PIN);
    
class FPGA_bus {
public:
 //    FPGA_bus(void);                                        // constructor
     FPGA_bus(int nos_PWM    = NOS_PWM_CHANNELS , 
              int nos_QE     = NOS_QE_CHANNELS  , 
              int nos_servo  = NOS_RC_CHANNELS );  // constructor
 
     
     void     initialise(void); 
    void do_transaction(uint32_t command, 
                        uint32_t register_address, 
                        uint32_t register_data,
                        uint32_t *data,
                        uint32_t *status);
     uint32_t do_command(FPGA_packet_t cmd_packet);
     void write_register(uint32_t register_addr, uint32_t value);
     void set_PWM_period(uint32_t channel, float frequency);
     void set_PWM_duty(uint32_t channel, float percentage);
     void PWM_enable(uint32_t channel);
     void PWM_config(uint32_t channel, uint32_t config_value);
     void set_RC_period(void);
     void set_RC_period(uint32_t duty_uS);
     void set_RC_pulse(uint32_t channel, uint32_t pulse_uS);
     void enable_RC_channel(uint32_t channel);
     void disable_RC_channel(uint32_t channel);
     void QE_config(uint32_t channel, uint32_t config_value);
     void enable_speed_measure(uint32_t channel, uint32_t config_value, uint32_t phase_time);
     uint32_t read_speed_measure(uint32_t channel);
     uint32_t read_count_measure(uint32_t channel);
     
     uint32_t get_SYS_data(void);
     
     int32_t global_FPGA_unit_error_flag;
     
protected:
    uint32_t    _nos_PWM_units;
    uint32_t    _nos_QE_units;
    uint32_t    _nos_servo_units;
    
private:
    uint32_t data, status, tmp_config;
    received_packet_t   in_pkt;

    void do_start(void);
    void do_end(void);
    void write_byte(uint32_t byte_value);
    uint32_t read_byte(void);
    void do_write(  uint32_t command, 
                    uint32_t register_address, 
                    uint32_t register_data);
    void do_read(received_packet_t   *buffer);
    
    DigitalOut async_uP_start;
    DigitalOut async_uP_handshake_1;
    DigitalOut async_uP_RW;
    DigitalOut async_uP_reset;
    DigitalIn  uP_ack;
    DigitalIn  uP_handshake_2;
    
    DigitalOut log_pin;
    
    struct SYS_data {
        uint8_t major_version;
        uint8_t minor_version;
        uint8_t number_of_PWM_channels;
        uint8_t number_of_QE_channels;
        uint8_t number_of_RC_channels;
        uint8_t PWM_period_value[NOS_PWM_CHANNELS];
        uint8_t PWM_duty_value[NOS_PWM_CHANNELS];
        uint8_t pad1;
    } sys_data;
};
 
 #endif
 