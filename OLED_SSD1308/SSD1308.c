/**  @file SSD1308 I2C device class file
 *   Based on Solomon Systech SSD1308 datasheet, rev. 1, 10/2008
 *   The SSD1308 is used for example in the Seeed 128x64 OLED Display
 *   http://www.seeedstudio.com/depot/grove-oled-display-12864-p-781.html?cPath=163_167
 */
// The original code by Andrew Schamp is using (and has been submitted as a part of) Jeff Rowberg's I2Cdevlib library,
// which should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// Some parts also mashed up from Graphic Library for driving monochrome displays based on the PCD8544,
// Copyright (c) 2011, Wim De Roeve, who in turn did partial port of code found on
// http://serdisplib.sourceforge.net/ser/pcd8544.html#links and by Petras Saduikis <petras@petras.co.uk>
//
// Changelog:
//   2011-08-25 - Initial release by Andrew Schamp <schamp@gmail.com>
//   2012-06-19 - Ported to mbed and optimised (WH)
//       
/*
   ================================================================================
   I2Cdev device library code is placed under the MIT license
   Copyright (c) 2011 Andrew Schamp
   Copyright (c) 2012 WH (mbed port)

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
   ================================================================================
   */

#include "SSD1308.h"
//#include "font_3x5.h"
//#include "font_5x7.h"
//#include "font_6x8.h"
#include "font_8x8.h"
//#include "font_8x12.h"
//#include "font_16x20.h"
#include "font_16x24.h"

#include "stm32f10x.h"
#include "I2Cs.h"


#define xtrue 1
#define xfalse 0

static uint8_t _readOpcode  = 0;  // contains the I2C address of the device
static uint8_t _writeOpcode = 0;  // contains the I2C address of the device
static bool    _inverted    = 0;  // inverted or normal text

bool is_SSD1308_DisplayOn = 0;

static void _i2c_start(void)
{
	int n = 5;
	while((!I2C1_Start(0)) && (n>0)) n--;
}

static void _i2c_write_addr(uint8_t data)
{
	int n = 5;
	while((!I2C1_SendByte(data)) && (n>0)) n--;
}

static void _i2c_write(uint8_t data)
{
	int n = 5;
	while((!I2C1_SendByte(data)) && (n>0)) n--;
}

static void _i2c_stop(void)
{
	int n = 5;
		while((!I2C1_Stop()) && (n>0)) n--;
}

// Internal use functions
static void _sendCommand(uint8_t command, uint8_t len, uint8_t* pParam);
static void _sendCommandByte(uint8_t command, uint8_t data);
static void _sendDataByte(uint8_t data);
static void _sendData(uint8_t len, uint8_t* pData);
static void _LowLayer_init(void);


/** @brief High level Init, most settings remain at Power-On reset value
*/
void SSD1308_Init()
{

 //   _writeOpcode = DEVICEADDRESS;// & 0xFE; // low order bit = 0 for write
  //  _readOpcode  = DEVICEADDRESS;// | 0x01; // low order bit = 1 for read
  //  _inverted    = false;

    _LowLayer_init();
}

/** @brief clear the display
*/
#if(0)
// Standard version
void SSD1308_clearDisplay(void)
{
    setPageAddress(0, MAX_PAGE);  // all pages
    setColumnAddress(0, MAX_COL); // all columns

    for (uint8_t page = 0; page < PAGES; page++)
    {
        for (uint8_t col = 0; col < COLUMNS; col++)
        {
            _sendByte(0x00);
        }
    }
}
#else
// Optimised version
// Save lots of I2C S,P, address and datacommands:
// Send S, address, DATA_MODE, data, data, data,...., P
void SSD1308_clearDisplay(void)
{

    int i = 0;
    //setDisplayOff();

    SSD1308_setPageAddress(0, MAX_PAGE);  // all pages
    SSD1308_setColumnAddress(0, MAX_COL); // all columns

    _i2c_start();
    _i2c_write_addr(_writeOpcode);
    _i2c_write(DATA_MODE);
    for (i=0; i<(PAGES * COLUMNS); i++)
    {
        _i2c_write(0x00);  // Write Data
    }
    _i2c_stop();

    //setDisplayOn();
}
#endif


/** @brief fill the display
 *  @param uint8_t pattern fillpattern vertical patch or 8 bits 
 */
