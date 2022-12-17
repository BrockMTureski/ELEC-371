#include "nios2_control.h"

#define INPORT (unsigned volatile int*) 0x00004000
#define OUTPORT (unsigned volatile int*) 0x00004200
#define TIMER_STATUS (unsigned volatile int*) 0x00005000
#define TIMER_CONTROL (unsigned volatile int*) 0x00005004
#define TIMER_START_LO (unsigned volatile int*) 0x00005008
#define TIMER_START_HI (unsigned volatile int*) 0x0000500C
#define JTAG_UART_DATA (unsigned volatile int*) 0x0000B600
#define JTAG_UART_STATUS (unsigned volatile int*) 0x0000B604

#define DATA_MASK 0xFFFF0000
#define ASCII_NUM 0x30
#define ASCII_LETTER 0x41
#define NEWLINE 0x0D

unsigned volatile int count;
unsigned volatile int second;

void init(){
    *TIMER_START_HI=0x00BE;
    *TIMER_START_LO=0xBC20;
    *TIMER_STATUS=0;
    *TIMER_CONTROL=7;

    count=0;
    second=0;

    *OUTPORT=0x80;

    NIOS2_WRITE_STATUS(0x1);
    NIOS2_WRITE_IENABLE(0x1);
}

void PrintChar(unsigned int ch){
    while((*JTAG_UART_STATUS & DATA_MASK)==0){
        *(JTAG_UART_DATA)=ch;
    }
}

void PrintHexDigit(unsigned int n){
    if(n<=9){
        PrintChar(n+ASCII_NUM);
    }else PrintChar(n+ASCII_LETTER-10);
}

void UpdateLEDs(void){
    unsigned int current_LED=*OUTPORT;
    if((current_LED & 0xF0)==0xF0) *OUTPORT=0x80;
    else *OUTPORT= ((current_LED>>1)+0x80);
}

void interrupt_handler(void){
    unsigned int ipending = NIOS2_READ_IPENDING();

    if (ipending & 0x1){
        ++count;
        UpdateLEDs();
        *TIMER_STATUS=0;

        if(count>=4){
            count=0;
            second=1;
        }
    }
}

int main(){
    unsigned int total_seconds=0;
    unsigned int data_char;
    init();

    while(1){
        if(second){
            second=0;
            total_seconds+=1;
        if(total_seconds%60==0 && total_seconds!=0){
            unsigned int outputport=*OUTPORT;
            unsigned int inputport=*INPORT;
            *OUTPORT=(outputport | 0x01); //set
            *OUTPORT=(*OUTPORT & 0xFE); //reset 
            while((inputport & 0x80)==0){ //wait
                inputport=*INPORT;
            }
            inputport=*INPORT;
            inputport=(inputport&0x7F)>>3; //reset done bit and shift out low 3 bits

            PrintHexDigit(inputport);
            PrintChar(NEWLINE);

        }
        }
    }
}