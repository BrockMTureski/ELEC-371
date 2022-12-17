#include "nios2_control.h"

#define INPORT (unsigned volatile int*) 0x00004A00
#define OUTPORT (unsigned volatile int*) 0x00004A80
#define TIMER_STATUS (unsigned volatile int*) 0x00005000
#define TIMER_CONTROL (unsigned volatile int*) 0x00005004
#define TIMER_START_LO (unsigned volatile int*) 0x00005008
#define TIMER_START_HI (unsigned volatile int*) 0x0000500C
#define JTAG_UART_STATUS (unsigned volatile int*) 0x00006804
#define JTAG_UART_DATA (unsigned volatile int*) 0x00006800

#define ASCII_NUM 0x30
#define ASCII_LETTER 0x41
#define DATA_MASK 0xFFFF0000
#define NEWLINE 0x0A

unsigned volatile int count;
unsigned volatile int second;

void init(){
    *TIMER_START_HI = 0x004C;
    *TIMER_START_LO = 0x4B40;
    *TIMER_STATUS = 0;
    *TIMER_CONTROL = 7;

    count=0;
    second=0;

    *OUTPORT=0xF0;

    NIOS2_WRITE_IENABLE(0x1);
    NIOS2_WRITE_STATUS(0x1);

}

void PrintChar(unsigned int ch){
    while((*JTAG_UART_STATUS & DATA_MASK)==0){
        *(JTAG_UART_DATA)=ch;
    }
}

void UpdateLEDs(){
    unsigned int lightconfig = *OUTPORT;
    *OUTPORT = ~(lightconfig);
}

void interrupt_handler(void){
    unsigned int ipending_status = NIOS2_READ_IPENDING();

    if(ipending_status & 0x1){
        count+=1;
        *TIMER_STATUS=0;
        if(count%5==0 && count!=0) UpdateLEDs();
        if(count>=10){
            second=1;
            count=0;
        }
    }
}

int main(void){
    unsigned int totalSeconds=0;
    unsigned int data_char_hi;
    unsigned int data_char_lo;
    unsigned int data_char;
    unsigned int zero_count=0;

    init();

    while(1){
        if(second){
            second=0;
            ++totalSeconds;
        }
        if(totalSeconds%4==0 && totalSeconds!=0){
            data_char=*INPORT;
            if(data_char=0x00) zero_count++;
            data_char_hi = (data_char & 0xF0)>>4;
            data_char_lo = (data_char &0x0F);


            if(data_char_hi<=9) data_char_hi+=ASCII_NUM;
            else data_char_hi+=ASCII_LETTER;
            if(data_char_lo<=9) data_char_lo+=ASCII_NUM;
            else data_char_lo+=ASCII_LETTER;

            PrintChar(data_char_hi);
            PrintChar(data_char_lo);
            PrintChar(NEWLINE);
        }
    }
}