#if(0)
//Standard version
void SSD1308_fillDisplay(uint8_t pattern) {

    //setDisplayOff();  

    SSD1308_setPageAddress(0, MAX_PAGE);  // all pages
    SSD1308_setColumnAddress(0, MAX_COL); // all columns

    for (uint8_t page = 0; page < PAGES; page++) {
        for (uint8_t col = 0; col < COLUMNS; col++) {
            _sendByte(pattern); 
        }
    }

    //setDisplayOn();  
}
#else
//Optimised version
void SSD1308_fillDisplay(uint8_t pattern, uint8_t start_page, uint8_t end_page,
        uint8_t start_col, uint8_t end_col)
{

    int count = 0;
    int i     = 0;

    count = (end_page - start_page + 1) * (end_col - start_col + 1);

    //setDisplayOff();
    SSD1308_setPageAddress(start_page, end_page);  // set page window
    SSD1308_setColumnAddress(start_col, end_col);  // set column window

    _i2c_start();
    _i2c_write_addr(_writeOpcode);
    _i2c_write(DATA_MODE);  
    for (i=0; i<count; i++)
    {
        _i2c_write(pattern);  // Write Data   
    }
    _i2c_stop();

    //setDisplayOn();
}
#endif

/** @brief write a bitmap to the display
 *  @param uint8_t* data pointer to bitmap
 *  @param uint8_t start_page begin page   (0..MAX_PAGE)
 *  @param uint8_t end_page   end page     (start_page..MAX_PAGE)                     
 *  @param uint8_t start_col  begin column (0..MAX_COL)
 *  @param uint8_t end_col    end column   (start_col..MAX_COL)
 */
#if(0)
//Standard version
void SSD1308_writeBitmap(int len, uint8_t* data) {

    //setDisplayOff();
    setPageAddress(0, MAX_PAGE);  // all pages
    setColumnAddress(0, MAX_COL); // all columns

    _i2c_start();
    _i2c_write_addr(_writeOpcode);
    _i2c_write(DATA_MODE);  
    for (int i=0; i<len; i++) {
        _i2c_write(data[i]);  // Write Data   
    }
    _i2c_stop();

    //setDisplayOn();
}
#else
//Optimised version
void SSD1308_writeBitmap(uint8_t* data, uint8_t start_page, uint8_t end_page,
        uint8_t start_col, uint8_t end_col)
{

    int i     = 0;
    int count = 0;

    count = (end_page - start_page + 1) * (end_col - start_col + 1);

    //setDisplayOff();
    SSD1308_setPageAddress(start_page, end_page);  // set page window
    SSD1308_setColumnAddress(start_col, end_col);  // set column window

    _i2c_start();
    _i2c_write_addr(_writeOpcode);
    _i2c_write(DATA_MODE);  
    for (i=0; i<count; i++) {
        _i2c_write(data[i]);  // Write Data       
    }
    _i2c_stop();

    //setDisplayOn();
}
#endif


/** @brief write a progressbar to the display, Width is (PRG_MAX_SCALE + 2) pixels
 *  @param uint8_t page begin page   (0..MAX_PAGE)
 *  @param uint8_t col  begin column (0..MAX_COL)
 *  @param int percentage value      (0..100)
 */
#define PRG_MAX_SCALE     50
#define PRG_LEFT_EDGE     0xFF
#define PRG_RIGHT_EDGE    0xFF
#define PRG_ACTIVE        0xFF
//#define PRG_ACTIVE        0xBD
#define PRG_NOT_ACTIVE    0x81

