### Wallski
My own wallpaper engine made on c for wayland compositor

## [usage]
- so build this or u can use the binary in the project release to use it where u will get a daemon and a client 
- daemon is used to listen the commands from cilent and do stuff accordingly 
- run the ``wallski --set <wallpath copied from pwd>`` and if daemon is running along it will do its stuff and ye it is presistant 
[note it is dependent to shaders so copy both of the shader file to ``/usr/share/wallski/assets``]

### how to setup? [development]
- wayland protocols , wlroots for necessary headers
- explicitly generate the xdgshell, and wlr-layershellunstable headers from the wayland-scanner from the given xmls 
```
https://gitlab.freedesktop.org/wlroots/wlr-protocols/-/tree/master/unstable?ref_type=heads
/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
```
- put those generated headers in the include systemwide for usage
- check if our project stb_image.h matches with ![stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
- contribute with your stuff and generate pr
[while generating build create a build directory in root folder and see the CMakelists and generate binary accordingly for the cause]


### requirement
- using clang format
- sensible documenting along the code 
- cmake  version 4+
- make
- gcc

### aiming
- parallax wallpaper support 


### current transitions
1. fade

https://github.com/user-attachments/assets/9bd00b7a-6bfe-48d7-8a5c-be1c3f7dc480

2. shatter

https://github.com/user-attachments/assets/f597c350-6eca-410e-ab8a-113d4e79250a

3. wipe

https://github.com/user-attachments/assets/e36599a6-9787-4b1c-b5e1-6045d96f1006



### what it looks like?
https://github.com/user-attachments/assets/c55a0369-c8c2-435f-a8a6-0bfe313bb3e7


