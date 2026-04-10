## README_en_AlzV5_ESP32S3


# Build Instructions

## One-click Build

```bash
python scripts/release.py AlzV5_ESP32S3 -c config_en.json
```

## Manual Configuration and Build

```bash
idf.py set-target esp32s3
```

**Configuration**

```bash
idf.py menuconfig
```

Select the board:

```
Xiaozhi Assistant -> Board Type -> AlzV5_ESP32S3
```

## Build and Flash

```bash
idf.py -DBOARD_NAME=AlzV5_ESP32S3 build flash
```

Note: If your device was previously shipped with the AlzV5_ESP32S3 firmware ( Xiaozhi base version), please be very careful with the flash partition addresses to avoid accidentally erasing the device information (such as EUI) of the AlzV5_ESP32S3. Otherwise, even if you restore the AlzV5_ESP32S3 firmware, the device may not be able to connect to the AiVera server correctly! Therefore, before flashing the firmware, be sure to record the necessary device information to ensure you have a way to recover it!

You can use the following command to back up the factory information:

```bash
# Firstly backup the factory information partition which contains the credentials for connecting the AiVera server
esptool.py --chip esp32s3 --baud 2000000 --before default_reset --after hard_reset --no-stub read_flash 0x9000 16384 nvsfactory.bin
```