#if(0)
//Standard version
void SSD1308_writeProgressBar(uint8_t page, uint8_t col, int percentage) {
    uint8_t scale_value;

    if (percentage <= 0) {
        scale_value = 0;
    } else if (percentage >= 100) {
        scale_value = PRG_MAX_SCALE - 1;
    }
    else {
        scale_value = (percentage * PRG_MAX_SCALE) / 100; 
    }      

    //setDisplayOff();
    setPageAddress(page, page);  
    setColumnAddress(col, MAX_COL); 

    _sendByte(PRG_LEFT_EDGE);

    for (uint8_t col = 0; col < scale_value; col++) {
        _sendByte(PRG_ACTIVE);
    }

    _sendByte(PRG_ACTIVE);

    for (uint8_t col = (scale_value+1); col < PRG_MAX_SCALE; col++) {
        _sendByte(PRG_NOT_ACTIVE);
    }

    _sendByte(PRG_RIGHT_EDGE);    

    //setDisplayOn();
}
#else
//Optimised version
void SSD1308_writeProgressBar(uint8_t page, uint8_t col, int percentage)
{
    uint8_t scale_value = 0;
    uint8_t i           = 0;

    if (percentage <= 0)
    {
        scale_value = 0;
    } else if (percentage >= 100)
    {
        scale_value = PRG_MAX_SCALE - 1 ;
    }
    else
    {
        scale_value = (percentage * PRG_MAX_SCALE) / 100; 
    }      

    //setDisplayOff();
    SSD1308_setPageAddress(page, page);
    SSD1308_setColumnAddress(col, MAX_COL);

    _i2c_start();
    _i2c_write_addr(_writeOpcode);
    _i2c_write(DATA_MODE);  
    _i2c_write(PRG_LEFT_EDGE);  // Write Data         

    for (i = 0; i < scale_value; i++)
    {
        _i2c_write(PRG_ACTIVE);  // Write Data                       
    }

    _i2c_write(PRG_ACTIVE);  // Write Data                       

    for (i = (scale_value+1); i < PRG_MAX_SCALE; i++)
    {
        _i2c_write(PRG_NOT_ACTIVE);  // Write Data                 
    }

    _i2c_write(PRG_RIGHT_EDGE);  // Write Data           

    _i2c_stop();

    //setDisplayOn();
}
#endif

/** @brief write a level meter to the display, Width is (PRG_MAX_SCALE + 2) pixels
 *  @param uint8_t page begin page   (0..MAX_PAGE)
 *  @param uint8_t col  begin column (0..MAX_COL)
 *  @param int percentage value      (0..100)
 *  @note  Optimised version
 */
void SSD1308_writeLevelBar(uint8_t page, uint8_t col, int percentage)
{
    uint8_t i           = 0;
    uint8_t scale_value = 0;

    if (percentage <= 0)
    {
        scale_value = 0;
    } else if (percentage >= 100)
    {
        scale_value = PRG_MAX_SCALE - 1;
    }
    else
    {
        scale_value = (percentage * PRG_MAX_SCALE) / 100; 
    }      

    //setDisplayOff();
    SSD1308_setPageAddress(page, page);
    SSD1308_setColumnAddress(col, MAX_COL);

    _i2c_start();
    _i2c_write_addr(_writeOpcode);
    _i2c_write(DATA_MODE);  

    _i2c_write(PRG_LEFT_EDGE);  // Write Data         

    for (i = 0; i < scale_value; i++)
    {
        _i2c_write(PRG_NOT_ACTIVE);  // Write Data                       
    }

    _i2c_write(PRG_ACTIVE);  // Write Data at active meterlevel

    for (i = scale_value+1; i < PRG_MAX_SCALE; i++)
    {
        _i2c_write(PRG_NOT_ACTIVE);  // Write Data                 
    }

    _i2c_write(PRG_RIGHT_EDGE);  // Write Data           

    _i2c_stop();

    //setDisplayOn();
}

void SSD1308_setInverted(bool inverted)
{
    _inverted = inverted;
}

/** @brief Write single character to the display using the 8x8 fontable
 *  @brief Start at current cursor location
 *  @param char chr character to write
 */
void SSD1308_writeChar(char chr)
{
    const uint8_t char_index = chr - 0x20;
    uint8_t i = 0;

    for (i = 0; i < 8; i++)
    {
        if (_inverted)
        {
            _sendDataByte(~font_8x8[char_index][i]);
        }
        else
        {
            _sendDataByte(font_8x8[char_index][i]);
        }  
    }
}

/** @brief Write large character (16x24 font)
 *  @param uint8_t row  row number    (0...MAX_ROW)
 *  @param uint8_t col  column number (0...MAX_COL)
 *  @param char chr     Used for displaying numbers 0 - 9 and '+', '-', '.'
 */
void SSD1308_writeBigChar(uint8_t row, uint8_t col, char chr)
{
	SSD1308_writeBitmap((uint8_t*) font_16x24[((int)chr) - FONT16x24_START],
            row, (row + FONT16x24_BYTES - 1),
            col, (col + FONT16x24_WIDTH - 1));
}

