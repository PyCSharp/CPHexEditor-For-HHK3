#include <appdef.h>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sdk/os/debug.h>
#include <sdk/os/input.h>
#include <sdk/os/lcd.h>
#include <sdk/os/mem.h>
#include <sys/_intsup.h>
#include <sdk/os/file.h>
#include <sys/_types.h>
#include <sys/unistd.h>
#include <unistd.h>

APP_NAME("HexEditor")
APP_DESCRIPTION("A simple hex editor. Can read and write byte, word and long. github.com/PyCSharp/CPHexEditor-For-HHK3")
APP_AUTHOR("SnailMath, PyCSharp")
APP_VERSION("1.0.0")

#define PIXEL(x, y) (vram[(x) + (y) * width])
#define mem_ch(x) numToAscii[memory[x]]

unsigned width, height;
uint16_t *vram;

unsigned char numToAscii[257] =
    "................................ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~.................................................................................................................................";

#define underlineLeft(x) ((x < 4) ? (18 + (18 * x)) : (24 + (18 * x)))
#define underlineLeftAscii(x) ((x < 4) ? (174 + (6 * x)) : (180 + (6 * x)))
#define underlineTop(y) (71 + (12 * y))

void initscreen();
void hexdump();

#define MAXinput 32
#if MAXinput % 25 == 1
#error MAXinput is not even
#endif

char input[MAXinput + 3];
char search[MAXinput / 2];
uint8_t *searchAddr;
char searchdirection = 0;
char searchlen = 0;
unsigned char inputpos;
uint8_t timeBackPressed = 1;

uint8_t *memory = (uint8_t*)0x808fcba0;
char cursorx = 2;
char cursory = 2;

static uintptr_t start = 0x00000000;
static uintptr_t end = 0x00000000;

static inline unsigned int save_sr(void) {
  unsigned int sr;
  __asm__ volatile("stc sr, %0" : "=r"(sr));
  return sr;
}

static inline void write_sr(unsigned int sr) {
  __asm__ volatile("ldc %0, sr" : : "r"(sr));
}

inline uint8_t hexCharToByte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0; 
}

size_t hexInputToText(const char* input, uint8_t* output, size_t maxOutputLen) {
    if (!input || !output) return 0;

    size_t i = 0; 
    size_t outPos = 0;

    if (input[0] == '>') i = 1;

    while (input[i] && input[i+1] && outPos < maxOutputLen) {
        if (!isxdigit(input[i]) || !isxdigit(input[i+1])) break;

        uint8_t high = hexCharToByte(input[i]);
        uint8_t low  = hexCharToByte(input[i+1]);
        output[outPos++] = (high << 4) | low;
        i += 2;
    }

    return outPos;
}

void setDumpAddresses()
{
    if (start == 0) 
    {
        start = (uintptr_t)memory;
    } 
    else 
    {
        end = (uintptr_t)memory;
    }
}

void writeHexDumpToFile() 
{
    File_Remove("\\fls0\\dump.dmp");

    unsigned int sr = save_sr();

    uintptr_t startMemoryAddress = start;
    uintptr_t endMemoryAddress = end;
    write_sr(0x100000F0 | sr);

    int fd = File_Open("\\fls0\\dump.dmp", FILE_OPEN_WRITE | FILE_OPEN_CREATE | FILE_OPEN_APPEND);
    
    char hexDumpTableHeader[57] = "Offset:\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n";

    auto _ = File_Write(fd, hexDumpTableHeader, sizeof(hexDumpTableHeader) - 1);

    char hexTextBuffer[4];
    char addressTextBuffer[32];

    for (uintptr_t addr = startMemoryAddress; addr <= endMemoryAddress; addr+=16) {
        int len = snprintf(addressTextBuffer, sizeof(addressTextBuffer), "%08zx", addr);
        [[maybe_unused]] auto f = File_Write(fd, addressTextBuffer, len);

        for (uintptr_t offset = 0; offset < 16; offset++) {
            uintptr_t currentAddr = addr + offset;
            if (currentAddr > endMemoryAddress) break;

            snprintf(hexTextBuffer, sizeof(hexTextBuffer), " %02X", *reinterpret_cast<uint8_t*>(currentAddr));
            [[maybe_unused]] auto f = File_Write(fd, hexTextBuffer, strlen(hexTextBuffer));
        }

        [[maybe_unused]] auto f4 = File_Write(fd, "\r\n", 2);
    }

    start = (uintptr_t)0x00000000;
    end = (uintptr_t)0x00000000;                                                                                                                                                                                                                                   
    File_Close(fd);
}

