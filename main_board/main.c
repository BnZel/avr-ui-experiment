#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <u8g2.h>
#include <u8x8_avr.h>
#include <uart.h>

// uart
#define DEBUG                                       1
               
// oled i2c 
#define SH1106_ADDR                                 0x78

// menu
#define NUM_ITEMS                                   3

// pin definitions
#define BUTTON_UP                                   PIND6
#define BUTTON_DOWN                                 PIND7   
#define BUTTON_SELECT                               PINB0
#define PRESSED_UP                                  bit_is_clear(PIND,6)
#define PRESSED_DOWN                                bit_is_clear(PIND,7)
#define PRESSED_SELECT                              bit_is_clear(PINB,0)


u8g2_t u8g2;


//=====================START OLED=====================
void init_display(void)
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_avr_hw_i2c, u8x8_avr_delay);
    u8g2_InitInterface(&u8g2);

    u8g2_SetI2CAddress(&u8g2, SH1106_ADDR);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);

    u8g2_SetFont(&u8g2, u8g2_font_7x14_tr);
    u8g2_SetFontRefHeightText(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
}

void display_text(int x, int y, char *text)
{
    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawStr(&u8g2, x, y, text);
    u8g2_SendBuffer(&u8g2);
}
//=====================END OLED=====================

//=====================START UART=====================
void uart_check_transmit(uint8_t check)
{
    // UCSR1A is the control register A of the USART
    // UDRE1 means if it is ready to transmit data
    // while UDRE1 is zero, transmitter is busy so don't pass
    // this is a method called pulling
    while (!(UCSR0A & (1 << UDRE0)));

    // sample binary number check
    // recieving end must recieve this number
    // otherwise it won't do task
    UDR0 = check;
}
//=====================END UART=====================

//=====================START SD=====================
void sd_init(void)
{
    // display_text(0,25,"Testing SD...");

    // if(!disk_initialize(CT_MMC)){
    //     uart_puts_p("MMC/SD initialization failed\n");
    //     display_text(0,0,"Failed.");
    //     u8g2_DrawStr(&u8g2,0,25,"1. Inserted?");
    //     u8g2_DrawStr(&u8g2,0,50,"2. FAT32/16?");
    //     u8g2_SendBuffer(&u8g2);

    //     _delay_ms(1000);
    // }
    // else{
    //     uart_puts("SD initialization success\n");
    //     display_text(0,0,"Success.");

    //     uart_puts("Device status is: ");
    //     uart_putw_dec(disk_status(CT_MMC));

    //     _delay_ms(1000);
    // }
}
//=====================END SD=====================

//=====================MENU START=====================
// ' uart_icon', 16x16px
const unsigned char epd_bitmap__uart_icon [] PROGMEM = {
	0x00, 0x00, 0xfe, 0x42, 0x10, 0x42, 0x10, 0x24, 0x10, 0x24, 0x10, 0x24, 0x10, 0x18, 0x00, 0x18, 
	0x3c, 0x18, 0x44, 0x18, 0x44, 0x24, 0x3c, 0x24, 0x44, 0x42, 0x44, 0x42, 0x44, 0x81, 0x00, 0x00
};

// ' ble_icon', 16x16px
const unsigned char epd_bitmap__ble_icon [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x80, 0x02, 0x88, 0x04, 0x90, 0x48, 0xa0, 0x24, 0xc0, 0x12, 
	0x80, 0x41, 0xc0, 0x12, 0xa0, 0x24, 0x90, 0x48, 0x88, 0x04, 0x80, 0x02, 0x80, 0x01, 0x00, 0x00
};

