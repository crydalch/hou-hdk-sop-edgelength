# SOP Edge Length HDK implementation. #

![Example](/home/config/help/nodes/sop/edge-length-selection-0.gif)
![Example](/home/config/help/nodes/sop/edge-length-selection-2.gif)

Overview [video](https://www.youtube.com/watch?v=g-z_6GPlp3U&list=PLWInthQ-GtLhzoyqhaJAvzico8mkXMyDI&index=4) on YouTube.

**IMPORTANT:**
* Requires [hou-hdk-common](https://github.com/sebastianswann/hou-hdk-common) repository
* Read other requiments on mentioned above repository

**Currently supported platforms:**
- [x] Windows
- [ ] Apple
- [ ] Linux

**TODO:**
- [ ] add icons

## LEGEND (common for all plugins):
* [source](/source) folder contains `.h` and `.cpp` files required to compile DSO.
* [cmake](/cmake) folder contains additional modules used by `CMakeLists.txt`.
* `build` folder will be automatically created by CMake process and will contain Visual Studio project files.
* `home/dso` folder will be automatically created by compilation process and will contain compiled plugin.
* *(optional)* `3rdParty` folder contains additional `.h`, `.cpp`, `.dll` and `.lib` files, provided by 3rd parties.
* *(optional)* `home/otls` folder contains HDAs that accompany compiled DSO.
* *(optional)* `home/icons` folder contains icons that are used by compiled DSO and/or HDAs.
* *(optional)* `home/scripts` folder contains contains python and VEX scripts.