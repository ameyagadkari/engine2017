/*
    This file provides an extremely basic interface for querying user input from the keyboard
*/

#ifndef EAE6320_USERINPUT_H
#define EAE6320_USERINPUT_H

// Include Files
//==============

#include <Engine/Windows/Includes.h>

#include <cstdint>

// Interface
//==========

namespace eae6320
{
    namespace UserInput
    {

        // Returns if the specified key is currently pressed

        // For standard letter or number keys, the representative ascii char can be used:
        // IsKeyPressed( 'A' ) or IsKeyPressed( '6' )

        // For special keys use one of the KeyCodes enumerations below

        bool IsKeyPressed(const uint_fast8_t i_keyCode);
        bool IsKeyPressedOnce(const uint_fast8_t i_keyCode);
        bool IsKeyPressedAndReleased(const uint_fast8_t i_keyCode);

        namespace KeyCodes
        {
            // These values are what the Windows-specific function expects, for simplicity
            enum eKeyCodes : uint_fast8_t
            {
                LEFT = 0x25,
                UP = 0x26,
                RIGHT = 0x27,
                DOWN = 0x28,

                SPACE = 0x20,

                ESCAPE = 0x1b,

                SHIFT = 0x10,
                CONTROL = 0x11,
                ALT = 0x12,

                TAB = 0x09,
                CAPS_LOCK = 0x14,

                BACK_SPACE = 0x08,
                ENTER = 0x0d,
                DELETE_BUTTON = 0x2e,

                PAGE_UP = 0x21,
                PAGE_DOWN = 0x22,
                END = 0x23,
                HOME = 0x24,

                F1 = 0x70,
                F2 = 0x71,
                F3 = 0x72,
                F4 = 0x73,
                F5 = 0x74,
                F6 = 0x75,
                F7 = 0x76,
                F8 = 0x77,
                F9 = 0x78,
                F10 = 0x79,
                F11 = 0x7a,
                F12 = 0x7b,

                A = 0x41,
                B = 0x42,
                C = 0x43,
                D = 0x44,
                E = 0x45,
                F = 0x46,
                G = 0x47,
                H = 0x48,
                I = 0x49,
                J = 0x4a,
                K = 0x4b,
                L = 0x4c,
                M = 0x4d,
                N = 0x4e,
                O = 0x4f,
                P = 0x50,
                Q = 0x51,
                R = 0x52,
                S = 0x53,
                T = 0x54,
                U = 0x55,
                V = 0x56,
                W = 0x57,
                X = 0x58,
                Y = 0x59,
                Z = 0x50,
            };
        }
#if defined( EAE6320_PLATFORM_WINDOWS )
#include "Windows/UserInput.win.h"
#endif
    }
}

#endif    // EAE6320_USERINPUT_H
