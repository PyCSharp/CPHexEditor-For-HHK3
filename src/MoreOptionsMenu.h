#pragma once
#include <sdk/os/debug.h>
#include <sdk/os/input.h>
#include <sdk/os/lcd.h>
#include <sdk/calc/calc.h>
#include <sdk/os/mcs.h>
#include <cstdlib>
#include <cstring>

extern bool selecting;
extern bool isDarkmode;

enum class ControlRegister {
    VBR,
    SSR,
    SPC,
    SGR,
    DBR,
    GBR,
    SR,
};

void MoreOptionsMenu();
void ReadWriteControlRegisterMenu();
void CheckForDarkmode();
void SetDarkmodePersistent();
void SetLightmodePersistent();
void Button(unsigned int width, unsigned int height, unsigned x, unsigned y);