void searchForText() 
{
    File_Remove("\\fls0\\FoundAddresses.txt");

    unsigned int sr = save_sr();
    write_sr(0x100000F0 | sr);
    int fd = File_Open("\\fls0\\FoundAddresses.txt", FILE_OPEN_CREATE | FILE_OPEN_WRITE | FILE_OPEN_APPEND);

    char buffer[33];
    char addressTextBuffer[32];
    uint8_t text[32];

    size_t len = hexInputToText(input, text, sizeof(text));
    for (size_t i = 0; i < len; i++) buffer[i] = text[i];
    buffer[len] = '\0';

    std::printf("Searching for: %s\n", buffer);
    std::fflush(stdout);
    Debug_WaitKey();

    for (uintptr_t addr = start; addr <= end - len + 1; addr++) 
    {
        int addrLen = snprintf(addressTextBuffer, sizeof(addressTextBuffer), "%08zx\r\n", addr);

        size_t match = 0;
        for (; match < len; match++) 
        {
            if (*(char*)(addr + match) != buffer[match])
                break;
        }
        if (match == len) 
        {
            auto _ = File_Write(fd, addressTextBuffer, addrLen);

            std::printf("Found at: 0x%08zx\n", addr);
            std::fflush(stdout);
            Debug_WaitKey();
            addr += match - 1;
        }
    }

    start = (uintptr_t)0x00000000;
    end = (uintptr_t)0x00000000;

    File_Close(fd);
    write_sr(sr);
}

