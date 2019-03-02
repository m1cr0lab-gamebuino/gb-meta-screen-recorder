---
rank: 1
layout: default
title: Overview
lang: en
lang-ref: index
---

# Why a new screen recorder?

Are you developing a new game? Are you proud of your new creation and want to demonstrate it without further delay on [Discord](https://discordapp.com/) or on the [gamebuino.com](https://gamebuino.com/creations/) website? You will probably want to do an animated GIF to show your masterpiece to the whole community <i class="far fa-smile"></i>

The Gamebuino META has two built-in tools that allow you to take screenshots and screen recordings. These tools are directly accessible via the **Home** <i class="fa fa-home"></i> button. What interests us here is the **video capture**:

![Native screen recording](../assets/figures/native-screen-recording-500x336.jpg){: width="250" height="168" class="shadow"}

The tool provided with the META is very useful, certainly... nevertheless it is quite **limited** and above all **extremely binding**!

- Limited because it **does not** allow to record a screen capture of an application developed for **high resolution** (`160x128` pixels). Only the standard resolution (`80x64` pixels) is supported. In addition, you cannot start or stop recording directly in your code at a specific time, or when certain events occur: you must necessarily interrupt the progress of your application and go through the menu accessible with the Home button.

- Binding because the capture tool saves a file in **GMV** format... which is very lightweight, but not readable in a web browser. So you can't do anything with it as it is. [Soru](https://gamebuino.com/@sorunome) has developed a [small utility](https://gamebuino.com/creations/gmv-to-gif-converter) that allows you to convert this GMV file into an animated GIF. But you will have to get the GMV file from the microSD card before you can convert it... For those who have already done the exercise, you will admit that these manipulations can drive you crazy when you do it again 10 times to get a recording that suits you!

This naturally led me to develop a small utility that meets my needs:

- **Simpler** and **faster** to use.
- **More flexible** in that I can now trigger or stop recording in multiple ways: either by pressing a button, or directly in my code at a specific time, or when certain events occur. In addition, once the recording has been done, I can still rework the screenshots, or even delete unnecessary screens, before generating my animated GIF.
- More **advanced** since I can now access high resolution.


# What are the implementation requirements?

|                    |           Beginner           |           Intermediary             | Advanced | Expert |
|-------------------:|:----------------------------:|:----------------------------------:|:--------:|:------:|
| **Required level** | <i class="fas fa-check"></i> | <i class="fas fa-check faded"></i> |          |        |

To be able to install and use this tool under the best conditions, you must:

- know the basics of the shell of your OS,
- master the basic concepts of the C++ language,
- have successfully completed the Academy's [Hello World!](https://gamebuino.com/academy/workshop/make-your-very-first-games-with-pong/hello-world) workshop,
- for high resolution: read the article [High Resolution without gb.display](https://gamebuino.com/creations/high-resolution-without-gb-display).

Owning a Gamebuino META is obviously essential to be able to record the screen.

Finally, to be able to run this utility and generate animated GIFs, you will need the following free software on your computer:

- **PHP 7.x** for **CLI** (*Command Line Interface*), i.e. accessible from your shell,
- **ImageMagick**, which includes a library and a set of command line utilities to create, convert, edit and display images in a wide variety of formats.
