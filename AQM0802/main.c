#include "debug.h"

#define AQM0802_ADDR 0x7C  

#define LCD_CMD  0x00
#define LCD_DATA 0x40

void I2C_WriteByte(uint8_t control, uint8_t data) {
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, AQM0802_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, control);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, data);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTOP(I2C1, ENABLE);
    Delay_Ms(1); // slave wait
}

void AQM0802_Init(void) {
    Delay_Ms(40);  //power on
    I2C_WriteByte(LCD_CMD, 0x38); 
    I2C_WriteByte(LCD_CMD, 0x39); 
    I2C_WriteByte(LCD_CMD, 0x14); 
    I2C_WriteByte(LCD_CMD, 0x70); 
    I2C_WriteByte(LCD_CMD, 0x56);
    I2C_WriteByte(LCD_CMD, 0x6C); 
    Delay_Ms(200); //p33              
    I2C_WriteByte(LCD_CMD, 0x38); 
    I2C_WriteByte(LCD_CMD, 0x0C); 
    I2C_WriteByte(LCD_CMD, 0x01); 
}

void AQM0802_SetCursor(uint8_t pos) {
    I2C_WriteByte(LCD_CMD, 0x80 | pos);
}

void AQM0802_WriteStr(const char* str) {
    while (*str) {
        I2C_WriteByte(LCD_DATA, *str++);
    }
}

void I2C1_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
}

int main(void) {
    SystemCoreClockUpdate();
    Delay_Init();
    I2C1_Init();

    AQM0802_Init();

    AQM0802_SetCursor(0x00);     
    AQM0802_WriteStr("Hello");

    AQM0802_SetCursor(0x40);     
    AQM0802_WriteStr("CH32V203");

    while (1);
}