int main() {
    LCD_GetSize(&width, &height);
    vram = LCD_GetVRAMAddress();

    input[0] = '>';
    input[1] = '_';
    input[2] = 0;
    inputpos = 1;

    memory = (uint8_t*)0x8cff0d00;

    struct Input_Event event;
    LCD_VRAMBackup();
    initscreen();
    LCD_Refresh();

    bool running = true;
    while (running) {
        Mem_Memset(&event, 0, sizeof(event));
        GetInput(&event, 0xFFFFFFFF, 0x10);

        switch (event.type) {
        case EVENT_KEY:
            if (event.data.key.direction == KEY_PRESSED) {
                char ch = ' ';
                bool typed = false;

                if (event.data.key.keyCode >= KEYCODE_0 && event.data.key.keyCode <= KEYCODE_9) {
                    ch = event.data.key.keyCode + ('0' - KEYCODE_0);
                    typed = true;
                }

                if (event.data.key.keyCode == KEYCODE_EXE) writeHexDumpToFile();
                if (event.data.key.keyCode == KEYCODE_EXP) setDumpAddresses();
                if (event.data.key.keyCode == KEYCODE_NEGATIVE) searchForText();
                if (event.data.key.keyCode == KEYCODE_EQUALS) ch = 'A', typed = true;
                if (event.data.key.keyCode == KEYCODE_X) ch = 'B', typed = true;
                if (event.data.key.keyCode == KEYCODE_Y) ch = 'C', typed = true;
                if (event.data.key.keyCode == KEYCODE_Z) ch = 'D', typed = true;
                if (event.data.key.keyCode == KEYCODE_POWER) ch = 'E', typed = true;
                if (event.data.key.keyCode == KEYCODE_DIVIDE) ch = 'F', typed = true;
                if (event.data.key.keyCode == KEYCODE_RIGHT) cursorx++;
                if (event.data.key.keyCode == KEYCODE_LEFT) cursorx--;
                if (event.data.key.keyCode == KEYCODE_DOWN) cursory++;
                if (event.data.key.keyCode == KEYCODE_UP) cursory--;

                if (cursorx > 7) cursorx = 0, cursory++;
                if (cursorx < 0) cursorx = 7, cursory--;
                if (cursory > 31) cursory = 0, memory += 0x100;
                if (cursory < 0) cursory = 31, memory -= 0x100;

                if (typed) {
                    if (inputpos <= MAXinput) {
                        input[inputpos++] = ch;
                        input[inputpos] = '\0';
                        input[inputpos + 1] = 0;
                        if (inputpos == (MAXinput + 1)) input[inputpos] = 0;
                    }
                }

                if (event.data.key.keyCode == KEYCODE_POWER_CLEAR)
                {
                    searchdirection = 0;
                    running = false;
                    continue;
                }

                if (event.data.key.keyCode == KEYCODE_BACKSPACE) {
                    if (inputpos > 1) {
                        input[inputpos--] = 0;
                        input[inputpos] = '_';
                    }
                }
            }
            else if (event.data.key.direction == KEY_HELD) {
                if (event.data.key.keyCode == KEYCODE_BACKSPACE) {
                    if (timeBackPressed < 20) timeBackPressed *= 2;
                    for (uint8_t i = 0; i < timeBackPressed; i++) {
                        if (inputpos > 1) {
                            input[inputpos--] = 0;
                            input[inputpos] = '_';
                        }
                    }
                }
            }
            else if ((Input_KeyEventType)event.data.touch_single.direction == KEY_RELEASED) {
                timeBackPressed = 1;
            }

            initscreen();
            LCD_Refresh();
            break;

        case EVENT_TOUCH: {
            unsigned int x = (unsigned int) event.data.touch_single.p1_x;
            unsigned int y = (unsigned int) event.data.touch_single.p1_y;

            Debug_Printf(46, 2, false, 0, "%3d %3d", x, y);

            if (event.data.touch_single.direction == TOUCH_UP) {
                if (y >= 497 && y <= 521) {
                    bool newSearch = false;

                    if (x <= 117 && x >= 93) {
                        searchdirection = 1;
                        newSearch = true;
                        searchAddr = memory + (cursory * 8) + cursorx + 1;
                    }
                    else if (x <= 81 && x >= 57) {
                        searchdirection = -1;
                        newSearch = true;
                        searchAddr = memory + (cursory * 8) + cursorx - 1;
                    }

                    if (newSearch) {
                        if (inputpos % 2 == 1 && inputpos > 1) {
                            for (int i = 0; i < (MAXinput / 2); i++) {
                                search[i] = (((input[1 + (2 * i)] <= '9') ? (input[1 + (2 * i)] - '0')
                                                                          : (input[1 + (2 * i)] + (10 - 'A'))) << 4) +
                                            (((input[2 + (2 * i)] <= '9') ? (input[2 + (2 * i)] - '0')
                                                                          : (input[2 + (2 * i)] + (10 - 'A'))));
                            }
                            searchlen = (inputpos - 1) / 2;
                        } else {
                            searchdirection = 0;
                        }
                    }

                    if (x >= 135 && x <= 177) {
                        if (inputpos >= 2) {
                            uint32_t newaddr = (((input[1] <= '9') ? (input[1] - '0') : (input[1] + (10 - 'A'))) << 28);
                            if (inputpos >= 3) newaddr += (((input[2] <= '9') ? (input[2] - '0') : (input[2] + (10 - 'A'))) << 24);
                            if (inputpos >= 4) newaddr += (((input[3] <= '9') ? (input[3] - '0') : (input[3] + (10 - 'A'))) << 20);
                            if (inputpos >= 5) newaddr += (((input[4] <= '9') ? (input[4] - '0') : (input[4] + (10 - 'A'))) << 16);
                            if (inputpos >= 6) newaddr += (((input[5] <= '9') ? (input[5] - '0') : (input[5] + (10 - 'A'))) << 12);
                            if (inputpos >= 7) newaddr += (((input[6] <= '9') ? (input[6] - '0') : (input[6] + (10 - 'A'))) << 8);
                            if (inputpos == 8) cursory = (((input[7] <= '9') ? (input[7] - '0') : (input[7] + (10 - 'A'))) << 1), cursorx = 0;
                            if (inputpos == 9) {
                                cursory = (((input[7] <= '9') ? (input[7] - '0') : (input[7] + (10 - 'A'))) << 1) +
                                          (((input[8] <= '9') ? (input[8] - '0') : (input[8] + (10 - 'A'))) >> 3);
                                cursorx = (((input[8] <= '9') ? (input[8] - '0') : (input[8] + (10 - 'A'))) & 7);
                            }
                            memory = (uint8_t*)(uintptr_t)newaddr;
                        }
                    }

                    if (x >= 261 && x <= 309) {
                        uint32_t write = 0;
                        if (inputpos == 3) {
                            write = (((input[1] <= '9') ? (input[1] - '0') : (input[1] + (10 - 'A'))) << 4) +
                                    ((input[2] <= '9') ? (input[2] - '0') : (input[2] + (10 - 'A')));
                            asm volatile("mov.b %1, @%0" :: "r"(memory + cursorx + (cursory * 8)), "r"(write));
                        }
                        if (inputpos == 5) {
                            write = (((input[1] <= '9') ? (input[1] - '0') : (input[1] + (10 - 'A'))) << 12) +
                                    (((input[2] <= '9') ? (input[2] - '0') : (input[2] + (10 - 'A'))) << 8) +
                                    (((input[3] <= '9') ? (input[3] - '0') : (input[3] + (10 - 'A'))) << 4) +
                                    ((input[4] <= '9') ? (input[4] - '0') : (input[4] + (10 - 'A')));
                            asm volatile("mov.w %1, @%0" :: "r"(memory + cursorx + (cursory * 8)), "r"(write));
                        }
                        if (inputpos == 9) {
                            write = (((input[1] <= '9') ? (input[1] - '0') : (input[1] + (10 - 'A'))) << 28) +
                                    (((input[2] <= '9') ? (input[2] - '0') : (input[2] + (10 - 'A'))) << 24) +
                                    (((input[3] <= '9') ? (input[3] - '0') : (input[3] + (10 - 'A'))) << 20) +
                                    (((input[4] <= '9') ? (input[4] - '0') : (input[4] + (10 - 'A'))) << 16) +
                                    (((input[5] <= '9') ? (input[5] - '0') : (input[5] + (10 - 'A'))) << 12) +
                                    (((input[6] <= '9') ? (input[6] - '0') : (input[6] + (10 - 'A'))) << 8) +
                                    (((input[7] <= '9') ? (input[7] - '0') : (input[7] + (10 - 'A'))) << 4) +
                                    ((input[8] <= '9') ? (input[8] - '0') : (input[8] + (10 - 'A')));
                            asm volatile("mov.l %1, @%0" :: "r"(memory + cursorx + (cursory * 8)), "r"(write));
                        }
                    }

                    if (x >= 201 && x <= 243) {
                        uint32_t read = 0;
                        if (inputpos == 3) {
                            asm volatile("mov.b @%1, %0" : "=r"(read) : "r"(memory + cursorx + (cursory * 8)));
                            input[1] = (((read >> 4) & 0xf) < 10) ? (((read >> 4) & 0xf) + '0') : (((read >> 4) & 0xf) + ('A' - 10));
                            input[2] = ((read & 0xf) < 10) ? ((read & 0xf) + '0') : ((read & 0xf) + ('A' - 10));
                        }
                        if (inputpos == 5 && cursorx % 2 == 0) {
                            asm volatile("mov.w @%1, %0" : "=r"(read) : "r"(memory + cursorx + (cursory * 8)));
                            input[1] = (((read >> 12) & 0xf) < 10) ? (((read >> 12) & 0xf) + '0') : (((read >> 12) & 0xf) + ('A' - 10));
                            input[2] = (((read >> 8) & 0xf) < 10) ? (((read >> 8) & 0xf) + '0') : (((read >> 8) & 0xf) + ('A' - 10));
                            input[3] = (((read >> 4) & 0xf) < 10) ? (((read >> 4) & 0xf) + '0') : (((read >> 4) & 0xf) + ('A' - 10));
                            input[4] = ((read & 0xf) < 10) ? ((read & 0xf) + '0') : ((read & 0xf) + ('A' - 10));
                        }
                        if (inputpos == 9 && cursorx % 4 == 0) {
                            asm volatile("mov.l @%1, %0" : "=r"(read) : "r"(memory + cursorx + (cursory * 8)));
                            input[1] = (((read >> 28) & 0xf) < 10) ? (((read >> 28) & 0xf) + '0') : (((read >> 28) & 0xf) + ('A' - 10));
                            input[2] = (((read >> 24) & 0xf) < 10) ? (((read >> 24) & 0xf) + '0') : (((read >> 24) & 0xf) + ('A' - 10));
                            input[3] = (((read >> 20) & 0xf) < 10) ? (((read >> 20) & 0xf) + '0') : (((read >> 20) & 0xf) + ('A' - 10));
                            input[4] = (((read >> 16) & 0xf) < 10) ? (((read >> 16) & 0xf) + '0') : (((read >> 16) & 0xf) + ('A' - 10));
                            input[5] = (((read >> 12) & 0xf) < 10) ? (((read >> 12) & 0xf) + '0') : (((read >> 12) & 0xf) + ('A' - 10));
                            input[6] = (((read >> 8) & 0xf) < 10) ? (((read >> 8) & 0xf) + '0') : (((read >> 8) & 0xf) + ('A' - 10));
                            input[7] = (((read >> 4) & 0xf) < 10) ? (((read >> 4) & 0xf) + '0') : (((read >> 4) & 0xf) + ('A' - 10));
                            input[8] = ((read & 0xf) < 10) ? ((read & 0xf) + '0') : ((read & 0xf) + ('A' - 10));
                        }
                    }
                }

                if (y <= 89 && y >= 65) {
                    if (x >= 243 && x <= 267) memory -= 0x100;
                    if (x >= 279 && x <= 303) memory += 0x100;
                }
            }
            break;
        }
        case EVENT_ACTBAR_RESIZE:
        case EVENT_ACTBAR_SWAP:
        case EVENT_ACTBAR_ROTATE:
        case EVENT_ACTBAR_ESC:
        case EVENT_ACTBAR_SETTINGS:
        case EVENT_TIMER:
          break;
        }

        if (searchdirection) {
            asm("searchloop:\n"
                "add %3, %1;"
                "mov.b @%1, r0;"
                "cmp/eq r0, %2;"
                "bf searchloop;"
                "nop;"
                "mov %1, %0"
                : "=r"(searchAddr)
                : "r"(searchAddr), "r"(search[0]), "r"(searchdirection));

            uint8_t i = 0;
            while (true) {
                if (search[i] != (uint8_t)*(searchAddr + i)) break;
                i++;
                if (i == searchlen) {
                    searchdirection = 0;
                    memory = (uint8_t*)((uintptr_t)searchAddr & 0xffffff00);
                    cursorx = (uintptr_t)searchAddr & 0x00000007;
                    cursory = ((uintptr_t)searchAddr & 0x000000f8) >> 3;
                    break;
                }
            }
        }

        initscreen();
        LCD_Refresh();
    }

    LCD_VRAMRestore();
    LCD_Refresh();
}

