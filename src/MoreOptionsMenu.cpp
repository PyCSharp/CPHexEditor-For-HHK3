#include "MoreOptionsMenu.h"

bool selecting = true;

void Button(unsigned int width, unsigned int height, unsigned x, unsigned y) {
    line(x, y, x + width, y, color(0, 128, 0));
    line(x, y, x, y + height, color(0, 128, 0));
    line(x, y + height, x + width, y + height, color(0, 128, 0));
    line(x + width, y + height, x + width, y, color(0, 128, 0));
}

uint32_t ReadControlRegister(ControlRegister controlRegister) {
    uint32_t vbr, ssr, spc, sgr, dbr, gbr, sr = 0;

    switch (controlRegister) {
        case ControlRegister::VBR:
            asm volatile(
                "stc vbr, %0"
                : "=r"(vbr)
            );

            return vbr;

        case ControlRegister::SSR:
            asm volatile(
                "stc ssr, %0"
                :"=r"(ssr)
            );

            return ssr;

        case ControlRegister::SPC:
            asm volatile(
                "stc spc, %0"
                :"=r"(spc)
            );

            return spc;

        case ControlRegister::SGR:
            asm volatile(
                "stc sgr, %0"
                :"=r"(sgr)
            );

            return sgr;


        case ControlRegister::DBR:
            asm volatile(
                "stc dbr, %0"
                :"=r"(dbr)
            );

            return dbr;

        case ControlRegister::GBR:
            asm volatile(
                "stc gbr, %0"
                :"=r"(gbr)
            );

            return gbr;

        case ControlRegister::SR:
            asm volatile(
                "stc sr, %0"
                :"=r"(sr)
            );

            return sr;
    }

    return 0;
}

void WriteControlRegister(ControlRegister controlRegister, uint32_t value) {
    switch (controlRegister) {
        case ControlRegister::VBR:
            asm volatile(
                "ldc %0, vbr"
                ::"r"(value)
            );

            break;

        case ControlRegister::SSR:
            asm volatile(
                "ldc %0, ssr"
                ::"r"(value)
            );

            break;

        case ControlRegister::SPC:
            asm volatile(
                "ldc %0, spc"
                ::"r"(value)
            );

            break;

        case ControlRegister::SGR:
            asm volatile(
                "ldc %0, sgr"
                ::"r"(value)
            );

            break;


        case ControlRegister::DBR:
            asm volatile(
                "ldc %0, dbr"
                ::"r"(value)
            );

            break;

        case ControlRegister::GBR:
            asm volatile(
                "ldc %0, gbr"
                ::"r"(value)
            );

            break;

        case ControlRegister::SR:
            asm volatile(
                "ldc %0, sr"
                ::"r"(value)
            );

            break;
    }
}

