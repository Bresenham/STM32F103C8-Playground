#include "ILI9163C.h"

void write_command(uint8_t cmd) {
  cs_low();
  a0_low();
  spi_transmit(cmd);
  cs_high();
}

void write_data(uint8_t data) {
  cs_low();
  a0_high();
  spi_transmit(data);
  cs_high();
}

void write_data_16(uint16_t data) {
  cs_low();
  a0_high();
  spi_transmit(data >> 8);
  spi_transmit(data);
  cs_high();
}

void drawRectFilled(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour) {
    uint16_t pixels;
            
    // To speed up plotting we define a x window with the width of the 
    // rectangle and then just output the required number of bytes to
    // fill down to the end point
    
    write_command(SET_COLUMN_ADDRESS); // Horizontal Address Start Position
    write_data(0x00);
    write_data(x0);
    write_data(0x00);
    write_data(x1);
  
    write_command(SET_PAGE_ADDRESS); // Vertical Address end Position
    write_data(0x00);
    write_data(y0);
    write_data(0x00);
    write_data(0x7f);
        
    write_command(WRITE_MEMORY_START);
    
    for (pixels = 0; pixels < (((x1 - x0) + 1) * ((y1 - y0) + 1)); pixels++)
        write_data_16(colour);
}

void drawPixel(uint8_t x, uint8_t y, uint16_t colour) {
    // Horizontal Address Start Position
    write_command(SET_COLUMN_ADDRESS);
    write_data(0x00);
    write_data(x);
    write_data(0x00);
    write_data(0x7f);
  
    // Vertical Address end Position
    write_command(SET_PAGE_ADDRESS);
    write_data(0x00);
    write_data(y);
    write_data(0x00);
    write_data(0x7f);

    // Plot the point
    write_command(WRITE_MEMORY_START);
    write_data_16(colour);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t colour) {
    int16_t dy = y1 - y0;
    int16_t dx = x1 - x0;
    int16_t stepx, stepy;

    if (dy < 0) {
        dy = -dy; stepy = -1; 
    }
    else stepy = 1; 

    if (dx < 0) {
        dx = -dx; stepx = -1; 
    }
    else stepx = 1; 

    dy <<= 1;                           // dy is now 2*dy
    dx <<= 1;                           // dx is now 2*dx
 
    drawPixel(x0, y0, colour);

    if (dx > dy) {
        int fraction = dy - (dx >> 1);  // same as 2*dy - dx
        while (x0 != x1) {
            if (fraction >= 0) {
                y0 += stepy;
                fraction -= dx;         // same as fraction -= 2*dx
            }

            x0 += stepx;
            fraction += dy;                 // same as fraction -= 2*dy
            drawPixel(x0, y0, colour);
        }
    } else {
        int fraction = dx - (dy >> 1);
        while (y0 != y1) {
            if (fraction >= 0) {
                x0 += stepx;
                fraction -= dy;
            }

            y0 += stepy;
            fraction += dx;
            drawPixel(x0, y0, colour);
        }
    }
}

void clear(uint16_t color) {
    uint16_t pixel;
  
    // Set the column address to 0-127
    write_command(SET_COLUMN_ADDRESS);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0x7f);

    // Set the page address to 0-127
    write_command(SET_PAGE_ADDRESS);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0x7f);
  
    // Plot the pixels
    write_command(WRITE_MEMORY_START);
    for(pixel = 0; pixel <= 128*128; pixel++){
      write_data_16(color);
    }
}

void ILI9163C_init() {
    /*
    cs_high();
    delay(50);
    reset_high();
    delay(50);
    */
    reset();
    delay(50);
    write_command(EXIT_SLEEP_MODE);
    delay(50); // Wait for the screen to wake up
    
    write_command(SET_PIXEL_FORMAT);
    write_data(0x03); // 16 bits per pixel
    delay(50);
    
    write_command(SET_GAMMA_CURVE);
    write_data(0x04); // Select gamma curve 3
    delay(50);
    
    write_command(GAM_R_SEL);
    write_data(0x01); // Gamma adjustment enabled
    delay(50);
    
    write_command(POSITIVE_GAMMA_CORRECT);
    delay(50);
    write_data(0x3f); // 1st Parameter
    write_data(0x25); // 2nd Parameter
    write_data(0x1c); // 3rd Parameter
    write_data(0x1e); // 4th Parameter
    write_data(0x20); // 5th Parameter
    write_data(0x12); // 6th Parameter
    write_data(0x2a); // 7th Parameter
    write_data(0x90); // 8th Parameter
    write_data(0x24); // 9th Parameter
    write_data(0x11); // 10th Parameter
    write_data(0x00); // 11th Parameter
    write_data(0x00); // 12th Parameter
    write_data(0x00); // 13th Parameter
    write_data(0x00); // 14th Parameter
    write_data(0x00); // 15th Parameter
    delay(50);
    
    write_command(NEGATIVE_GAMMA_CORRECT);
    delay(50);
    write_data(0x20); // 1st Parameter
    write_data(0x20); // 2nd Parameter
    write_data(0x20); // 3rd Parameter
    write_data(0x20); // 4th Parameter
    write_data(0x05); // 5th Parameter
    write_data(0x00); // 6th Parameter
    write_data(0x15); // 7th Parameter
    write_data(0xa7); // 8th Parameter
    write_data(0x3d); // 9th Parameter
    write_data(0x18); // 10th Parameter
    write_data(0x25); // 11th Parameter
    write_data(0x2a); // 12th Parameter
    write_data(0x2b); // 13th Parameter
    write_data(0x2b); // 14th Parameter
    write_data(0x3a); // 15th Parameter
    delay(50);
    
    write_command(FRAME_RATE_CONTROL1);
    write_data(0x08); // DIVA = 8
    write_data(0x08); // VPA = 8
    delay(50);
    
    write_command(DISPLAY_INVERSION);
    write_data(0x07); // NLA = 1, NLB = 1, NLC = 1 (all on Frame Inversion)
    delay(50);
   
    write_command(POWER_CONTROL1);
    write_data(0x0a); // VRH = 10:  GVDD = 4.30
    write_data(0x02); // VC = 2: VCI1 = 2.65
    delay(50);
    
    write_command(POWER_CONTROL2);
    write_data(0x02); // BT = 2: AVDD = 2xVCI1, VCL = -1xVCI1, VGH = 5xVCI1, VGL = -2xVCI1
    delay(50);
    
    write_command(VCOM_CONTROL1);
    write_data(0x50); // VMH = 80: VCOMH voltage = 4.5
    write_data(0x5b); // VML = 91: VCOML voltage = -0.225
    delay(50);
    
    write_command(VCOM_OFFSET_CONTROL);
    write_data(0x40); // nVM = 0, VMF = 64: VCOMH output = VMH, VCOML output = VML
    delay(50);
    
    write_command(SET_COLUMN_ADDRESS);
    delay(50);
    write_data(0x00); // XSH
    write_data(0x00); // XSL
    write_data(0x00); // XEH
    write_data(0x7f); // XEL (128 pixels x)
    delay(50);
   
    write_command(SET_PAGE_ADDRESS);
    delay(50);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0x7f); // 128 pixels y
    delay(50);
    
    // Select display orientation
    write_command(SET_ADDRESS_MODE);
    write_data(192); //can also be 0, 96, 160, 192
    delay(50);
    
    // Set the display to on
    write_command(SET_DISPLAY_ON);
    write_command(WRITE_MEMORY_START);
}