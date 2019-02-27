---
rank: 2
layout: default
title: Brewing
permalink: /en/brewing/
lang: en
lang-ref: brewing
---

# Installation of the required software

We need:

- [PHP-CLI](http://php.net/manual/en/features.commandline.php) 7.x to execute the recording script that will receive the META screenshots,
- the PHP [GD](http://php.net/manual/en/book.image.php) module for image processing, which will allow us to convert the received binary raw data into PNG files,
- the [ImageMagick](https://www.imagemagick.org/) software suite to convert these PNG files to animated GIF.

Depending on your operating system, the installation procedure is different. Therefore, please refer directly to the sections that apply to you.


# Installation of PHP 7 and the GD module

## macOS Mojave

You're ~~right~~ lucky... you're on a Mac <i class="fab fa-apple"></i> and these software are pre-installed with macOS <i class="far fa-smile"></i> So you don't have to do anything special for this step! You can check this with the following commands by opening a Terminal:

```bash
$ php -v
PHP 7.1.23 (cli) (built: Nov  7 2018 18:20:35) ( NTS )
Copyright (c) 1997-2018 The PHP Group
Zend Engine v3.1.0, Copyright (c) 1998-2018 Zend Technologies

$ php -m | grep gd
gd  # <-- it means that the GD module is properly installed
```

You can go directly to the section that details the installation of ImageMagick.


## Debian Linux

Here I detail the installation procedure for PHP for the [Debian](https://www.debian.org/) distribution. If you have another distribution, you will need to adapt the following commands using the appropriate package manager.

Start by updating your database of available packages:

```bash
$ sudo apt update
```

Then, update your system:

```bash
$ sudo apt upgrade
```

Then check if `php-cli` and `php-gd` packages are not already installed:

```bash
$ dpkg -l | grep -iE 'php.*-(cli|gd)'
#
#                   depends on your architecture
#                 (in my case it's a Raspberry Pi)
#                                  |
#                                  v
ii  php7.0-cli  7.0.33-0+deb9u1  armhf  command-line interpreter for the PHP scripting language
ii  php7.0-gd   7.0.33-0+deb9u1  armhf  GD module for PHP
```

If one of these lines (preceded by `ii`) does not appear with you, it means that the corresponding package is not yet installed. In this case, proceed as follows for their installation:

```bash
# to install php-cli:
$ sudo apt install php-cli

# to install php-gd:
$ sudo apt install php-gd

# or to install both at the same time:
$ sudo apt install php-cli php-gd
```

Once the packages have been installed, check that everything went well:

```bash
$ php -v
PHP 7.0.33-0+deb9u1 (cli) (built: Dec  7 2018 11:36:49) ( NTS )
Copyright (c) 1997-2017 The PHP Group
Zend Engine v3.0.0, Copyright (c) 1998-2017 Zend Technologies
    with Zend OPcache v7.0.33-0+deb9u1, Copyright (c) 1999-2017, by Zend Technologies

$ php -m | grep gd
gd  # <-- it means that the GD module is properly installed
```


## Windows 10

<!-- procedure found on: http://kizu514.com/blog/install-php7-and-composer-on-windows-10/ -->

Download the latest *non-thread safe* version of PHP 7 at [https://windows.php.net](https://windows.php.net/download/)

![Downloading PHP 7 for Windows 10](../../assets/figures/windows-php7-download.png){: class="shadow"}

Then unarchive the ZIP content in `C:\PHP7`, and :

- duplicate the file `C:\PHP7\php.ini-development`
- by renaming it by `C:\PHP7\php.ini`

Open the new file `C:\PHP7\php.ini` in a text editor like [Notepad++](https://notepad-plus-plus.org/en/) and go to the ***Directory in which the loadable extensions (modules) reside*** section, then uncomment the `extension_dir = "ext"` line:

```
; Directory in which the loadable extensions (modules) reside.
; http://php.net/extension-dir
;extension_dir = "./"
; On windows:
extension_dir = "ext"
```

Go a little lower, to the ***Dynamic Extensions*** section and uncomment the extensions you want to activate (the one we are interested in here is the `gd2` extension):

```
;;;;;;;;;;;;;;;;;;;;;;
; Dynamic Extensions ;
;;;;;;;;;;;;;;;;;;;;;;

; ...

;extension=bz2
;extension=curl
;extension=fileinfo
; -----------------
; it's right here!
extension=gd2
; -----------------
;extension=gettext
;extension=gmp
;extension=intl
;extension=imap

; ...
```

Then **add** `C:\PHP7` to your `Path` environment variable.
Click on the image to enlarge it:

[![Configuring the Path variable][image]{: class="shadow"}][image]

[image]: ../../assets/figures/php7-windows-path.png

Finally, in the **CMD** shell, test that the installation went well:

```
Microsoft Windows [Version 10.0.17763.195]

noob@GAMER-PC C:\Users\noob
$ php -v  <-- to test the version of the PHP interpreter
$ php -m  <-- to display the list of activated extensions
```

In the list of activated extensions, you must find `gd2`.

# Installation of ImageMagick

<!-- j'utilise ici un caractère spécial invisible pour distinguer les titres de sections     -->
<!-- définis avec les mêmes chaînes de caractères, de façon à ce que la table des matières,  -->
<!-- qui est gérée par un code JavaScript pour automatiser le scolling sur la page, ne les   -->
<!-- confonde pas !                                                                          -->

## macOS Mojave &#0;

For the installation of ImageMagick, I suggest you use the excellent [Homebrew](https://brew.sh/index_fr) package manager, which you can easily install as follows:

```bash
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

The script explains what it will do, then pauses before executing it.

Once Homebrew is installed, you have the `brew` command to manage your packages. For more details on how to use it, you can enter the following command in a Terminal:

```bash
$ brew help
```

To install ImageMagick, simply run the command :

```bash
$ brew install imagemagick
```

Next, we will need the `convert` image conversion utility. You can check that it is now available by running the following command:

```bash
$ convert -version
Version: ImageMagick 7.0.8-28 Q16 x86_64 2019-02-19 https://imagemagick.org
Copyright: © 1999-2019 ImageMagick Studio LLC
License: https://imagemagick.org/script/license.php
Features: Cipher DPC HDRI Modules OpenMP
Delegates (built-in): bzlib freetype heic jng jp2 jpeg lcms ltdl lzma openexr png tiff webp xml zlib
```


## Debian Linux &#0;

For Linux, the procedure is pretty much the same, with its own package manager:

```bash
$ sudo apt install imagemagick
$ convert -version
```


## Windows 10 &#0;

For Windows, [download](https://www.imagemagick.org/script/download.php#windows) the latest precompiled version of ImageMagick. Once installed, you can also open the **CMD** shell and check that the `convert` tool is installed correctly:

```
Microsoft Windows [Version 10.0.17763.195]

noob@GAMER-PC C:\Users\noob
$ convert -version
```


## Everything is ready!

And here we are, we have installed all the tools necessary to execute the PHP recording script that will capture the successive META screens and make them PNG files, which we will then convert into animated GIF. So let's move on to integrating the `ScreenRecorder` into the C++ code of your sketch...