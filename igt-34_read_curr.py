""" This script is created as an EXAMPLE for specific purpose on Neousys IGT series.

   THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
   NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
   NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHORS SHALL NOT, UNDER ANY
   CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
   DAMAGES, FOR ANY REASON WHATSOEVER.

Usage: copy/download the igt_cffi.h file, modified from the original igt.h, to the folder where the .py file is.
       And the run it by python3 (tested with python3.5 shipped with the factory image)

Example command:
wget https://raw.githubusercontent.com/iiot-tw/igt-20-utils/master/igt_cffi.h 
python3 igt-34_read_curr.py
"""

from cffi import FFI
import time

# Create a CFFI interface
ffi = FFI()

# Include the C header file
with open('igt_cffi.h', 'r') as f:
    ffi.cdef(f.read())

# Load the shared library
lib = ffi.dlopen('/usr/lib/libigt.so')

# Define constants
AI_CH = 0
response_timeout = 10000

# Configure IGT AI
ret = lib.IGT_AI_Config(lib.IGT_AI_TYPE_CURRENT, \
                        lib.IGT_AI_CURRENT_STYLE_DEFAULT, \
#ret = lib.IGT_AI_Config(lib.IGT_AI_TYPE_VOLTAGE, \
#                        lib.IGT_AI_VOLTAGE_STYLE_DEFAULT, \
                        ffi.cast("unsigned int", response_timeout))

print("Config:", ret)

# Enable IGT AI
ret = lib.IGT_AI_Enable()
if ret != 0:
    print("Failed to enable:", ret)
    exit(ret)

icount = 0
while True:
    # Read IGT AI
    fAI0 = ffi.new("double *")
    ret = lib.IGT_AI_ReadlineF64(ffi.cast("unsigned int", AI_CH), fAI0)
    print("Read#{}: {:.5f}mA (ret={}) (#{:04d})".format(AI_CH, fAI0[0], ret, icount))
    icount += 1
    time.sleep(0.5)

# Disable IGT AI
ret = lib.IGT_AI_Disable()
print("Disable:", ret)
