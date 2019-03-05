#!/usr/bin/env php
<?php

/**
 * +---------------------------------------+
 * |    Gamebuino META Screen Recorder     | 
 * | © 2019 Stéphane Calderoni (aka Steph) |
 * |     https://gamebuino.com/@steph      |
 * +---------------------------------------+
 * 
 * This tool captures the screen frames rendered on the Gamebuino META
 * by retrieving data from the serial port. Capture can be done in
 * standard resolution or high resolution.
 * 
 * To run this script, you need to install the C++ files `ScreenRecorder.h`
 * and `ScreenRecorder.cpp` in the folder of your sketch. Then, insert the
 * following line of code in your `setup()` function:
 * 
 * ----------------------------------------------------------------------------------
 * // if rendering is performed in standard resolution
 * ScreenRecorder::init();
 * ScreenRecorder::setForWindows(); // if your are on Windows
 * 
 * // if rendering is performed in high resolution (by cutting the screen into slices)
 * ScreenRecorder::init(RENDERING_SLICE_HEIGHT);
 * ScreenRecorder::setForWindows(); // if your are on Windows
 * ----------------------------------------------------------------------------------
 * 
 * Then insert the following line in the appropriate place in your code,
 * when the screen has been rendered in its entirety:
 * 
 * ----------------------------------------------------------------------------------
 * // for standard resolution, insert this line at the end of your `loop()` function:
 * ScreenRecorder::monitor(gb.display._buffer);
 * 
 * // for high resolution, insert this line when your slice buffer has been populated
 * // (note that you must specify the rank of the slice):
 * ScreenRecorder::monitor(buffer, sliceIndex);
 * ----------------------------------------------------------------------------------
 * 
 * Then, compile your sketch and upload it to your META. Run this script to listen to
 * the serial port. For example:
 * 
 * ----------------------------------------------------------------------------------
 * $ screenrecord -d frames
 * ----------------------------------------------------------------------------------
 * 
 * You can get details of options recognized by the script by running this command:
 * 
 * ----------------------------------------------------------------------------------
 * $ screenrecord -h
 * ----------------------------------------------------------------------------------
 * 
 * Once the script is running, press and hold (at least 1 second) META's BUTTON_MENU
 * to start recording. The META will emit red flashes throughout the recording period.
 * Each frame is rebuilt and saved as a PNG file in the folder specified by the `-d`
 * flag of the `screenrecord` command (the `frames` folder here). To stop recording,
 * briefly press BUTTON_MENU again and the script will stop itself.
 *
 * You can also start and stop recording at specific times directly in your C++ code.
 * To do this, use the following instructions:
 * ----------------------------------------------------------------------------------
 * ScreenRecorder::startRecording();
 * ScreenRecorder::stopRecording();
 * ----------------------------------------------------------------------------------
 * 
 * To create an animated GIF, you must have installed the `imagemagick` software
 * beforehand and use its `convert` program. For example, to create an animation
 * of 160x128 pixels at 25 fps, run the following command:
 * 
 * ----------------------------------------------------------------------------------
 * $ convert -delay 4 -loop 0 frames/*.png -scale 160x128 screenrecording.gif
 * ----------------------------------------------------------------------------------
 * 
 * You will then get the file `screenrecording.gif`.
 * 
 * That's All Folks!
 */

/**
 * Device (calling unit) corresponding to the USB port
 * used by your META to communicate with your computer.
 */
define('SERIAL_DEVICE_ID', '/dev/cu.usbmodem141401');

/**
 * Root name of the PNG files that will be created.
 */
define('SAVEFILE_BASENAME', 'frame');

/**
 * These files will be numbered (starting from 1).
 * You must specify here how many digits you want to format the numbering.
 */
define('NUM_DIGITS_SAVEFILE', 4);

/**
 * This tool is dedicated to recording the Gamebuino META screen.
 * We can therefore set the dimensions of the screen here.
 * We set values corresponding to the standard definition,
 * the script will detect on its own that META is in high definition
 * and will adapt these values automatically.
 */
define('SCREEN_WIDTH',  80);
define('SCREEN_HEIGHT', 64);

/**
 * Definition of the class in charge of recording the screen.
 */
