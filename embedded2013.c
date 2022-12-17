//include statement
#include "nios2_control.h"
//def statements
#define INPORT (unsigned volatile int*) 0x00003000
#define OUTPORT (unsigned volatile int*) 0x00003400
#define TIMER_STATUS (unsigned volatile int*) 0x00005000
#define TIMER_CONTROL (unsigned volatile int*) 0x00005004
#define TIMER_START_LO (unsigned volatile int*) 0x00005008
#define TIMER_START_HI (unsigned volatile int*) 0x0000500C
#define JTAG_UART_DATA (unsigned volatile int*) 0x0000AE00
#define JTAG_UART_STATUS (unsigned volatile int*) 0x0000AE00


#define ASCII_NUM_START 0x30
#define ASCII_LETTER_START 0x41
#define DATA_MASK 0xFFFF0000
#define NEWLINE 0x0D

unsigned volatile int count;
unsigned volatile int one_second;

void init(){
    *TIMER_START_LO=0x9680;
    *TIMER_START_HI=0x0098;
    *TIMER_STATUS=0;
    *TIMER_CONTROL=7;

    *OUTPORT=0x00;

    count=0;
    one_second=0;

    NIOS2_WRITE_IENABLE(0x1);
    NIOS2_WRITE_STATUS(0x1);

}

void interrupt_handler(){
    unsigned int ipending_status = NIOS2_READ_IPENDING();

    if(ipending_status & 0x1){
        *TIMER_STATUS=0;
        count+=1;
        if(count>=5){
            count=0;
            one_second=1;
        }


    }
}

void print_char(char ch){
    while((*JTAG_UART_STATUS & DATA_MASK)==0){
        *(JTAG_UART_DATA)=ch;
    }
}

void main(){
    unsigned int input_char = *INPORT;
    unsigned int second_count=0;
    unsigned int display_char_lo;
    unsigned int display_char_hi;

    init();

    while(1){
        if(one_second){
            second_count+=1;
            one_second=0;
            if(second_count>=256) second_count=0;
            *OUTPORT=second_count;
        }
        if(input_char!=*INPORT){
            input_char=*INPORT;
            display_char_lo=(input_char & 0x0F);
            display_char_hi=(input_char & 0xF0>>4);
            if(display_char_hi<=9) display_char_hi+=ASCII_NUM_START;
            else display_char_hi+=ASCII_LETTER_START;
            if(display_char_lo<=9) display_char_lo+=ASCII_NUM_START;
            else display_char_lo+=ASCII_LETTER_START;

            print_char(display_char_hi);
            print_char(display_char_lo);
            print_char(NEWLINE);
        }
        
    }

}