void ReadWriteControlRegisterMenu() {
    [[maybe_unused]] char addr[10];
    addr[0] = '>';
    addr[1] = '\0';
    unsigned int addrIndex = 1;
    int markerWidth = 320;
    int markerHeight = 20;
    int y0 = 8;


    struct Input_Event event;
    fillScreen(color(0, 0, 0));

    for (int y = 8; y < 8 + markerHeight; y++)
    {
        for (int x = 0; x < 0 + markerWidth; x++)
        {
            setPixel(x, y, color(128, 128, 128));
        }
    }

    while (selecting) {
        memset(&event, 0, sizeof(event));
        GetInput(&event, 0xFFFFFFFF, 0x10);

        uint32_t vbr = ReadControlRegister(ControlRegister::VBR);
        uint32_t ssr = ReadControlRegister(ControlRegister::SSR);
        uint32_t spc = ReadControlRegister(ControlRegister::SPC);
        uint32_t sgr = ReadControlRegister(ControlRegister::SGR);
        uint32_t dbr = ReadControlRegister(ControlRegister::DBR);
        uint32_t gbr = ReadControlRegister(ControlRegister::GBR);
        uint32_t sr = ReadControlRegister(ControlRegister::SR);

        switch (event.type) {
            case EVENT_KEY:
                if (event.data.key.direction == KEY_PRESSED) {
                    if (event.data.key.keyCode == KEYCODE_DOWN) {
                        if (y0 < 152) {

                            fillScreen(color(0, 0, 0));

                            y0 += 24;
                            for (int y = y0; y < y0 + markerHeight; y++)
                            {
                                for (int x = 0; x < 0 + markerWidth; x++)
                                {
                                    setPixel(x, y, color(128, 128, 128));
                                }
                            }

                            LCD_Refresh();
                        }

                        else {
                            fillScreen(color(0, 0, 0));
                            y0 = 8;

                            for (int y = 8; y < 8 + markerHeight; y++)
                            {
                                for (int x = 0; x < 0 + markerWidth; x++)
                                {
                                    setPixel(x, y, color(128, 128, 128));
                                }
                            }

                            LCD_Refresh();
                        }

                        break;
                    }

                    else if (event.data.key.keyCode == KEYCODE_UP) {
                        if (y0 > 8) {

                            fillScreen(color(0, 0, 0));

                            y0 -= 24;
                            for (int y = y0; y < y0 + markerHeight; y++)
                            {
                                for (int x = 0; x < 0 + markerWidth; x++)
                                {
                                    setPixel(x, y, color(128, 128, 128));
                                }
                            }

                            LCD_Refresh();
                        }

                        else {
                            fillScreen(color(0, 0, 0));
                            y0 = 152;

                            for (int y = y0; y < y0 + markerHeight; y++)
                            {
                                for (int x = 0; x < 0 + markerWidth; x++)
                                {
                                    setPixel(x, y, color(128, 128, 128));
                                }
                            }

                            LCD_Refresh();
                        }
                    }

                    else if (event.data.key.keyCode == KEYCODE_EXE) {
                        bool typing = true;
                        fillScreen(color(0, 0, 0));
                        LCD_Refresh();
                        Debug_SetCursorPosition(1, 20);
                        Debug_PrintString(addr, true);
                        LCD_Refresh();
                        

                        while (typing) {
                            memset(&event, 0, sizeof(event));
                            GetInput(&event, 0xFFFFFFFF, 0x10);

                            switch (event.type) {
                                case EVENT_KEY:
                                    if (event.data.key.direction == KEY_PRESSED) {
                                        uint32_t key = event.data.key.keyCode;
                                        char c;

                                        if (key == KEYCODE_BACKSPACE) {
                                            if (addrIndex > 1) {
                                                addr[--addrIndex] = '\0';
                                                Debug_SetCursorPosition(1, 20);
                                                fillScreen(color(0, 0, 0));
                                                Debug_PrintString(addr, true);
                                                LCD_Refresh();
                                            }
                                            continue;
                                        }

                                        else if (key == KEYCODE_EXE) {
                                            if (addrIndex == sizeof(addr) - 1) {
                                                typing = false;
                                                
                                                const char* hex = addr;
                                                hex++;

                                                uintptr_t newAddr = static_cast<uintptr_t>(
                                                    strtoull(hex, nullptr, 16)
                                                );
                                                
                                                memset(addr, 0, sizeof(addr));
                                                addrIndex = 1;
                                                addr[0] = '>';
                                                fillScreen(color(0, 0, 0));

                                                switch (y0) {
                                                    case 8:
                                                        WriteControlRegister(ControlRegister::VBR, newAddr);
                                                        break;

                                                    case 32:
                                                        WriteControlRegister(ControlRegister::SSR, newAddr);
                                                        break;
                                                    
                                                    case 56:
                                                        WriteControlRegister(ControlRegister::SPC, newAddr);
                                                        break;

                                                    case 80:
                                                        WriteControlRegister(ControlRegister::SGR, newAddr);
                                                        break;

                                                    case 104:
                                                        WriteControlRegister(ControlRegister::DBR, newAddr);
                                                        break;

                                                    case 128:
                                                        WriteControlRegister(ControlRegister::GBR, newAddr);
                                                        break;

                                                    case 152:
                                                        WriteControlRegister(ControlRegister::SR, newAddr);
                                                        break;
                                                }
                                            }
                                        }

                                        if (addrIndex >= sizeof(addr) - 1)
                                            continue;

                                        if      (key >= KEYCODE_0 && key <= KEYCODE_9)  c = '0' + (key - KEYCODE_0);
                                        else if (key == KEYCODE_EQUALS)                  c = 'A';
                                        else if (key == KEYCODE_X)                       c = 'B';
                                        else if (key == KEYCODE_Y)                       c = 'C';
                                        else if (key == KEYCODE_Z)                       c = 'D';
                                        else if (key == KEYCODE_POWER)                   c = 'E';
                                        else if (key == KEYCODE_DIVIDE)                  c = 'F';
                                        else                                            continue;

                                        addr[addrIndex++] = c;
                                        addr[addrIndex] = '\0';

                                        char tmp[2] = { c, '\0' };
                                        Debug_PrintString(tmp, true);
                                        LCD_Refresh();
                                    }

                                    break;

                                default:
                                    break;
                            }
                        }
                    }
                }
            
                break;

            case EVENT_TOUCH:
                if (event.data.touch_single.direction == TOUCH_UP) {
                    unsigned int x = (unsigned int) event.data.touch_single.p1_x;
                    unsigned int y = (unsigned int) event.data.touch_single.p1_y;

                    if (x <= 310 && x >= 10 && y <= 513 && y >= 483) {
                        selecting = false;
                        fillScreen(color(0, 0, 0));
                        Button(300, 30, 10, 15);    
                        Button(300, 30, 10, 483);        
                        Debug_Printf(16, 41, true, 0, "Go back to Hex Editor");
                        Debug_Printf(11, 2, true, 0, "Read/Write Control Registers");
                        LCD_Refresh();
                    }
                }

            default:
                break;
        }

        if (selecting) {
            Debug_Printf(1, 1, true, 0, "VBR: 0x%08lX", vbr);
            Debug_Printf(1, 3, true, 0, "SSR: 0x%08lX", ssr);
            Debug_Printf(1, 5, true, 0, "SPC: 0x%08lX", spc);
            Debug_Printf(1, 7, true, 0, "SGR: 0x%08lX", sgr);
            Debug_Printf(1, 9, true, 0, "DBR: 0x%08lX", dbr);
            Debug_Printf(1, 11, true, 0, "GBR: 0x%08lX", gbr);
            Debug_Printf(1, 13, true, 0, "SR: 0x%08lX", sr);

            Button(300, 30, 10, 483);

            Debug_Printf(12, 41, true, 0, "Go back to More Options Menu");

            LCD_Refresh();
        }   
    }
}

void MoreOptionsMenu() {
    bool inMenu = true;
    struct Input_Event event;

    fillScreen(color(0, 0, 0));
    Button(300, 30, 10, 15);    
    Button(300, 30, 10, 483);        
    Debug_Printf(16, 41, true, 0, "Go back to Hex Editor");
    Debug_Printf(11, 2, true, 0, "Read/Write Control Registers");
    LCD_Refresh();

    while (inMenu) {
        memset(&event, 0, sizeof(event));
        GetInput(&event, 0xFFFFFFFF, 0x10);

        switch (event.type) {
            case EVENT_TOUCH:
                if (event.data.touch_single.direction == TOUCH_UP) {
                    unsigned int x = (unsigned int) event.data.touch_single.p1_x;
                    unsigned int y = (unsigned int) event.data.touch_single.p1_y;

                    if (x <= 310 && x >= 10 && y <= 45 && y >= 15) {
                        selecting = true;
                        ReadWriteControlRegisterMenu();
                    }

                    else if (x <= 310 && x >= 10 && y <= 513 && y >= 483) {
                        inMenu = false;
                    }
                }

                break;

            default:
                break;
        }
    }
}