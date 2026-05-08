import subprocess
import sys
import os

# Memory sizes for STM32F746ZGTx
FLASH_TOTAL = 1024 * 1024  # 1MB
RAM_TOTAL   = 320 * 1024   # 320KB

def format_size(bytes_val):
    """Formats bytes into a string like '112.896 KB'."""
    kb_val = bytes_val / 1024.0
    return f"{kb_val:,.3f}".replace(",", "X").replace(".", ",").replace("X", ".").replace(",000", "") + " KB"

def get_size(size_tool, elf_file):
    try:
        # Run the provided size tool
        result = subprocess.run([size_tool, elf_file], capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error running {size_tool}: {result.stderr}")
            return None
            
        lines = result.stdout.strip().split('\n')
        if len(lines) < 2:
            return None
        
        # Parse text, data, bss
        # Format: text	   data	    bss	    dec	    hex	filename
        values = lines[1].split()
        text = int(values[0])
        data = int(values[1])
        bss = int(values[2])
        return text, data, bss
    except Exception as e:
        print(f"Error: {e}")
        return None

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python print_size.py <size_tool_path> <elf_file>")
        sys.exit(1)
    
    size_tool = sys.argv[1]
    elf = sys.argv[2]
    
    if not os.path.exists(elf):
        print(f"Error: File {elf} not found.")
        sys.exit(1)
        
    sizes = get_size(size_tool, elf)
    if sizes:
        text, data, bss = sizes
        flash_used = text + data
        ram_used = data + bss
        
        flash_pct = (flash_used / FLASH_TOTAL) * 100
        ram_pct = (ram_used / RAM_TOTAL) * 100
        bss_pct = (bss / RAM_TOTAL) * 100
        
        print("\n" + "="*60)
        print(f"MEMORY USAGE REPORT: {os.path.basename(elf)}")
        print("-" * 60)
        print(f"FLASH: {format_size(flash_used):>12} / {format_size(FLASH_TOTAL):<12} ({flash_pct:6.2f}%)")
        print(f"RAM:   {format_size(ram_used):>12} / {format_size(RAM_TOTAL):<12} ({ram_pct:6.2f}%)")
        print(f"  - Data: {format_size(data)}")
        print(f"  - BSS:  {format_size(bss)} ({bss_pct:6.2f}% of total RAM)")
        print("="*60 + "\n")
    else:
        print("Failed to calculate memory usage.")


