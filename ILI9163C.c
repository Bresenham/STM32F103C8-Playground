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
  a0_high();
  cs_low();
  spi_transmit(data >> 8);
  spi_transmit(data);
  cs_high();
}

void ILI9163C_init() {
    cs_high();
    reset_high();
    
    reset();
    delay(50);
    write_command(EXIT_SLEEP_MODE);
    delay(50); // Wait for the screen to wake up
    
    write_command(SET_PIXEL_FORMAT);
    write_data(0x05); // 16 bits per pixel
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
    write_data(0x00); // XSH
    write_data(0x00); // XSL
    write_data(0x00); // XEH
    write_data(0x7f); // XEL (128 pixels x)
    delay(50);
   
    write_command(SET_PAGE_ADDRESS);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0x7f); // 128 pixels y
    delay(50);
    
    // Select display orientation
    write_command(SET_ADDRESS_MODE);
    write_data(192); //can also be 0, 96, 160
    delay(50);
    
    // Set the display to on
    write_command(SET_DISPLAY_ON);
    write_command(WRITE_MEMORY_START);
}