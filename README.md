# Wireless Temperature Logging with RPi Pico W
## Introduction
This branch called "vfs" is the same project as the main branch, but it uses a different approach to the way files are stored. The FAT filesystem implementation by [carlk3](https://github.com/carlk3) is substituted by [this](https://github.com/oyama/pico-vfs) virtual file system designed specifically for the RPi Pico. This filesystem supports both FAT and littlefs filesystems, both on an SD card and on the on-board 2MB flash memory the Pico provides. AS a way to try out both filesystems, I decided for the Access Point to save just the last value in its flash memory (littlefs), and the Station to keep logging the temperatures it receives in an SD card (FAT).

## Components Used
Since only one SD card was used now, the components are a few less compared to the main branch.
* 2 Raspberry Pi Pico W Microcontrollers
* 2 Breadboards
* 1 Push button
* 1 Waveshare Micro SD Storage Boards
* 1 Transcend 300S MicroSD 4GB Cards
* 1 Adafruit MCP9808 Temperature Sensor
* Jumper Wires

## Implementation
Since implementation is really similar to the main branch, I will just highlight the key differences.
* As mentioned in the intro, the Access Point now logs temperature values in the 2MB flash memory, utilizing a littlefs filesystem. More on that filesystem can be found [here](https://github.com/littlefs-project/littlefs).
* ```filesystem.c``` is repalced by ```fs_utils.C``` as to not cause confusion, since vfs also uses the same name for a folder.
* ```fs_init()``` is shared bewtween the two microcontrollers, but they don't initialize the same filesystems! This is done using ```#ifdef``` directives that check whether ```USE_SD_FAT``` or ```USE_FLASH_LFS``` preprocessor macros are defined. These macros are defined seperately for the access point and the station inside the ```CMakeLists.txt``` file. 
  
## Images
Below I attach some images, demonstrating the setup used for this implementation. It's easy to see that these diagrams are simpler than the original case, since less components are used.
<figure>
    <img src="images/access_point.jpg" width="600" height="500">
    <figcaption> Access Point Layout </figcaption>
</figure>

| MCP9808 Temperature Sensor |  Pin on Pico (Access Point) |
| :------------------------: | :-------------------------: |
| Vcc | VSYS (Physical Pin 39) |
| GND | Physical Pin 38 (or any GND pin) |
| SCL | I2C0 SCL (Physical Pin 7) |
| SDA | I2C0 SDA (Physical Pin 6) |
<figure>
    <img src="images/station.jpg" width="600" height="500">
    <figcaption> Station Layout </figcaption>
</figure>

| 2 Pin Button (Send Message) | SD Card Breakout Board |  Pin on Pico (Station) |
| :------------------------: | :--------------------: | :-------------------------: |
| First Pin | - | GPIO Pin 15 (Physical Pin 20) |
| Second Pin | - | Physical Pin 8 (or any GND pin) |
| - | 3.3V | 3.3V(OUT) |
| - | GND | Physical Pin 38 (or any GND pin) |
| - | MISO(DO) | SPI0 RX (Physical Pin 21) |
| - | MOSI(DI) | SPI0 RT (Physical Pin 25) |
| - | SCLK | SPI0 SCK (Physical Pin 24) |
| - | CS | SPI0 CSn (Physical Pin 22) |

## How to Build
If you are interested in trying out the project for yourself you can clone 
this repo and then build the binaries. To do so open up a terminal and 
navigate to where you want the project folder to be created using the ```cd``` 
command. After that, type this command to clone the repo:
```
git clone --recurse-submodules https://github.com/PanagiotisKarath/PicoW-WirelessTempLogger
```
To build the new project using the vfs filesystem, you have to first "checkout"
the vfs branch by typing:
```git checkout vfs```.

After that, ```cd``` inside the folder and type these commands to create a 
"build" folder, go in it and build the binaries. In this build folder is where 
you'll find the binaries that can be flashed onto the microcontrollers.
```
mkdir build
cd build
cmake -DPICO_BOARD=pico_w ..
make
```
Make sure to include the ```-DPICO-BOARD=pico_w``` to specify that the wireless
 version of the Pico is used. One last note, you can also type ```make -j2``` 
 to split the build process into threads, 2 in this example but you can split 
 it into more, and speed up the process.

After the process is done, you can find the ```.uf2``` and ```.elf``` files in 
the build folder, which you then flash on the Pico microcontrollers. See 
"Binaries Folder" for more information on how to flash the binaries.

## Binaries Folder
In case you are not interested in building the project, both the .uf2 and .elf 
files are included in the "bin" folder, where they can be simply loaded on 2 
Pico W boards. To learn about how to flash binaries on the 
Picos see [here](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html#your-first-binaries). 
If you own a Debug Probe, you want to flash the ```.elf``` files. See more 
about how this is done [here](https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html).

## Other repositories used
Besides the repositories mentioned in the main branch, I have to give credit to
[oyama](https://github.com/oyama) for creating the pico-vfs project. You can 
check the vfs project [here](https://github.com/oyama/pico-vfs).