class ScreenRecorder
{
    /** @var resource Resource representing the serial port reading pointer. */
    private $serial;
    /** @var string Identifier of to the serial port. */
    private $serial_id;
    /** @var string Path of the directory in which the PNG files will be saved. */
    private $directory;
    /** @var string Base name of the PNG files that will be saved. */
    private $basename;
    /** @var int Number of digits required to format the numbering of PNG files. */
    private $digits;
    /** @var bool TRUE if high resolution has been detected, FALSE otherwise */
    private $hd;
    /** @var int Screen width in pixels */
    private $screenwidth;
    /** @var int Screen height in pixels */
    private $screenheight;
    /** @var int Slices height in pixels */
    private $sliceheight;
    /** @var int Number of slices */
    private $numslices;
    /** @var bool TRUE when recording has started, FALSE otherwise */
    private $recording;
    /** @var bool TRUE while listening to serial port, FALSE otherwise */
    private $listening;

    /**
     * Constructor
     * 
     * @param  array $args list of arguments catched on command line
     * @return ScreenRecorder instance
     */
    public function __construct(array &$args) {
        // sets the default properties
        $this->serial_id    = SERIAL_DEVICE_ID;
        $this->directory    = '.';
        $this->basename     = SAVEFILE_BASENAME;
        $this->digits       = NUM_DIGITS_SAVEFILE;
        $this->hd           = false;
        $this->screenwidth  = SCREEN_WIDTH;
        $this->screenheight = SCREEN_HEIGHT;
        $this->sliceheight  = SCREEN_HEIGHT; // default value for standard resolution
        $this->numslices    = 1;
        $this->recording    = false;
        $this->listening    = false;
        
        // analyzes arguments passed on the command line
        $this->getParams($args);
        // listen to the serial device
        $this->startListening();
    }

    /**
     * Analyzes arguments passed on the command line.
     * 
     * @param  array $args list of arguments catched on command line
     * @return void
     */
    private function getParams(array &$args) {
        // retrieves the program name
        $command = basename(array_shift($args));
        // counts the number of arguments
        $n = count($args);
        // goes through the arguments one by one
        for ($i=0; $i<$n; $i+=2) {
            switch ($args[$i]) {
                case '-h': // help
                    $this->help($command);
                    die();
                    break;
                case '-p': // serial port
                    $serial = $args[$i+1];
                    if (!file_exists($serial)) {
                        die(sprintf("[Error] no such device: %s\n", $this->highlight($serial)));
                    }
                    $this->serial_id = $serial;
                    break;
                case '-d': // recording directory
                    $path = $args[$i+1];
                    if (!is_writable($path)) {
                        die(sprintf("[Error] no such directory: %s\n        or it could be not writable...\n", $this->highlight($path)));
                    }
                    $this->directory = $path;
                    break;
                case '-b': // PNG files basename
                    $name = $args[$i+1];
                    if (!preg_match('/\w+/', $name)) {
                        die(sprintf("[Error] wrong basename: %s\n", $this->highlight($name)));
                    }
                    $this->basename = $name;
                    break;
                case '-n': // number of digits to number the PNG files
                    $n = $args[$i+1];
                    $this->digits = max(1, min(5, $n));
                    break;
            }
        }
    }

    /**
     * Displays the command help.
     * 
     * @param  string $command the name of the command
     * @return void
     */
    private function help(string $command) {
        $buffer  = $this->highlight("\n+---------------------------------------+\n");
        $buffer .= $this->highlight("|    Gamebuino META Screen Recorder     |\n");
        $buffer .= $this->highlight("| © 2019 Stéphane Calderoni (aka Steph) |\n");
        $buffer .= $this->highlight("|     https://gamebuino.com/@steph      |\n");
        $buffer .= $this->highlight("+---------------------------------------+\n\n");
        $buffer .= sprintf("Usage: %s [options]\n\n", $this->highlight($command));
        $buffer .= "DESCRIPTION\n\n";
        $buffer .= "\tThis utility records the Gamebuino META screen in Standard & High Definition\n";
        $buffer .= "\tusing communication through a serial port\n\n";
        $buffer .= "OPTIONS\n\n";
        $buffer .= sprintf("\t%s  displays the synopsis\n", $this->highlight('-h'));
        $buffer .= sprintf("\t%s  sets the serial port (default: %s)\n", $this->highlight('-p'), SERIAL_DEVICE_ID);
        $buffer .= sprintf("\t%s  sets the output directory (default: current directory)\n", $this->highlight('-d'));
        $buffer .= sprintf("\t%s  sets the image files basename (default: %s)\n", $this->highlight('-b'), SAVEFILE_BASENAME);
        $buffer .= sprintf("\t%s  sets the number of digits to number the image files (default: %s)\n\n", $this->highlight('-n'), NUM_DIGITS_SAVEFILE);
        $buffer .= "ANIMATED GIF CREATION\n\n";
        $buffer .= sprintf("\tYou must have %s installed to be able to create the animated GIF file.\n", $this->highlight('imagemagick'));
        $buffer .= "\tFor example, to create an animation with a resolution of 160x128 pixels at 25 fps,\n";
        $buffer .= "\tsimply type the following command:\n\n";
        $buffer .= $this->highlight("\tconvert -delay 4 -loop 0 png_dir_path/*.png -scale 160x128 screenrecording.gif\n\n");
        echo $buffer;
    }

