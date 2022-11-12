#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "Not a cross compiler"
#endif

#if !defined(__i386__)
#error "You need an i386 elf compiler"
#endif

enum VGAColour {
  VGA_COLOUR_BLACK = 0,
  VGA_COLOUR_BLUE = 1,
  VGA_COLOUR_GREEN = 2,
  VGA_COLOUR_CYAN = 3,
  VGA_COLOUR_RED = 4,
  VGA_COLOUR_MAGENTA = 5,
  VGA_COLOUR_BROWN = 6,
  VGA_COLOUR_LIGHT_GREY = 7,
  VGA_COLOUR_DARK_GREY = 8,
  VGA_COLOUR_LIGHT_BLUE = 9,
  VGA_COLOUR_LIGHT_GREEN = 10,
  VGA_COLOUR_LIGHT_CYAN = 11,
  VGA_COLOUR_LIGHT_MAGENTA = 13,
  VGA_COLOUR_LIGHT_BROWN = 14,
  VGA_COLOUR_WHITE = 15,
};

static inline uint8_t vgaEntryColour(enum VGAColour fore, enum VGAColour back) {
  return fore | back << 4;
}

static inline uint16_t vgaEntry(unsigned char c, uint8_t colour) {
  return (uint16_t)c | (uint16_t)colour << 8;
}

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len]) {
    len++;
  }
  return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminalRow;
size_t terminalCol;
uint8_t terminalColour;
uint16_t *terminalBuf;

void terminalInit(void) {
  terminalRow = 0;
  terminalCol = 0;
  terminalColour = vgaEntryColour(VGA_COLOUR_WHITE, VGA_COLOUR_RED);
  terminalBuf = (uint16_t *)0xB8000;

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminalBuf[index] = vgaEntry(' ', terminalColour);
    }
  }
}

void terminalSetColour(uint8_t colour) { terminalColour = colour; }
void terminalPutEntryAt(char c, uint8_t colour, size_t x, size_t y) {
  const size_t i = y * VGA_WIDTH + x;
  terminalBuf[i] = vgaEntry(c, colour);
}

void terminalPutChar(char c) {
  terminalPutEntryAt(c, terminalColour, terminalCol, terminalRow);
  if (++terminalCol == VGA_WIDTH) {
    terminalCol = 0;
    if (++terminalRow == VGA_HEIGHT) {
      terminalRow = 0;
    }
  }
}

void terminalWrite(const char *dat, size_t size) {
  for (size_t i = 0; i < size; i++) {
    terminalPutChar(dat[i]);
  }
}

void terminalWriteString(const char *data) {
  terminalWrite(data, strlen(data));
}

void kernelMain(void) {
  terminalInit();
  terminalWriteString("Hello, World!\n");
}
