#pragma once

// Enumerate of requests from PC
typedef enum
{
    REQUEST_SERIAL_NUMBER,
    REQUEST_LINE_STATUS,
    REQUEST_KB_PRESS
} RequestsTypedef;

void dut_init(void);