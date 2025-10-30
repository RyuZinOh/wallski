### Wallski
My own wallpaper engine made on c for wayland compositor

https://github.com/user-attachments/assets/5c48b51e-9046-4619-9c74-6152fe0165d9






## [usage]
- so build this or u can use the binary in the project release to use it where u will get a daemon and a client 
- daemon is used to listen the commands from cilent and do stuff accordingly 
- run the ``wallski --set <wallpath copied from pwd> --transition <transition_options>`` and if daemon is running along it will do its stuff and ye it is presistant 
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

https://github.com/user-attachments/assets/0906f243-60d6-47a8-9664-82fafb728984

4. wipe

https://github.com/user-attachments/assets/e36599a6-9787-4b1c-b5e1-6045d96f1006

4. ripple

https://github.com/user-attachments/assets/157ee903-b3b6-4785-ac59-93bc1fdc8a96

5.zoom

https://github.com/user-attachments/assets/dbc5f860-b9bc-4a5d-aa00-89e408d22848