    /**
     * Highlights a string of characters for display in the terminal.
     * 
     * @param  string $str
     * @return string the input string formatted for display in the terminal
     */
    private function highlight(string $str) : string
    {
        return "\e[1m${str}\e[0m";
    }

    /**
     * Starts the serial device listening.
     * 
     * @return void
     */
    private function startListening() {
        // reading binary data from the serial port
        if ($this->serial = @fopen($this->serial_id, "rb")) {

            // initializes the index of the frame to be recorded
            $n = 1;

            // starts listening and waits for the beginning of the recording
            $this->listening = true;
            printf("\nStart listening to the serial port %s\n\nWaiting for data... ", $this->highlight($this->serial_id));
            
            // waits for the start recording tag
            $this->waitingForStartingTag();
            
            // displays some details about recording
            printf(
                "Start screen recording in %s resolution%s\n\n",
                $this->highlight($this->hd ? 'high' : 'standard'),
                !$this->hd ? sprintf(' (slice height: %d pixels)', $this->sliceheight) : ''
            );
            
            // decodes the catched frame during recording
            while ($this->recording) {
                $this->decodeScreenShot($n++);
            }

            // recording has been stopped at this stage
            printf("\n%s PNG files have been recorded in %sx%s (%s resolution).\n\n", $n - 2, $this->screenwidth, $this->screenheight, $this->hd ? 'high' : 'standard');
            
            // display how to create an animated GIF from PNG files
            $buffer  = sprintf("You must have %s installed to convert these PNG files into an animated GIF file.\n", $this->highlight('imagemagick'));
            $buffer .= "For a 25 fps animation, simply type the following command:\n\n";
            $buffer .= $this->highlight(sprintf("convert -delay 4 -loop 0 %s/*.png -scale %sx%s screenrecording.gif\n\n", $this->directory, $this->screenwidth, $this->screenheight));
            echo $buffer;

        } else {
            die(sprintf("[Error] cannot open the serial port: %s\n", $this->highlight($this->serial_id)));
        }
    }

    /**
     * This method listens to the serial port while waiting for
     * the `start` tag to begin recording.
     * The capture resolution will be determined by the data following the tag.
     * 
     * @return void
     */
    private function waitingForStartingTag() {
        $started = false;
        while ($this->listening && !$started) {
            $data = unpack('A5tag/Cwidth/Cheight/Csliceheight', fread($this->serial, 8));
            $started = $data['tag'] == 'start';
            if ($started) {
                $this->recording    = true;
                $this->screenwidth  = $data['width'];
                $this->screenheight = $data['height'];
                $this->sliceheight  = $data['sliceheight'];
                $this->numslices    = $this->screenheight / $this->sliceheight;
                $this->hd           = ($this->screenwidth == 2 * SCREEN_WIDTH);
            }
        }
    }

