#include "led_screen.h"
#include <string.h>

const uint8_t font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 32: Spacja
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // 33: !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 34: "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // 35: #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // 36: $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 37: %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 38: &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // 39: '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // 40: (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // 41: )
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // 42: *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // 43: +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 44: ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 45: -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 46: .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 47: /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 48: 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 49: 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 50: 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 51: 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 52: 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 53: 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 54: 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 55: 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 56: 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 57: 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 58: :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 59: ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // 60: <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 61: =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 62: >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 63: ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // 64: @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 65: A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 66: B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 67: C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 68: D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 69: E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 70: F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 71: G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 72: H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 73: I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 74: J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 75: K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 76: L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 77: M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 78: N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 79: O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 80: P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 81: Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 82: R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 83: S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 84: T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 85: U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 86: V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // 87: W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 88: X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 89: Y
    {0x61, 0x51, 0x49, 0x45, 0x43}  // 90: Z
};

static void write_command(ssd1306_t *display, uint8_t cmd)
{
    uint8_t data_pack[2] = {0x00, cmd};
    i2c_write_dt(&display->dev_i2c, data_pack, 2);
}

static void write_data(ssd1306_t *display, uint8_t* data, size_t len)
{
    i2c_burst_write_dt(&display->dev_i2c, 0x40, data, len);
}

void ssd1306_clear(ssd1306_t *display)
{
    memset(display->buffer, 0, sizeof(display->buffer));
}

void ssd1306_update(ssd1306_t *display)
{   
    write_command(display, 0x21);
    write_command(display, 0x00);
    write_command(display, 0x7F);


    write_command(display, 0x22);
    write_command(display, 0x00);
    write_command(display, 0x07);

    write_data(display, display->buffer, sizeof(display->buffer));
}

bool ssd1306_init(ssd1306_t *display)
{
    if (!i2c_is_ready_dt(&display->dev_i2c)) {
        return false;
    }

    write_command(display, 0xAE); //Display OFF

    write_command(display, 0xD5); // Ustawienie zegara
    write_command(display, 0x80); // Domyślna wartość

    write_command(display, 0xA8); // rozdzielczość
    write_command(display, 0x3F); //

    write_command(display, 0xD3); // Przesunięcie ekranu
    write_command(display, 0x00); // Brak przesunięcia

    write_command(display, 0x40); // Ustawienie linii startowej na 0

    write_command(display, 0x8D); // Charge Pump
    write_command(display, 0x14); //

    write_command(display, 0x20); // Tryb adresowania pamięci
    write_command(display, 0x00); // 0x00 = Horizontal Addressing Mode
    write_command(display, 0xA1); // Odwrócenie osi X
    write_command(display, 0xC8); // Odwrócenie osi Y
    write_command(display, 0xDA); // Konfiguracja pinów
    write_command(display, 0x12); // 128x64

    write_command(display, 0x81); // Ustawienie kontrastu
    write_command(display, 0xCF);

    write_command(display, 0xD9); // Okres ładowania
    write_command(display, 0xF1);

    write_command(display, 0xDB); //  VCOMH
    write_command(display, 0x40);

    write_command(display, 0xA4); // Używaj zawartości RAMdwyświetlania obrazu
    write_command(display, 0xA6); // Normalny tryb wyświetlania

    // 3. Wyczyszczenie bufora i wysłanie go do ekranu,
    ssd1306_clear(display);
    ssd1306_update(display);

    // 4. włączenie matrycy
    write_command(display, 0xAF); // Włącz ekran (Display ON)
    
    return true;
}

void ssd1306_draw_pixel(ssd1306_t *display, int x, int y, bool color)
{
    if (x < 0 || x >= 128 || y < 0 || y >= 64) {
        return;
    }
    int index = x + (y / 8) * 128;
        
    uint8_t bit = y % 8;

    if (color) {
        display->buffer[index] |= (1 << bit);  
    } else {
        display->buffer[index] &= ~(1 << bit);
    }
}

void ssd1306_draw_string(ssd1306_t *display, int x, int y, const char* str,uint8_t scale)
{
    int current_x = x;
    while(*str != '\0')
    {
        char c = *str;
        
        if(c >= 32 && c <= 90)
        {
            int font_ind = c - 32;

            for(int col = 0; col < 5; col++)
            {
                uint8_t col_data = font5x7[font_ind][col];
                
                for(int row = 0; row < 7; row++)
                {
                    if(col_data & (1<<row)) // kolor
                    {
                        for(uint8_t sx = 0;sx< scale; sx++)
                        {
                            for(uint8_t sy = 0;sy < scale; sy++)
                            {
                                uint8_t pixel_x = current_x + (col * scale) + sx;
                                uint8_t pixel_y = y + (row * scale) + sy;
                        
                            ssd1306_draw_pixel(display, pixel_x, pixel_y, true);
                            }
                        }
                    }
                
                }
            }
            current_x += 6*scale; // 
        }
        str++;
    }
    //iteruje sie bo string wyciagajac chara
 //dopoki w char nie dojde do konca 
 // biore chara i odejmuje od niego 32 zagladam do tablicy 
 // petla w petli x piec razy i wypisuje w kadym przebiegu y jak wypisze juz do konca znak to dodaje do x +1zeby zrobic odstep
}

void ssd1306_draw_bitmap(ssd1306_t *display, int x, int y, int w, int h, const uint8_t* bitmap) 
{
    int byte_width = (w + 7) / 8; 
    
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            
            uint8_t data = bitmap[row * byte_width + (col / 8)];
         
            if (data & (1 << (7 - (col % 8)))) {
                ssd1306_draw_pixel(display, x + col, y + row, true);
            } else {
                ssd1306_draw_pixel(display, x + col, y + row, false); 
            }
        }
    }
}

void ssd1306_clear_area(ssd1306_t *display, int x, int y, uint8_t length, uint8_t width)
{
    for(int l = 0;l<length;l++)
    {
        for(int w = 0;w<width;w++)
        {
            ssd1306_draw_pixel(display,x+l,w+y,false);
        }
    }

}

void ssd1306_fill_rect(ssd1306_t *display, int x, int y, int length, int width)
{
    for(int dx=0 ;dx<length; dx++ )
    {
        for(int dy = 0; dy < width; dy++)
        {
            ssd1306_draw_pixel(display, dx + x, y + dy, true);
        }
    }
}
