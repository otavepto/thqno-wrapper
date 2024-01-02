A replacement for the Steam wrapper used in Saints Row.  

### This will  
* Load the actual Steam dll from "wrapper\THQNOnline\steam\steam_api64.dll"
* Bypass the .dll integrity check

### How to use  
* Rename the `steam_api64.dll` (~8 MB) in the root folder to `thqno_api64.dll`
* Copy the replacement .dll and paste it beside the .exe, the debug build prints various events  

### How it works  
The games calls `fopen("steam_api64.dll")` 2 times to calculate some checksum for the file,  
which eventually translates to a call to `CreateFile(...)`.  

You'll see that this project hooks `NtCreateFile` instead, and this is done on purpose  
because on Windows 10 for example, `CreateFileA/W` is *mirrored* in `kernelbase.dll`  
which will (unintentionally) break some hooking techniques.  
On the other hand, NtCreateFile is always found in `ntdll.dll`.  
You can check the code, it's really simple.