/** @brief Write a string to the display using the 8x8 font
 *  @brief Start at selected cursor location, text will wrap around until it is done
 *  @param uint8_t row  row number    (0...ROWS/FONT_HEIGHT)
 *  @param uint8_t col  column number (0...COLUMNS/FONT_WIDTH)
 *  @param uint16_t len number of chars in text
 *  @param const char * text pointer to text
 */
void SSD1308_writeString(uint8_t row, uint8_t col, const char * text)
{
    uint16_t index         = 0;
    uint16_t len           = strlen(text);
    const uint8_t col_addr = FONT8x8_WIDTH*col;

    SSD1308_setPageAddress(row, MAX_PAGE);
    SSD1308_setColumnAddress(col_addr, MAX_COL);

    while ((col+index) < CHARS && (index < len))
    {
        // write first line, starting at given position
    	SSD1308_writeChar(text[index++]);
    }

    // write remaining lines
    // write until the end of memory
    // then wrap around again from the top.
    if (index + 1 < len)
    {
        bool wrapEntireScreen = false;
        SSD1308_setPageAddress(row + 1, MAX_PAGE);
        SSD1308_setColumnAddress(0, MAX_COL);
        while (index + 1 < len)
        {
        	SSD1308_writeChar(text[index++]);
            // if we've written the last character space on the screen, 
            // reset the page and column address so that it wraps around from the top again
            if (!wrapEntireScreen && (row*CHARS + col + index) > 127)
            {
            	SSD1308_setPageAddress(0, MAX_PAGE);
            	SSD1308_setColumnAddress(0, MAX_COL);
                wrapEntireScreen = true;
            }
        }
    }
}

/** @brief Set Horizontal Addressing Mode (cursor incr left-to-right, top-to-bottom)
 * 
 */
void SSD1308_setHorizontalAddressingMode(void)
{
    SSD1308_setMemoryAddressingMode(HORIZONTAL_ADDRESSING_MODE); 
}

/** @brief Set Vertical Addressing Mode  (cursor incr top-to-bottom, left-to-right)
 * 
 */
void SSD1308_setVerticalAddressingMode(void)
{
    SSD1308_setMemoryAddressingMode(VERTICAL_ADDRESSING_MODE); 
}

/** @brief Set Page Addressing Mode  (cursor incr left-to-right)
 * 
 */
void SSD1308_setPageAddressingMode(void)
{
    SSD1308_setMemoryAddressingMode(PAGE_ADDRESSING_MODE); 
}

/** @brief Set Addressing Mode
 *  @param uint8_t mode 
 */
void SSD1308_setMemoryAddressingMode(uint8_t mode)
{
    _sendCommandByte(SET_MEMORY_ADDRESSING_MODE, mode);   
}

/** @param uint8_t start startpage (valid range 0..MAX_PAGE)
 *  @param uint8_t end   endpage   (valid range start..MAX_PAGE)
 */
void SSD1308_setPageAddress(uint8_t start, uint8_t end)
{
    uint8_t buf[2] = {start, end};
    _sendCommand(SET_PAGE_ADDRESS, 2, &buf[0]);
}

/** @param uint8_t start startcolumn (valid range 0..MAX_COL)
 *  @param uint8_t end   endcolumn   (valid range start..MAX_COL)
 */
void SSD1308_setColumnAddress(uint8_t start, uint8_t end) {

    uint8_t buf[2] = {start, end};
    _sendCommand(SET_COLUMN_ADDRESS, 2, &buf[0]);
}

/** 
 *  @brief Set Display StartLine, takes one byte, 0x00-0x3F
 *  @param uint8_t line startline (valid range 0..MAX_ROWS)
 */  
void SSD1308_setDisplayStartLine(uint8_t line)
{
    line = line & MAX_ROW;

    _sendCommand(SET_DISPLAY_START_LINE | line, 0, (void*)0);
}

/** 
 *  @brief Set Column Start (for Page Addressing Mode only)
 *  @param uint8_t column column start (valid range 0..MAX_COL)
 */  
void SSD1308_setColumnStartForPageAddressingMode(uint8_t column)
{
    column = column & MAX_COL;

    _sendCommand(SET_LOWER_COLUMN  | ( column     & 0x0F), 0, (void*) 0);  // lower nibble
    _sendCommand(SET_HIGHER_COLUMN | ((column>>4) & 0x0F), 0, (void*) 0);  // higher nibble
}

