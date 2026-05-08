import os
import urllib.request
import zipfile
import sys
import shutil

THIRD_PARTY_DIR = os.path.join(os.path.dirname(os.path.dirname(__file__)), "third_party")

REPOS = {
    "STM32F7xx_HAL_Driver": {
        "url": "https://github.com/STMicroelectronics/stm32f7xx_hal_driver/archive/refs/heads/master.zip",
        "zip_dir": "stm32f7xx-hal-driver-master",
    },
    "FreeRTOS": {
        "url": "https://github.com/FreeRTOS/FreeRTOS-Kernel/archive/refs/tags/V10.5.1.zip",
        "zip_dir": "FreeRTOS-Kernel-10.5.1",
    },
    "FatFs": {
        "url": "https://github.com/STMicroelectronics/stm32_mw_fatfs/archive/refs/heads/master.zip",
        "zip_dir": "stm32-mw-fatfs-master",
    },
    "CMSIS_Device": {
        "url": "https://github.com/STMicroelectronics/cmsis_device_f7/archive/refs/heads/master.zip",
        "zip_dir": "cmsis-device-f7-master",
    },
    "CMSIS_Core": {
        "url": "https://github.com/STMicroelectronics/cmsis_core/archive/refs/heads/master.zip",
        "zip_dir": "cmsis-core-master",
    },
    "CMSIS_FreeRTOS": {
        "url": "https://github.com/ARM-software/CMSIS-FreeRTOS/archive/refs/heads/main.zip",
        "zip_dir": "CMSIS-FreeRTOS-main",
    }
}

def download_progress(count, block_size, total_size):
    if total_size > 0:
        percent = int(count * block_size * 100 / total_size)
        sys.stdout.write(f"\rDownloading... {percent}%")
    else:
        sys.stdout.write(f"\rDownloading... {count * block_size / 1024 / 1024:.2f} MB")
    sys.stdout.flush()

def setup_repo(name, info):
    print(f"\n--- Setting up {name} ---")
    target_dir = os.path.join(THIRD_PARTY_DIR, name)
    
    if os.path.exists(target_dir) and os.listdir(target_dir):
        print(f"{name} already exists and is not empty. Skipping.")
        return

    zip_path = os.path.join(THIRD_PARTY_DIR, f"{name}.zip")
    
    # Download
    print(f"Downloading from {info['url']}")
    try:
        urllib.request.urlretrieve(info["url"], zip_path, reporthook=download_progress)
        print("\nDownload complete.")
    except Exception as e:
        print(f"\nFailed to download {name}: {e}")
        return

    # Extract
    print("Extracting...")
    try:
        with zipfile.ZipFile(zip_path, 'r') as zip_ref:
            zip_ref.extractall(THIRD_PARTY_DIR)
        print("Extraction complete.")
    except Exception as e:
        print(f"Failed to extract {name}: {e}")
        if os.path.exists(zip_path):
            os.remove(zip_path)
        return

    # Move contents from extracted dir to target dir
    extracted_dir = os.path.join(THIRD_PARTY_DIR, info["zip_dir"])
    if os.path.exists(extracted_dir):
        if not os.path.exists(target_dir):
            os.makedirs(target_dir)
        
        for item in os.listdir(extracted_dir):
            s = os.path.join(extracted_dir, item)
            d = os.path.join(target_dir, item)
            if os.path.exists(d):
                if os.path.isdir(d):
                    shutil.rmtree(d)
                else:
                    os.remove(d)
            shutil.move(s, d)
        
        shutil.rmtree(extracted_dir)
        print(f"{name} successfully installed in {target_dir}")
    
    # Cleanup zip
    if os.path.exists(zip_path):
        os.remove(zip_path)

def main():
    if not os.path.exists(THIRD_PARTY_DIR):
        os.makedirs(THIRD_PARTY_DIR)

    for name, info in REPOS.items():
        setup_repo(name, info)
        
    print("\nAll third-party libraries have been downloaded and extracted.")

if __name__ == "__main__":
    main()
