/*
 * FPGA_bus : 8-bit bi-directional bus between uP and FPGA
 */
#include "mbed.h"
#include "FPGA_bus.h"

/** create a FPGA_bus object connecting uP to FPGA
 *
 * @param  nos_PWM      Number of PWM channels (default = 4)
 * @param  nos_QE       Number of Quadrature Encoder channels (default = 4)
 * @param  nos_servo    Number of RC servo channels (default = 8)
 *
 * Notes
 *      You can only change the defaults by recompiling the SystemVerilog code
 *      on the FPGA.
<<<<<<< working copy
 */ 
 
 /*
=======
 */  /*
FPGA_bus::FPGA_bus(int nos_PWM   = NOS_PWM_CHANNELS, 
                   int nos_QE    = NOS_QE_CHANNELS, 
                   int nos_servo = NOS_RC_CHANNELS)
                   
                :   async_uP_start(ASYNC_UP_START_PIN), 
                    async_uP_handshake_1(ASYNC_UP_HANDSHAKE_1_PIN),
                    async_uP_RW(ASYNC_UP_RW_PIN),
                    async_uP_reset(ASYNC_UP_RESET_PIN),
                    uP_ack(ASYNC_UP_ACK_PIN),
                    uP_handshake_2(ASYNC_UP_HANDSHAKE_2_PIN), 
                    log_pin(LOG_PIN)                 
{
    _nos_PWM_units   = nos_PWM;
    _nos_QE_units    = nos_QE;
    _nos_servo_units = nos_servo;
    
    async_uP_start   = LOW;
}

>>>>>>> merge rev
FPGA_bus::FPGA_bus(void)
                :   async_uP_start(ASYNC_UP_START_PIN), 
                    async_uP_handshake_1(ASYNC_UP_HANDSHAKE_1_PIN),
                    async_uP_RW(ASYNC_UP_RW_PIN),
                    async_uP_reset(ASYNC_UP_RESET_PIN),
                    uP_ack(ASYNC_UP_ACK_PIN),
                    uP_handshake_2(ASYNC_UP_HANDSHAKE_2_PIN),
                    log_pin(LOG_PIN)                  
{
    _nos_PWM_units   = NOS_PWM_CHANNELS;
    _nos_QE_units    = NOS_QE_CHANNELS;
    _nos_servo_units = NOS_RC_CHANNELS;
    
    async_uP_start       = LOW;
}
 */
FPGA_bus::FPGA_bus(int nos_PWM   /* = NOS_PWM_CHANNELS */, 
                   int nos_QE    /* = NOS_QE_CHANNELS  */, 
                   int nos_servo /* = NOS_RC_CHANNELS  */ )
                   
                :   
                    _nos_PWM_units(nos_PWM),
                    _nos_QE_units(nos_QE),
                    _nos_servo_units(nos_servo),   
                                            
                    async_uP_start(ASYNC_UP_START_PIN), 
                    async_uP_handshake_1(ASYNC_UP_HANDSHAKE_1_PIN),
                    async_uP_RW(ASYNC_UP_RW_PIN),
                    async_uP_reset(ASYNC_UP_RESET_PIN),
                    uP_ack(ASYNC_UP_ACK_PIN),
                    uP_handshake_2(ASYNC_UP_HANDSHAKE_2_PIN),
                    log_pin(LOG_PIN)
                    

{
  /*  _nos_PWM_units   = nos_PWM;
    _nos_QE_units    = nos_QE;
    _nos_servo_units = nos_servo;  */
    
    async_uP_start   = LOW;
}


void  FPGA_bus::initialise(void)
{
    // GPIOC Periph clock enable
    
    ENABLE_GPIO_SUBSYSTEM;
    wait_us(2);
    
    async_uP_start       = LOW;
    async_uP_reset       = HIGH;
    async_uP_handshake_1 = LOW;
    async_uP_RW          = LOW;
    
    async_uP_reset = LOW;       // generate low reset pulse
    wait_us(20);
    async_uP_reset = HIGH;
    wait_us(20);
    
    global_FPGA_unit_error_flag = NO_ERROR;
    log_pin = LOW;
}
    
void FPGA_bus::do_start(void)
{
    async_uP_start       = HIGH;
    async_uP_handshake_1 = LOW;
    async_uP_start       = LOW;
}

