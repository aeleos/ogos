#include <stdio.h>
#include <stdlib.h>
#include <vga.h>
void terminal_clrscreen(void);
void terminal_scroll(void);
void terminal_setcolor(uint8_t color);
void move_cursor(void);

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;


void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_DARK_GREY));
	terminal_buffer = VGA_MEMORY;
	terminal_clrscreen();
}

void terminal_clrscreen(void){
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
	move_cursor();
}

void terminal_scroll(void){
  for (size_t y = 0; y <= VGA_HEIGHT; y++) {
    for (size_t x = 0; x <= VGA_WIDTH; x++) {
      const size_t tempindex = y * VGA_WIDTH + x;
      const size_t index = tempindex - VGA_WIDTH;
      if (y == VGA_HEIGHT)
       terminal_buffer[index] = make_vgaentry(' ', terminal_color);
      else if (y != 0) {
        terminal_buffer[index] = terminal_buffer[tempindex];
      }
    }
  }
	move_cursor();
}



void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_putchar(char c)
{
	if (c == '\n'){
		terminal_column = 0;
		if (terminal_row == VGA_HEIGHT) {
			terminal_scroll();
			terminal_row = 24;
		} else {
			terminal_row++;
		}
		move_cursor();
		return;
	}
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT) {
      terminal_scroll();
      terminal_row = 24;
    } else {
      terminal_row++;
    }
  }
	move_cursor();
}

void move_cursor()
{
   // The screen is 80 characters wide...
	const size_t index = terminal_row * VGA_WIDTH + terminal_column;

	 // cursor LOW port to vga INDEX register
	outb(0x3D4, 14); // Tell the VGA board we are setting the high cursor byte.
 	outb(0x3D5, index >> 8); // Send the high cursor byte.
 	outb(0x3D4, 15); // Tell the VGA board we are setting the low cursor byte.
 	outb(0x3D5, index); // Send the low cursor byte.


}

void terminal_write(const char* data, size_t size)
{
	for ( size_t i = 0; i < size; i++ )
		terminal_putchar(data[i]);
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}
