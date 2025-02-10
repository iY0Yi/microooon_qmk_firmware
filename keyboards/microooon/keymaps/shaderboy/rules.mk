MCU = atmega32u4
BOOTLOADER = caterina

POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom
SRC += drivers/sensors/blackberry_trackball.c
MOUSEKEY_ENABLE = yes

CUSTOM_MATRIX = lite
SRC += matrix.c