void FPGA_bus::do_end(void)
{
    while (uP_ack == HIGH)
        ;
    async_uP_start = LOW;
}

void FPGA_bus::write_byte(uint32_t byte_value)
{
    SET_BUS_OUTPUT;
    OUTPUT_BYTE_TO_BUS(byte_value);
    async_uP_RW = WRITE_BUS;
    async_uP_handshake_1 = HIGH;
    while (uP_handshake_2 == LOW)
        ;
    async_uP_handshake_1 = LOW;
    while (uP_handshake_2 == HIGH)
        ;
}

uint32_t FPGA_bus::read_byte(void)
{
    SET_BUS_INPUT;
    async_uP_RW = READ_BUS;
    while (uP_handshake_2 == LOW)
        ;
    data = INPUT_BYTE_FROM_BUS;
    async_uP_handshake_1 = HIGH;
    while (uP_handshake_2 == HIGH)
        ;
    async_uP_handshake_1 = LOW;
    return data;
}

void FPGA_bus::do_write(uint32_t command, 
              uint32_t register_address, 
              uint32_t register_data)
{
    write_byte(command);
    write_byte(register_address);
    write_byte(register_data);
    write_byte(register_data>>8);
    write_byte(register_data>>16);
    write_byte(register_data>>24);
}

void FPGA_bus::do_read(received_packet_t   *buffer)
{
    for (int i=0; i < (NOS_RECEIVED_PACKET_WORDS<<2) ; i++) {
        buffer->byte_data[i] = (uint8_t)read_byte();
    }
}

void FPGA_bus::do_transaction(uint32_t command, 
                    uint32_t register_address, 
                    uint32_t register_data,
                    uint32_t *data,
                    uint32_t *status)
{
    log_pin = HIGH;
    do_start();
    do_write(command, register_address, register_data);
    do_read(&in_pkt);
    do_end();
    log_pin = LOW;
    *data = in_pkt.word_data[0];
    *status = in_pkt.word_data[1];
}

uint32_t  FPGA_bus::do_command(FPGA_packet_t cmd_packet)
{
    async_uP_start = LOW;
    return 0;
}
    
void FPGA_bus::set_PWM_period(uint32_t channel, float frequency)
{
    uint32_t register_address = ((PWM_BASE + (channel * NOS_PWM_REGISTERS)) + PWM_PERIOD);
    uint32_t period_value = (uint32_t)(1000000/(20 * frequency));
    do_transaction(WRITE_REGISTER_CMD, register_address, period_value, &data, &status); 
    sys_data.PWM_period_value[channel] = period_value;
    global_FPGA_unit_error_flag = status;
}

void FPGA_bus::set_PWM_duty(uint32_t channel, float percentage)
{
    uint32_t register_address = ((PWM_BASE + (channel * NOS_PWM_REGISTERS)) + PWM_ON_TIME);
    uint32_t duty_value = (uint32_t)((sys_data.PWM_period_value[channel] * percentage) / 100);
    do_transaction(WRITE_REGISTER_CMD, register_address , duty_value, &data, &status);
    sys_data.PWM_duty_value[channel] = duty_value;
    global_FPGA_unit_error_flag = status;;
}

void FPGA_bus::PWM_enable(uint32_t channel)
{
    uint32_t register_address = ((PWM_BASE + (channel * NOS_PWM_REGISTERS)) + PWM_CONFIG);
    do_transaction(WRITE_REGISTER_CMD, register_address , 1, &data, &status);
 //   sys_data.PWM_duty_value[channel] = duty_value;
    global_FPGA_unit_error_flag = status;;
}

void FPGA_bus::PWM_config(uint32_t channel, uint32_t config_value)
{
    uint32_t register_address = ((PWM_BASE + (channel * NOS_PWM_REGISTERS)) + PWM_CONFIG);
    do_transaction(WRITE_REGISTER_CMD, register_address , config_value, &data, &status);
    sys_data.PWM_duty_value[channel] = config_value;
    global_FPGA_unit_error_flag = status;;
}

void FPGA_bus::set_RC_period(void)
{
    do_transaction(WRITE_REGISTER_CMD, (RC_BASE + RC_SERVO_PERIOD), 1000000, &data, &status);
    global_FPGA_unit_error_flag = status;
}

