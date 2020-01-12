#start st-util from another terminal before running gdb:
#arm-none-eabi-gdb -tui test.elf
tar extended-remote :4242
load
break main
