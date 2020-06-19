# Shader image processing
## Description
This program was created as a project for the discipline of "Program Technology-2".

It's simple 2d desktop editor that can change your images with a power of your videocard.
#  
The program was writen with C++ using GLEW and GLFW (to use OpenGL), GLM and STB. To use it, you need a videocard that suport at least OpenGL v3.3.
## Possibilities
So far you can choose on of thise options to change your photo:
- Saturation
- Gamma correction
- Brightness
- Contrast
- White balance (temperature)
- Hue
- Chromatic abberation (dispersion)
- Sharpness
- Diffenrent blurs
- And others
## Technical features
To make some changes to the photo, we are using shaders, all of them can be found in $/res/ dir.

Application works with jpg, png, bmp image formats, supports saving to file.  

## How to use?
1.Open Shader_image_processing_v2.exe, highly recommended to open it with your external videocard.
          
![](screenshots_new/screenshot1_1.PNG)
          
2.Type path to your image and type where you whant to save a result (pathes can be absolute and relative).
          
![](screenshots_new/screenshot1_2.PNG)
          
3.Navigate through console using your keyboard, to confirm changes type Enter, to save your file type S.

## Troubleshooting
1. If some of blures doesn't work, you probably opened program with integrated videocard, try to choose external. (Problem was spoted on Radeon Vega 8).

2. If navigation keys doesn't work, choose your image as an active window.

3. If navigation window become crazy, just type some of navigation keys.

## Screenshots
![](screenshots_new/screenshot1_3.PNG)
![](screenshots_new/screenshot1_4.PNG)
![](screenshots_new/screenshot1_5.PNG)
![](screenshots_new/screenshot1_6.PNG)
![](screenshots_new/screenshot1_7.PNG)
![](screenshots_new/screenshot1_8.PNG)
![](screenshots_new/screenshot1_9.PNG)
      
Posible result:

![](screenshots_new/screenshot1_10.PNG)