/** 
 *  @brief Set Page Start (for Page Addressing Mode only)
 *  @param uint8_t page page start (valid range PAGE0 - PAGE7)
 */    
void SSD1308_setPageStartForPageAddressingMode(uint8_t page)
{
    page = page & MAX_PAGE;

    _sendCommand(SET_PAGE_START_ADDRESS | page, 0, (void*) 0);
}

/** @brief Set Contrast
 *  @param uint8_t contrast (valid range 0x00 (lowest) - 0xFF (highest))
 */
void SSD1308_setContrastControl(uint8_t contrast)
{
    _sendCommandByte(SET_CONTRAST, contrast);  
} 

/** @brief Enable Display
*/ 
void SSD1308_setDisplayOn(void)
{
    _sendCommand(SET_DISPLAY_POWER_ON, 0, (void*) 0);
    is_SSD1308_DisplayOn = 1;
}

/** @brief Disable Display
*/ 
void SSD1308_setDisplayOff(void)
{
    _sendCommand(SET_DISPLAY_POWER_OFF, 0, (void*) 0);
   // is_SSD1308_DisplayOn = 0;
}

/** @brief Enable or Disable Display
 *  @param bool on
 */
void SSD1308_setDisplayPower(bool on)
{
    if(on)
    {
    	//SSD1308_setDisplayOn();
    }
    else
    {
    	//SSD1308_setDisplayOff();
    }
}

/** @brief Show White pixels on Black background
*/ 
void SSD1308_setDisplayNormal(void)
{
    _sendCommand(SET_NORMAL_DISPLAY, 0, (void*) 0);
}

/** @brief Show Black pixels on White background
*/ 
void SSD1308_setDisplayInverse(void)
{
    _sendCommand(SET_INVERSE_DISPLAY, 0, (void*)0);
}

/** @brief Blink display by fading in and out over a set number of frames
 *  @param bool on
 */
void SSD1308_setDisplayBlink(bool on)
{
    if (on)
    {
        _sendCommandByte(SET_FADE_BLINK, (BLINK_ENABLE | FADE_INTERVAL_128_FRAMES));
    }
    else
    {  
        _sendCommandByte(SET_FADE_BLINK, FADE_BLINK_DISABLE);  
    }
}       


/** @brief Fade out display in set number of frames
 *  @param bool on
 */
void SSD1308_setDisplayFade(bool on)
{
    if (on)
    {
        _sendCommandByte(SET_FADE_BLINK, (FADE_OUT_ENABLE | FADE_INTERVAL_128_FRAMES));
    }
    else
    {  
        _sendCommandByte(SET_FADE_BLINK, FADE_BLINK_DISABLE);  
    }
}    

/** @brief Display Flip (Left/Right, Up/Down)
 *  @param bool left flip Left/Right
 *  @param bool down flip Up/Down
 */
void SSD1308_setDisplayFlip(bool left, bool down)
{
    if (left)
    {
        // column address   0 is mapped to SEG0 (Reset)
        _sendCommand(SET_SEGMENT_REMAP_0, 0, (void*)0);
    }
    else
    {
        // column address 127 is mapped to SEG0    
        _sendCommand(SET_SEGMENT_REMAP_127, 0, (void*)0);
    }  

    if (down)
    {
        // Reset mode
        _sendCommand(SET_COMMON_REMAP_0, 0, (void*)0);
    }
    else
    {
        // Flip Up/Down (Need to rewrite display before H effect shows)
        _sendCommand(SET_COMMON_REMAP_63, 0, (void*)0);
    }  

}

/** @brief Sets Internal Iref
*/
void SSD1308_setInternalIref()
{
    //  uint8_t cmds[2] = {SET_IREF_SELECTION, INTERNAL_IREF};

    _sendCommandByte(SET_IREF_SELECTION, INTERNAL_IREF);   
}

/** @brief Sets External Iref (default)
*/
void SSD1308_setExternalIref()
{
    //  uint8_t cmds[2] = {SET_IREF_SELECTION, EXTERNAL_IREF};
    _sendCommandByte(SET_IREF_SELECTION, EXTERNAL_IREF); 
}

/** @brief Shows All Pixels On
*/
void SSD1308_setEntireDisplayOn()
{
    _sendCommand(SET_ENTIRE_DISPLAY_ON, 0, (void*)0); 
}