void FPGA_bus::set_RC_period(uint32_t duty_uS)
{
    uint32_t nos_20nS_ticks = ((duty_uS * nS_IN_uS)/FPGA_CLOCK_PERIOD_nS);
    do_transaction(WRITE_REGISTER_CMD, (RC_BASE + RC_SERVO_PERIOD), nos_20nS_ticks, &data, &status);
    global_FPGA_unit_error_flag = status;
}

void FPGA_bus :: set_RC_pulse(uint32_t channel, uint32_t pulse_uS)
{
    uint32_t nos_20nS_ticks = ((pulse_uS * nS_IN_uS)/FPGA_CLOCK_PERIOD_nS);
    do_transaction(WRITE_REGISTER_CMD, (RC_BASE + RC_SERVO_ON_TIME + channel), nos_20nS_ticks, &data, &status);
    global_FPGA_unit_error_flag = status;;    
}

void FPGA_bus::enable_RC_channel(uint32_t channel)
{
    do_transaction(READ_REGISTER_CMD, (RC_BASE + RC_SERVO_CONFIG), NULL, &data, &status);
    int32_t config = (data || (0x01 << channel)) + GLOBAL_RC_ENABLE;
    do_transaction(WRITE_REGISTER_CMD, (RC_BASE + RC_SERVO_CONFIG), config, &data, &status);
    global_FPGA_unit_error_flag = status;
}

void FPGA_bus::disable_RC_channel(uint32_t channel)
{
    do_transaction(READ_REGISTER_CMD, (RC_BASE + RC_SERVO_CONFIG), NULL, &data, &status);
    uint32_t config = data && ~(0x01 << channel);
    do_transaction(WRITE_REGISTER_CMD, (RC_BASE + RC_SERVO_CONFIG), config, &data, &status);
    global_FPGA_unit_error_flag = status;
}

void FPGA_bus::QE_config(uint32_t channel, uint32_t config_value)
{
    uint32_t register_address = ((PWM_BASE + (channel * NOS_PWM_REGISTERS)) + PWM_CONFIG);
    do_transaction(WRITE_REGISTER_CMD, register_address , config_value, &data, &status);
    global_FPGA_unit_error_flag = status;;
}

void FPGA_bus::enable_speed_measure(uint32_t channel, uint32_t config_value, uint32_t phase_time)
{
    uint32_t register_base = (QE_BASE + (channel * NOS_QE_REGISTERS));
    do_transaction(WRITE_REGISTER_CMD, (register_base + QE_SIM_PHASE_TIME), phase_time, &data, &status);
    global_FPGA_unit_error_flag = status;
    if (status != NO_ERROR) {
        return;
    }
    do_transaction(WRITE_REGISTER_CMD, (register_base + QE_CONFIG), config_value, &data, &status);
    global_FPGA_unit_error_flag = status;
}

uint32_t FPGA_bus::read_speed_measure(uint32_t channel)
{
    uint32_t register_address = ((QE_BASE + (channel * NOS_QE_REGISTERS)) + QE_SPEED_BUFFER);
    do_transaction(READ_REGISTER_CMD, register_address, NULL, &data, &status);
    global_FPGA_unit_error_flag = status;
    return data;
}

uint32_t FPGA_bus::read_count_measure(uint32_t channel)
{
    uint32_t register_address = ((QE_BASE + (channel * NOS_QE_REGISTERS)) + QE_COUNT_BUFFER);
    do_transaction(READ_REGISTER_CMD, register_address, NULL, &data, &status);
    global_FPGA_unit_error_flag = status;
    return data;
}

uint32_t FPGA_bus::get_SYS_data(void)
{
    do_transaction(READ_REGISTER_CMD, SYS_DATA_REG_ADDR, NULL, &data, &status);
    sys_data.major_version          = (data & 0x0000000F);
    sys_data.minor_version          = (data >> 4) & 0x0000000F;
    sys_data.number_of_PWM_channels = (data >> 8) & 0x0000000F;
    sys_data.number_of_QE_channels  = (data >> 8) & 0x0000000F;
    sys_data.number_of_RC_channels  = (data >> 8) & 0x0000000F;
    
    global_FPGA_unit_error_flag = status;
    return data;
}