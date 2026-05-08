# run_ram.tcl
# OpenOCD script to load an ELF into RAM and start execution without a hard reset

init
reset init

# The ELF file path is passed via the ELF_FILE_FWD environment variable
echo "Loading image: $::env(ELF_FILE_FWD)"
load_image $::env(ELF_FILE_FWD)

# The vector table is linked at the start of RAM (0x20000000)
# The first 32-bit word is the initial Stack Pointer (_estack)
set sp [mrw 0x20000000]

# The second 32-bit word is the Reset_Handler address
set pc [mrw 0x20000004]

echo "Configuring registers: SP = $sp, PC = $pc"
reg sp $sp
reg pc $pc

echo "Resuming execution from RAM..."
resume

exit
