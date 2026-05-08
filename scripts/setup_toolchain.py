import os
import urllib.request
import zipfile
import sys

# Toolchain configuration (Arm GNU Toolchain 13.2.rel1 for Windows)
TOOLCHAIN_URL = "https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-mingw-w64-i686-arm-none-eabi.zip"
TOOLCHAIN_DIR = os.path.join(os.path.dirname(os.path.dirname(__file__)), "toolchain")
ZIP_PATH = os.path.join(TOOLCHAIN_DIR, "arm-gnu-toolchain.zip")
ORIGINAL_DIR_NAME = "arm-gnu-toolchain-13.2.rel1-mingw-w64-i686-arm-none-eabi"
TARGET_DIR_NAME = "arm-gnu-toolchain"

# OpenOCD configuration (xPack OpenOCD 0.12.0-3 for Windows)
OPENOCD_URL = "https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v0.12.0-3/xpack-openocd-0.12.0-3-win32-x64.zip"
OPENOCD_ZIP_PATH = os.path.join(TOOLCHAIN_DIR, "openocd.zip")
OPENOCD_ORIGINAL_DIR_NAME = "xpack-openocd-0.12.0-3"
OPENOCD_TARGET_DIR_NAME = "openocd"

# Ninja configuration (v1.12.1 for Windows)
NINJA_URL = "https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip"
NINJA_ZIP_PATH = os.path.join(TOOLCHAIN_DIR, "ninja.zip")
NINJA_TARGET_DIR_NAME = "ninja"

def download_progress(count, block_size, total_size):
    percent = int(count * block_size * 100 / total_size)
    sys.stdout.write(f"\rDownloading... {percent}%")
    sys.stdout.flush()

def download_and_extract(url, zip_path, original_dir, target_dir_name, name):
    extracted_path = os.path.join(TOOLCHAIN_DIR, target_dir_name)
    
    if os.path.exists(extracted_path):
        print(f"{name} already exists at {extracted_path}")
        return

    print(f"Downloading {name} from {url}...")
    try:
        urllib.request.urlretrieve(url, zip_path, reporthook=download_progress)
        print("\nDownload complete.")
    except Exception as e:
        print(f"\nFailed to download {name}: {e}")
        sys.exit(1)

    print(f"Extracting {name}...")
    try:
        if original_dir is None:
            # If no original directory (direct extraction into target)
            os.makedirs(extracted_path, exist_ok=True)
            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                zip_ref.extractall(extracted_path)
        else:
            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                zip_ref.extractall(TOOLCHAIN_DIR)
                
            original_path = os.path.join(TOOLCHAIN_DIR, original_dir)
            if os.path.exists(original_path):
                os.rename(original_path, extracted_path)
            
        print(f"Extraction of {name} complete.")
    except Exception as e:
        print(f"Failed to extract {name}: {e}")
        sys.exit(1)
        
    # Cleanup zip file
    if os.path.exists(zip_path):
        os.remove(zip_path)

    print(f"{name} successfully installed in: {extracted_path}")

def main():
    if not os.path.exists(TOOLCHAIN_DIR):
        os.makedirs(TOOLCHAIN_DIR)
        
    download_and_extract(TOOLCHAIN_URL, ZIP_PATH, ORIGINAL_DIR_NAME, TARGET_DIR_NAME, "Arm GNU Toolchain")
    download_and_extract(OPENOCD_URL, OPENOCD_ZIP_PATH, OPENOCD_ORIGINAL_DIR_NAME, OPENOCD_TARGET_DIR_NAME, "OpenOCD")
    download_and_extract(NINJA_URL, NINJA_ZIP_PATH, None, NINJA_TARGET_DIR_NAME, "Ninja")

    print("\nYou can now configure CMake with this toolchain and use the flash scripts.")

if __name__ == "__main__":
    main()