    /**
     * Starts decoding the binary data to rebuild the displayed frame.
     * 
     * @param  int $n the index of the frame to be recorded
     * @return void
     */
    private function decodeScreenShot(int $n) {
        // creates an empty (black) image at the size of the META screen
        $gd = @imagecreatetruecolor($this->screenwidth, $this->screenheight)
              or die("\n\n[Error] Unable to create a GD image stream.\n\n");

        // is about to read all the slices sent on the serial port
        for ($i=0; $i<$this->numslices; $i++) {
            if ($this->recording) {
                // we may search for the `stop` tag on the first 4 bytes
                $data = unpack('n2', fread($this->serial, 4));
                $tag  = $this->fourBytesToString($data);
                if ($tag == 'stop') {
                    $this->recording = false;
                } else {
                    if ($i == 0) print(sprintf("Decoding frame %'0".$this->digits."d...", $n));
                    // decodes all remaining 16-bit integers of the current slice
                    // remember that 4 bytes have already been taken from the data stream
                    $slice = unpack('n*', fread($this->serial, 2 * $this->screenwidth * $this->sliceheight - 4));
                    
                    // the 4 bytes previously removed are inserted again to rebuild the complete slice
                    // !!! BE CAREFUL !!!
                    // the `unpack` function returns an array whose numerical indexes start at the value 1
                    array_unshift($slice, $data[1], $data[2]);

                    // decodes the data that make up the current slice
                    $this->decodeSlice($slice);
                    // and copy them onto the current image
                    $this->writeSliceOnImage($i, $slice, $gd);
                }
            }
        }
        
        // once all the slices have been scanned, saves the recomposed image in a file in PNG format
        if ($this->recording) {
            @imagepng($gd, sprintf($this->directory.DIRECTORY_SEPARATOR.$this->basename."-%'0".$this->digits."d.png", $n))
            or die("\n\n[Error] Unable to create the PNG file to save the captured frame.\n\n");
            print(" saved\n");
        }
    }

    /**
     * Decodes a 4-character string from two 16-bit integers
     * decomposed in four bytes.
     * 
     * @param  array the 4 bytes to decode
     * @return string the 4-character string
     */
    private function fourBytesToString(array $bytes) : string {
        array_walk($bytes, function (&$e) {
            $e = chr($e >> 8) . chr($e & 0xff);
        });
        return join('', $bytes);
    }

    /**
     * This method decodes the binary data of the current slice
     * and returns a table containing the RGB888 color codes
     * of the corresponding pixels.
     * 
     * The binary data read are 16-bit integers representing
     * the RGB565 color codes of the pixels on the META screen.
     * 
     * In high resolution, endianness is Big Endian.
     * In standard resolution, endianness is Little Endian.
     * 
     * @param  array $data the reference to the slice buffer
     * @return void
     */
    private function decodeSlice(array &$data) {
        array_walk($data, function (&$c) {
            $c = $this->rgb565torgb888($c);
        });
    }
    
    /**
     * Transforms a RGB565 color code into RGB888.
     * 
     * In high resolution, endianness is Big Endian.
     * In standard resolution, endianness is Little Endian.
     * 
     * @param  int $color an RGB565 colour code
     * @return int the corresponding RGB888 colour code
     */
    private function rgb565torgb888(int $color) : int {
        if (!$this->hd) $color = ($color << 8) | ($color >> 8);
        return (($color & 0xf800) >> 8) << 16 | (($color & 0x7e0) >> 3) << 8 | ($color & 0x1f) << 3;
    }

    /**
     * This method rebuilds the image part corresponding to the current slice.
     * 
     * @param  int      $i     the index of the current slice
     * @param  array    $slice the reference to the RGB888 color code table of the current slice
     * @param  resource $gd    the image corresponding to the current frame
     * @return void
     */
    private function writeSliceOnImage(int $i, array &$slice, $gd) {
        // calculates the ordinate of the pixels
        // of the upper fringe of the current slice
        $yOffset = $i * $this->sliceheight;
        // scans all pixels of the current slice
        for ($y = 0; $y < $this->sliceheight; $y++) {
            for ($x = 0; $x < $this->screenwidth; $x++) {
                // retrieves the RGB888 color code of the pixel located at the coordinates (x,y)
                $c = $slice[$y * $this->screenwidth + $x];
                // and if this code does not correspond to black...
                if ($c) {
                    // separates the red, green and blue signals from the RGB888 code
                    // and creates the corresponding displayable color
                    $color = @imagecolorallocate($gd, ($c & 0xff0000) >> 16, ($c & 0xff00) >> 8, $c & 0xff)
                             or die("\n\n[Error] Unable to allocate a color for the current image.\n\n");;
                    // sets the color of the pixel at the (x,y + yOffset) coordinates on the image
                    @imagesetpixel($gd, $x, $y + $yOffset, $color)
                    or die("\n\n[Error] Impossible to draw a pixel on the current image.\n\n");
                }
            }
        }
    }
}

/**
 * Runs the command (if GD lib is available).
 */
if (function_exists('imagecreatetruecolor')) {
    new ScreenRecorder($argv);
} else {
    die ("[Error] The GD module for PHP must be installed to be able to execute this script.\n");
}
