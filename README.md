## About

The PTVO Data Tag Link library is used to send data and commands to external or slave devices, host systems or visualizers. The library supports:

* Boolean values to represent On/Off statuses.
* Various data types (decimal or float numbers).
* Various data sizes (8, 16, 32, and 64 bits).

### Arduino IDE Installation

Install the library into the `libraries` folder of your Arduino installation or link from a ZIP archive (Sketch - Include library - Add .ZIP library).

### Visual Studio Code (VSCode) Installation

1. Install the Arduino CLI extension
1.1. Open the extensions manager (Ctrl + Shift + X).
1.2. Search for Arduino (by Microsoft).
1.3. Install and enable the extension.
2. Go to the Arduino user data folder (e.g., c:\Users\YourName\Documents\Arduino\libraries).
3. Create the PtvoDataTagLink folder.
4. Copy everything from the "src" folder in a ZIP archive to the local library folder.

### Visual Studio Code (VSCode) + PlatformIO Installation

1. Install the PlatformIO IDE extension
1.1. Open the extensions manager (Ctrl + Shift + X).
1.2. Search for PlatformIO IDE.
1.3. Install and enable the extension.

#### Install the library locally.
1. Create the "libraries" sub-folder in your project folder.
2. Create the PtvoDataTagLink folder (the library should be linked in every project separately).
3. Copy everything from the "src" folder in a ZIP archive to the local library folder.

#### Install the library globally.
1. Open PlatformIO CLI (Ctrl + Shift + P, search for "PlatformIO Core CLI").
2. In the console window:

```pio lib -g install path_to_zip_file```

### Library dependecies

None (uses built-in the "Serial" library).

### License Information

The hardware is released under the GNU Lesser General Public License (LGPL)

Distributed as-is; no warranty is given. 