/** @brief Shows Pixels as RAM content
*/
void SSD1308_setEntireDisplayRAM()
{
    _sendCommand(SET_DISPLAY_GDDRAM, 0, (void*)0);
}

/** @brief Shows Pixels On or as RAM content
 *  @param bool on (true is All on, false is RAM content)
 */
void SSD1308_setEntireDisplay(bool on)
{
    if (on) 
    {
    	SSD1308_setEntireDisplayOn();  // All Pixels on
    }
    else 
    {  
    	SSD1308_setEntireDisplayRAM(); // Pixels are RAM content
    }
}


/** @brief Horizontal scroll by one column per interval
 *  @param bool left select Left/Right scroll
 *  @param uint8_t start_page begin page   (0..MAX_PAGE)
 *  @param uint8_t end_page   end page     (start_page..MAX_PAGE)                     
 *  @param uint8_t interval   scroll interval in frames (see codes above)                      
 */  
void SSD1308_setContinuousHorizontalScroll(bool left, uint8_t start_page, uint8_t end_page, uint8_t interval) 
{
    uint8_t buf[6] = {0x00, start_page, interval, end_page, 0x00, 0xFF};
    if (left) 
    {
        _sendCommand(SET_LEFT_HOR_SCROLL, 6, &buf[0]);  // Scroll Left
    }
    else 
    {  
        _sendCommand(SET_RIGHT_HOR_SCROLL, 6, &buf[0]); // Scroll Right
    }

}

/** @brief Horizontal and Vertical scroll by one column per interval
 *  @param bool left select Left/Right scroll
 *  @param uint8_t start_page begin page   (0..MAX_PAGE)
 *  @param uint8_t end_page   end page     (start_page..MAX_PAGE)                     
 *  @param uint8_t offset     vert offset  (0x01..0x63)                       
 *  @param uint8_t interval   scroll interval in frames (see codes above)                       
 */  
void SSD1308_setContinuousVerticalAndHorizontalScroll(bool left, uint8_t start_page, uint8_t end_page, 
        uint8_t offset, uint8_t interval) 
{

    uint8_t buf[5] = {0x00, start_page, interval, end_page, offset};
    if (left) 
    {
        _sendCommand(SET_VERT_LEFT_HOR_SCROLL, 5, &buf[0]);  // Scroll Left
    }
    else 
    {  
        _sendCommand(SET_VERT_RIGHT_HOR_SCROLL, 5, &buf[0]); // Scroll Right  
    }
}    

/** @brief Set Vertical scroll area
 *  @param uint8_t topRowsFixed      fixed rows   (0..MAX_ROW)                     
 *  @param uint8_t scrollRowsoffset  scroll rows  (topRowsFixed..ROWS)                       
 */  
void SSD1308_setVerticalScrollArea(uint8_t topRowsFixed, uint8_t scrollRows) 
{ 

    uint8_t buf[2] = {topRowsFixed, scrollRows};
    if ((topRowsFixed + scrollRows) > ROWS) 
    {
        scrollRows = ROWS - topRowsFixed; 
    };

    _sendCommand(SET_VERTICAL_SCROLL_AREA, 2, &buf[0]);
}

/** @brief Activate or Deactivate Horizontal and Vertical scroll
 *  @brief Note: after deactivating scrolling, the RAM data needs to be rewritten
 *  @param bool on activate scroll 
 */  
void SSD1308_setDisplayScroll(bool on) 
{
    if (on) 
    {
        _sendCommand(SET_ACTIVATE_SCROLL, 0, (void*)0);   // Scroll on
    }
    else 
    {  
        _sendCommand(SET_DEACTIVATE_SCROLL, 0, (void*)0); // Scroll off  
    }
}

//================================================================================
// Internal helper function implement
//================================================================================