void initscreen() {
    LCD_ClearScreen();
    Debug_SetCursorPosition(1, 0);

    Debug_PrintString("HexEditor      PyCSharp", 0);
    Debug_Printf(21, 1, false, 0, "github.com/");
    Debug_Printf(0, 4, false, 0, "   0  1  2  3   4  5  6  7");
    hexdump();
    Debug_Printf(40, 4, false, 0, "0x%08X", (unsigned int)(uintptr_t)memory);
    Debug_Printf(40, 5, false, 0, "+---+ +---+");
    Debug_Printf(40, 6, false, 0, "| < | | > |");
    Debug_Printf(40, 7, false, 0, "+---+ +---+");
    Debug_Printf(1, 39, false, 0, "%8s", input);
    Debug_Printf(9, 40, false, 0, "0x%08X", (unsigned int)(uintptr_t)searchAddr);
    Debug_Printf(9, 41, false, 0, "+---+ +---+  +------+   +------+  +-------+");
    Debug_Printf(1, 42, false, 0, "Search: | < | | > |  | Goto |   | Read |  | Write |");
    Debug_Printf(9, 43, false, 0, "+---+ +---+  +------+   +------+  +-------+");
}

void hexdump() {
    if (!memory) return;

    for (int i = 0; i < 16; i++) {
        Debug_Printf(1, 5 + (2 * i), false, 0,
                     "%X %02X %02X %02X %02X  %02X %02X %02X %02X  %c%c%c%c %c%c%c%c", i,
                     memory[0 + (16 * i)], memory[1 + (16 * i)], memory[2 + (16 * i)], memory[3 + (16 * i)],
                     memory[4 + (16 * i)], memory[5 + (16 * i)], memory[6 + (16 * i)], memory[7 + (16 * i)],
                     mem_ch(0 + (16 * i)), mem_ch(1 + (16 * i)), mem_ch(2 + (16 * i)), mem_ch(3 + (16 * i)),
                     mem_ch(4 + (16 * i)), mem_ch(5 + (16 * i)), mem_ch(6 + (16 * i)), mem_ch(7 + (16 * i)));

        Debug_Printf(1, 6 + (2 * i), false, 0,
                     "  %02X %02X %02X %02X  %02X %02X %02X %02X  %c%c%c%c %c%c%c%c",
                     memory[8 + (16 * i)], memory[9 + (16 * i)], memory[10 + (16 * i)], memory[11 + (16 * i)],
                     memory[12 + (16 * i)], memory[13 + (16 * i)], memory[14 + (16 * i)], memory[15 + (16 * i)],
                     mem_ch(8 + (16 * i)), mem_ch(9 + (16 * i)), mem_ch(10 + (16 * i)), mem_ch(11 + (16 * i)),
                     mem_ch(12 + (16 * i)), mem_ch(13 + (16 * i)), mem_ch(14 + (16 * i)), mem_ch(15 + (16 * i)));
    }

    for (int x = underlineLeft(cursorx); x <= underlineLeft(cursorx) + 12; x++)
        PIXEL(x, underlineTop(cursory)) = 0b1111100000000000;

    for (int x = underlineLeftAscii(cursorx); x <= underlineLeftAscii(cursorx) + 6; x++)
        PIXEL(x, underlineTop(cursory)) = 0;
}