// ' files_icon', 16x16px
const unsigned char epd_bitmap__files_icon [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0xfc, 0x0f, 0x04, 0x18, 0x04, 0x38, 0xe4, 0x23, 0x04, 0x20, 0x04, 0x20, 
	0xf4, 0x27, 0x04, 0x20, 0x04, 0x20, 0xe4, 0x23, 0x04, 0x20, 0xfc, 0x3f, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. 
// (Total bytes used to store images in PROGMEM = 144)
const unsigned char* bitmap_icons[3] = {
    epd_bitmap__uart_icon,
    epd_bitmap__files_icon,
	epd_bitmap__ble_icon
};

// ' select_item', 128x20px
const unsigned char epd_bitmap__select_item [] PROGMEM = {
	0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 
	0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07
};

// to add more items if needed
const char menu_items[NUM_ITEMS][20] = {
    {"UART"},
    {"View Files"},
    {"BLE Buzz"}
};

void menu_uart(void)
{
    u8g2_ClearBuffer(&u8g2);  

    // testing the led 
    // on the reciever atmega328
    u8g2_DrawStr(&u8g2,0,0,"9600 Baud Rate"); 

    u8g2_DrawStr(&u8g2,0,25,"TESTING...");
    u8g2_DrawStr(&u8g2,0,50,"LED IS ON");

    uart_puts("Sending binary LED test\n");
    uart_check_transmit(0b11110000);    
    u8g2_SendBuffer(&u8g2);

    _delay_ms(1000);

    u8g2_DrawStr(&u8g2,0,50,"LED IS OFF");
    uart_check_transmit(0b00000000); 
    u8g2_SendBuffer(&u8g2);
    
    _delay_ms(1000);
}

void menu_ble(void)
{
    uart_puts("lost+found...\n");

    while(uart_getc())
    {
        display_text(0,25,"BUZZING...");

        if(uart_getc() == 'l'){
            uart_puts("device found\n"); 
            display_text(0,25,"FOUND!");
            PORTB ^= (1 << PINB1); 
            break;   
        }
    }    
}
//=====================MENU END=====================


int main(void)
{
    // buzzer
    DDRB |= 1 << PINB1;                     // output PINB1
    PORTB ^= PINB1;                         // toggle PIN 1 on PORTB

    // inputs
    // buttons
    DDRD &= ~(1 << BUTTON_UP);              // input PIND7
    DDRD &= ~(1 << BUTTON_DOWN);            // input PIND6 
    DDRB &= ~(1 << BUTTON_SELECT);          // input PINB0

    // button status
    PORTD |= 1 << BUTTON_UP;                // set PIND7 to high reading
    PORTD |= 1 << BUTTON_DOWN;              // set PIND6 to high reading 
    PORTD |= 1 << BUTTON_SELECT;            // set PINB0 to high reading

    // variables
    int pressed = 0;                        // for button confirm
    char ble_status;                        // to store status of bluetooth module

    
    // this is for when there's  
    // more selections to add
    int select = 1;
    int prev = 0;
    int next = 2;

    int select_outline_x = 4; 
    int select_outline_y = 22;

    // setup functions
    init_display();                         // oled sh1106 i2c setup
    uart_init();                            // set to 9600 by default


    while(1)
    {
        // display menu selection
        u8g2_FirstPage(&u8g2);
        do{
            // select outline
            u8g2_DrawXBMP(
                            &u8g2,
                            select_outline_x, select_outline_y,
                            128,20,
                            epd_bitmap__select_item
                        );

            // uart
            u8g2_DrawXBMP(&u8g2,6,2,16,16,bitmap_icons[0]);
            u8g2_SetFont(&u8g2,u8g2_font_7x14_tr);
            u8g2_DrawStr(&u8g2,30,5,menu_items[prev]);

            // files
            u8g2_DrawXBMP(&u8g2,6,24,16,16,bitmap_icons[1]);
            u8g2_SetFont(&u8g2,u8g2_font_7x14_tr);
            u8g2_DrawStr(&u8g2,30,27,menu_items[select]);

            // ble
            u8g2_DrawXBMP(&u8g2,6,46,16,16,bitmap_icons[2]); 
            u8g2_SetFont(&u8g2,u8g2_font_7x14_tr);
            u8g2_DrawStr(&u8g2,30,49,menu_items[next]);

            // move selection outline
            // up and down 
            // on the y position
            // minimum value is 0
            // maximum value is 44
            // increments/decrements by 22
            if(PRESSED_UP){
                if(select_outline_y <= 0){
                    select_outline_y = 44;
                }
                else{
                    select_outline_y -= 22;
                }
            }

            if(PRESSED_DOWN){
                if(select_outline_y >= 44){
                    select_outline_y = 0;
                }
                else{
                    select_outline_y += 22;
                }
            }


            // based on the y position
            // switch over to that menu
            if(PRESSED_SELECT){
                switch (select_outline_y)
                {
                    case 0:
                        uart_puts("=====UART selected=====\n\n");
                        menu_uart();
                        break;
                    case 22:
                        uart_puts("=====View Files selected=====\n\n");
                        sd_init();
                        break;
                    case 44:
                        uart_puts("=====BLE Buzz selected=====\n\n");
                        PORTB ^= 1 << PINB1;
                        menu_ble();
                        break;
                }
            }
        } 
        while (u8g2_NextPage(&u8g2));
    }
}