static void _sendCommand(uint8_t command, uint8_t len, uint8_t* pParam)
{

    uint8_t i = 0;
#if 1
    _i2c_start();
    _i2c_write_addr(_writeOpcode);

    _i2c_write(COMMAND_MODE);
    _i2c_write(command);              // Write Command

    for(i = 0; i < len; i++)
    {
        _i2c_write(COMMAND_MODE);
        _i2c_write(*pParam++);        // Write Param
    }

    _i2c_stop();
#endif

#if 0
    if(len == 0)
    {
    	xI2CMasterWriteS1(ulMaster, _writeOpcode, COMMAND_MODE, xfalse);
    	xI2CMasterWriteS2(ulMaster, command, xtrue);
    }
    else
    {
    	xI2CMasterWriteS1(ulMaster, _writeOpcode, COMMAND_MODE, xfalse);
    	xI2CMasterWriteS2(ulMaster, command, xfalse);

    	for(i = 0; i < (len - 1); i++)
    	{
            xI2CMasterWriteS2(ulMaster, COMMAND_MODE, xfalse);
            xI2CMasterWriteS2(ulMaster, *pParam++, xfalse);
    	}

        xI2CMasterWriteS2(ulMaster, COMMAND_MODE, xfalse);
        xI2CMasterWriteS2(ulMaster, *pParam++, xtrue);
    }
#endif
}

static void _sendCommandByte(uint8_t command, uint8_t data)
{
#if 1
    _i2c_start();
    _i2c_write_addr(_writeOpcode);

    _i2c_write(COMMAND_MODE);
    _i2c_write(command);              // Write Command

    _i2c_write(COMMAND_MODE);
    _i2c_write(data);            // Write Param

    _i2c_stop();
#endif

#if 0

	xI2CMasterWriteS1(ulMaster, _writeOpcode, COMMAND_MODE, xfalse);
	xI2CMasterWriteS2(ulMaster, command, xfalse);

	xI2CMasterWriteS2(ulMaster, COMMAND_MODE, xfalse);
	xI2CMasterWriteS2(ulMaster, data, xtrue);

#endif

}

static void _sendDataByte(uint8_t data)
{
#if 1
    _i2c_start();
    _i2c_write_addr(_writeOpcode);
    _i2c_write(DATA_MODE);
    _i2c_write(data);
    _i2c_stop();
#endif

#if 0

	xI2CMasterWriteS1(ulMaster, _writeOpcode, DATA_MODE, xfalse);
	xI2CMasterWriteS2(ulMaster, data, xtrue);

#endif

}

static void _sendData(uint8_t len, uint8_t* pData)
{

    uint8_t i = 0;
#if 1

    _i2c_start();
    _i2c_write_addr(_writeOpcode);
    _i2c_write(DATA_MODE);
    for (i=0; i<len ; i++)
    {
        _i2c_write(*pData++);  // Write Data
    }
    _i2c_stop();
#endif

#if 0
    if(len == 0)
    {
    	return; //Error
    }
    else
    {
    	xI2CMasterWriteS1(ulMaster, _writeOpcode, DATA_MODE, xfalse);

    	for(i = 0; i < (len - 1); i++)
    	{
            xI2CMasterWriteS2(ulMaster, *pData++, xfalse);
    	}

    	xI2CMasterWriteS2(ulMaster, *pData++, xtrue);

    }
#endif

}

void LCDI2C_WriteCommand(uint8_t _data){
I2C1_Start(OLED_DATA_ADDR);
I2C1_SendByte(OLED_COMMAND_MODE);
I2C1_SendByte(_data);
I2C1_Stop();
}

// передача данных
void LCDI2C_WriteData(uint8_t _data){
I2C1_Start(OLED_DATA_ADDR); //Wire.beginTransmission(_Addr);
I2C1_SendByte(OLED_DATA_MODE);
I2C1_SendByte(_data);
I2C1_Stop();
//I2C_GenerateSTOP(I2C1, ENABLE); //Wire.endTransmission();
}


