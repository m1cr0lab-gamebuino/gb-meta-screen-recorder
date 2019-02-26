# Gamebuino META Screen Recorder

This tool will allow you to record your Gamebuino-META screen while running an application. Recording can be done in standard (`80x64`) or high (`160x128`) resolution. The display data on the screen is sent to the serial port via a USB cable. This data is recovered by a PHP script that rebuilds the frames one by one and saves them in PNG files. You can then generate an animated GIF with ImageMagick from the saved PNG files.

Check out the [documentation](http://iw4rr10r.github.io/gb-meta-screen-recorder) for this tool to learn more!