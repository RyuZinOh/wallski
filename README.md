### Wallski
My own wallpaper engine made on c for wayland compositor

### how to setup?
- wayland protocols , wlroots for necessary headers
- explicitly generate the xdgshell, and wlr-layershellunstable headers from the wayland-scanner from the given xmls 
```
https://gitlab.freedesktop.org/wlroots/wlr-protocols/-/tree/master/unstable?ref_type=heads
/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
```
- put those generated headers in the include systemwide for usage
- check if our project stb_image.h matches with ![stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
- contribute with your stuff and generate pr
[while generaing build create a build directory in root folder and see the CMakelists and generate binary accordingly for the cause]


### requirement
- using clang format
- sensible documenting along the code 
- cmake  version 4+
- make
- gcc

### aiming
- parallax wallpaper support 