static void _LowLayer_init(void)
{
/*
    _sendCommand(SET_DISPLAY_POWER_OFF, 0, (void*)0);      // 0xAE

    // column address   0 is mapped to SEG0 (Reset)    
    // row address   0 is mapped to COM0 (Reset)      
    _sendCommand(SET_SEGMENT_REMAP_0, 0, (void*)0);        // 0xA0 (Reset)
    _sendCommand(SET_COMMON_REMAP_0, 0, (void*)0);         // 0xC0 (Reset) 

    SSD1308_setDisplayStartLine(0);                   // 0x40 (Reset) 

    _sendCommandByte(SET_COMMON_CONF, COMMON_BASE | COMMON_ALTERNATIVE | COMMON_LEFTRIGHT_NORMAL); // 0xDA, 0x12 (Reset)

    // Pagemode or Horizontal mode
    //  SSD1308_setPageAddressingMode();                  // 0x20, 0x02 (Reset)  
    //  SSD1308_setColumnStartForPageAddressingMode(0);   // 0x00, 0x10 (Reset = Column 0)
    //  SSD1308_setPageStartForPageAddressingMode(PAGE_0);// 0xBO       (Reset = Page 0)
    SSD1308_setHorizontalAddressingMode();            // 0x20, 0x00 (Non-Reset)
    SSD1308_setColumnAddress(0, MAX_COL);             // 0x21, 0x00, 0x37 (Reset)
    SSD1308_setPageAddress(0, MAX_PAGE);              // 0x22, 0x00, 0x07 (Reset)

    SSD1308_setExternalIref();                        // 0xAD, 0x10 (Reset)

    _sendCommandByte(SET_DISPLAY_CLOCK, 0x70);        // 0xD5, 0x70 (Reset = 0x80)
    _sendCommandByte(SET_PRECHARGE_TIME, 0x21);       // 0xD9, 0x21 (Reset = 0x22)
    _sendCommandByte(SET_VCOMH_DESELECT_LEVEL, 0x30); // 0xDB, 0x30 (Reset = 0x20)
    _sendCommandByte(SET_MULTIPLEX_RATIO, 0x3F);      // 0xA8, 0x3F (Reset)
    _sendCommandByte(SET_DISPLAY_OFFSET, 0x00);       // 0xD3, 0x00 (Reset)

    _sendCommandByte(SET_CONTRAST, 0x7F);             // 0x81, 0x7F (Reset)

    _sendCommand(SET_NORMAL_DISPLAY, 0, (void*)0);    // 0xA6 (Reset)

    SSD1308_setEntireDisplayRAM();                    // 0xA4 (Reset)
    SSD1308_setDisplayScroll(false);

    SSD1308_clearDisplay();
    SSD1308_setInverted(false);
    SSD1308_setDisplayOn();
    _sendCommand(SET_DISPLAY_POWER_ON, 0, (void*)0);   // 0xAF*/

	LCDI2C_WriteCommand(0xAE); //display off
	LCDI2C_WriteCommand(0x20); //Set Memory Addressing Mode
	LCDI2C_WriteCommand(0x00); //00,Horizontal Addressing Mode;01,Vertical
	//Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	LCDI2C_WriteCommand(0xB0); //Set Page Start Address for Page Addressing
	//Mode,0-7
	LCDI2C_WriteCommand(0xC8); //Set COM Output Scan Direction
	LCDI2C_WriteCommand(0x00); //---set low column address
	LCDI2C_WriteCommand(0x10); //---set high column address
	LCDI2C_WriteCommand(0x40); //--set start line address
	LCDI2C_WriteCommand(0x81); //--set contrast control register
	LCDI2C_WriteCommand(0xFF);
	LCDI2C_WriteCommand(0xA1); //--set segment re-map 0 to 127
	LCDI2C_WriteCommand(0xA6); //--set normal display
	LCDI2C_WriteCommand(0xA8); //--set multiplex ratio(1 to 64)
	LCDI2C_WriteCommand(0x3F); //
	LCDI2C_WriteCommand(0xA4); //0xa4,Output follows RAM
	//content;0xa5,Output ignores RAM content
	LCDI2C_WriteCommand(0xD3); //-set display offset
	LCDI2C_WriteCommand(0x00); //-not offset
	LCDI2C_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
	LCDI2C_WriteCommand(0xF0); //--set divide ratio
	LCDI2C_WriteCommand(0xD9); //--set pre-charge period
	LCDI2C_WriteCommand(0x22); //
	LCDI2C_WriteCommand(0xDA); //--set com pins hardware configuration
	LCDI2C_WriteCommand(0x12);
	LCDI2C_WriteCommand(0xDB); //--set vcomh
	LCDI2C_WriteCommand(0x20); //0x20,0.77xVcc
	LCDI2C_WriteCommand(0x8D); //--set DC-DC enable
	LCDI2C_WriteCommand(0x14); //
	LCDI2C_WriteCommand(0x2E); // stop scrolling
	LCDI2C_WriteCommand(0xAF); //--turn on SSD1306 panel вот здесь должен засветиться "мусор" на экране
}

