#include "nios2_control.h"

#define OUTPORT (unsigned volatile int*) 0x00004800
#define INPORT (unsigned volatile int*) 0x00004400
#define TIMER_STATUS (unsigned volatile int*) 0x00005000
#define TIMER_CONTROL (unsigned volatile int*) 0x00005004
#define TIMER_START_HI (unsigned volatile int*) 0x0000500C
#define TIMER_START_LO (unsigned volatile int*) 0x00005008
#define JTAG_UART_DATA (unsigned volatile int*) 0x0000A000
#define JTAG_UART_STATUS (unsigned volatile int*) 0x0000A004

#define DATA_MASK 0xFFFF0000
#define NEWLINE 0x0A
#define ASCII_NUM 0x30
#define ASCII_LETTER 0x41

unsigned volatile int count;
unsigned volatile int second;

void init(void){
    *TIMER_START_HI=0x0098;
    *TIMER_START_LO=0x9680;
    *TIMER_CONTROL=7;
    *TIMER_STATUS=0;

    count=0;
    second=0;

    *OUTPORT=0x1;

    NIOS2_WRITE_IENABLE(0x1);
    NIOS2_WRITE_STATUS(0x1);
}

void PrintChar(unsigned int ch){
    while((*JTAG_UART_STATUS & DATA_MASK)==0){
        *(JTAG_UART_DATA)=ch;
    }
}

void PrintHexDigit(unsigned int value){
    if(value<=9){
        PrintChar(value+ASCII_NUM);
    }else{
        PrintChar(value+ASCII_LETTER-10);
    }
}

void UpdateLEDs(void){
    unsigned int currentPos=*OUTPORT;
    if(currentPos>=0x8000) *OUTPORT=0x1;
    else *OUTPORT=currentPos<<1
}

void interrupt_handler(void){
    unsigned int ipending_status=NIOS2_READ_IPENDING();
    if(ipending_status & 0x1){
        *TIMER_STATUS=0;
        count+=1;
        UpdateLEDs();

        if(count>=5){
            second=1;
            count=0;
        }
    }
}

int main(void){
    unsigned int elapsed_seconds = 0;
    unsigned int hexdig1;
    unsigned int hexdig2;
    unsigned int hexdig3;
    unsigned int hexdig4;
    unsigned int input_chars;

    init();

    while(1){
    if(second){
        elapsed_seconds+=1;
        second=0;
    }
    if(elapsed_seconds%10==0){
        input_chars=*INPORT;
        hexdig1=(input_chars & 0xF000>>12);
        hexdig2=(input_chars & 0x0F00>>8);
        hexdig3=(input_chars & 0x00F0>>4);
        hexdig4=(input_chars & 0x000F);

        PrintHexDigit(hexdig1);
        PrintHexDigit(hexdig2);
        PrintHexDigit(hexdig3);
        PrintHexDigit(hexdig4);
        PrintChar(NEWLINE);

    }
    }
}
