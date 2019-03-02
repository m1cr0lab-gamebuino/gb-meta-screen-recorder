---
rank: 4
layout: default
title: Utilisation
permalink: /fr/utilisation/
lang: fr
lang-ref: usage
---

# Téléchargement du script PHP

Maintenant que votre croquis est configuré, vous pouvez passer à l'enregistrement de l'écran de votre console. Pour cela, vous allez avoir besoin d'un scipt PHP (`screenrecord`) qui va écouter le port série par lequel la META va communiquer ses données à votre ordinateur. `screenrecord` va donc réceptionner ces données brutes : il s'agit d'un flot d'entiers sur 16 bits codant les couleurs de l'ensemble des pixels qui sont affichés à l'écran. Le script va donc décoder toutes ces données, procéder à la reconstuction des frames qui se succèdent sur l'écran de la META et enregistrer chacun d'entre eux dans un fichier au format PNG. Tous ces fichiers seront numérotés pour conserver l'ordre de capture. Vous pourrez alors utiliser l'outil `convert`, qui fait partie de la suite logicielle ImageMagick, pour convertir cette série de fichiers PNG en une GIF animée.

Nous allons commencer par télécharger le script PHP pour l'installer sur votre ordinateur. Pour cela, faites un clic droit sur le lien suivant : [screenrecord](https://raw.githubusercontent.com/iw4rr10r/gb-meta-screen-recorder/master/sources/screenrecord), puis sélectionnez ***Enregistrer le lien sous...*** et sauvegardez-le dans votre dossier `Téléchargements`.


# Installation du script

La première chose à faire est de le rendre exécutable par votre système d'exploitation et de faire en sorte qu'il puisse être localisé par votre système, quel que soit le répertoire dans lequel vous vous trouvez.


## macOS Mojave & Linux Debian

Pour rendre votre script exécutable, lancez les commandes suivantes :

```bash
$ cd ~/Downloads          # <-- le répertoire où vous avez sauvegardé le script
$ chmod u+x screenrecord  # <-- ajoute la permission d'exécution
```

La localisation des exécutables par le système s'appuie sur la variable d'environnement `PATH`. Si vous possédez déjà un répertoire dans votre espace utilisateur où vous prenez l'habitude de ranger vos scripts et binaires exécutables, rangez le script `screenrecord` à cet endroit. Votre variable `PATH` est déjà probablement configurée pour le prendre en compte. Sinon, continuez à lire la suite...

Si vous n'avez pas encore de répertoire dédié à cet usage, je vous suggère d'en créer un (par exemple `~/bin`). Ouvrez un Terminal et lancez les commandes suivantes :

```bash
$ mkdir ~/bin           # <-- création de votre répertoire d'exécutables
$ cd ~/Downloads        # <-- le répertoire où vous avez sauvegardé le script
$ mv screenrecord ~/bin # <-- déplacez-le dans votre répertoire d'exécutables
```

Puis éditez votre fichier de configuration `~/.bashrc` et ajoutez-y la ligne suivante à la fin :

<div class="filename">~/.bashrc</div>
```bash
export PATH="$HOME/bin:$PATH"
```

Voilà, vous devriez maintenant être en mesure d'exécuter le script `screenrecord` quel que soit le répertoire où vous vous trouvez :

```bash
$ screenrecord -h

+---------------------------------------+
|    Gamebuino META Screen Recorder     |
| © 2019 Stéphane Calderoni (aka Steph) |
|     https://gamebuino.com/@steph      |
+---------------------------------------+

Usage: screenrecord [options]

DESCRIPTION

	This utility records the Gamebuino META screen in Standard & High Definition
	using communication through a serial port

OPTIONS

	-h  displays the synopsis
	-p  sets the serial port (default: /dev/cu.usbmodem141401)
	-d  sets the output directory (default: current directory)
	-b  sets the image files basename (default: frame)
	-n  sets the number of digits to number the image files (default: 4)

ANIMATED GIF CREATION

	You must have imagemagick installed to be able to generate the animated GIF file.
	For example, to generate an animation with a resolution of 160x128 pixels at 25 fps,
	simply type the following command:

	convert -delay 4 -loop 0 png_dir_path/*.png -scale 160x128 screenrecording.gif

```


## Windows 10

Dans le cas de Windows, il existe quelques étapes à suivre plus complexes pour faire fonctionner PHP plus simplement depuis la ligne de commande. Je vous encourage à consulter [cette documentation](http://php.net/manual/fr/install.windows.legacy.index.php#install.windows.legacy.commandline) si vous souhaitez les appliquer.

Mais nous préférerons ici employer une méthode plus immédiate pour aller à l'essentiel et être en mesure d'effectuer nos enregistrements rapidement. Rappelez-vous que vous avez installé PHP dans le dossier `C:\PHP7` et que vous avez ajouté ce chemin dans votre variable d'environnement `Path`. De cette manière, l'exécutable `php.exe` est accessible quel que soit le répertoire dans lequel vous vous trouvez depuis votre interpréteur de commandes.

Nous allons appliquer sensiblement la même méthode pour fixer la localisation du script `screenrecord`. Par exemple, nous allons créer un répertoire `C:\bin` et y déplacer le script en lui ajoutant l'extension `.php` :

```
$ mkdir C:\bin
$ move %HomePath%\Downloads\screenrecord C:\bin\screenrecord.php
```

Pour vérifier que l'on peut bien exécuter le script, on pourra lancer la commande suivante :

```
$ php C:\bin\screenrecord.php -h
```

Ça fait un peu long, nan ? Pour simplifier les choses nous allons créer un fichier *batch* qui va le faire à notre place. Et nous nommerons ce fichier `screenrecord.bat`. Donc créez ce fichier dans votre répertoire `C:\bin` et inscrivez-y les lignes suivante :

<div class="filename">C:\bin\screenrecord.bat</div>
```
@echo OFF
php C:\bin\screenrecord.php %*
```

Il ne nous reste plus qu'à ajouter `C:\bin` à la variable d'environnement `Path`, en procédant comme nous l'avons fait dans la section [Préparation](../preparation/) lors de l'installation de PHP...

Et maintenant, vous pouvez lancer ce fichier de commande batch depuis n'importe quel répertoire :

```bash
$ screenrecord.bat -h

+---------------------------------------+
|    Gamebuino META Screen Recorder     |
| © 2019 Stéphane Calderoni (aka Steph) |
|     https://gamebuino.com/@steph      |
+---------------------------------------+

Usage: screenrecord [options]

DESCRIPTION

	This utility records the Gamebuino META screen in Standard & High Definition
	using communication through a serial port

OPTIONS

	-h  displays the synopsis
	-p  sets the serial port (default: /dev/cu.usbmodem141401)
	-d  sets the output directory (default: current directory)
	-b  sets the image files basename (default: frame)
	-n  sets the number of digits to number the image files (default: 4)

ANIMATED GIF CREATION

	You must have imagemagick installed to be able to generate the animated GIF file.
	For example, to generate an animation with a resolution of 160x128 pixels at 25 fps,
	simply type the following command:

	convert -delay 4 -loop 0 png_dir_path/*.png -scale 160x128 screenrecording.gif

```


# Comment je fais pour enregistrer mon écran ?

La première chose à faire ici est de déterminer l'identifiant du périphérique série par lequel votre META va communiquer avec votre ordinateur.


## Identifier le port série

Connectez la META à votre ordinateur avec un câble USB, puis ouvrez l'Arduino IDE et rendez-vous dans le menu **Tools** :

![Identification du port série](../../assets/figures/mac-serial-port.jpg)

Mon identifiant est `/dev/cu.usbmodem141401`. L’identifiant du port sera probablement différent chez vous, mais ça n'est pas gênant. C'est cet identifiant que vous devrez spécifier à `screenrecord` au moment de l'exécuter. Donc notez-le quelque-part !

> **Remarque concernant Windows**
>
> Vous devrez préalablement aller jeter un coup d'oeil dans le **Gestionnaire de périphériques** pour identifier le bon port :
>
> ![Gestionnaire de périphériques](../../assets/figures/windows-device-manager.png){: class="shadow"}
>
> Ici, il s'agit du port `COM13`. Retenez-le pour la suite...


## Effectuer l'enregistrement manuellement

Maintenant que nous avons identifié le bon port, et que nous pouvons exécuter la commande `screenrecord` depuis n'importe-quel répertoire, vous pouvez vous rendre dans le répertoire du projet Gamebuino de votre choix (celui que vous avez configuré en ajoutant les instructions nécessaires qui invoquent la classe `ScreenRecorder`).

Dans le chapitre précédent, nous avions créé une application pour la haute résolution (`Sketch.ino`). Si vous l'avez recopié, vous pouvez vous placer dans le répertoire correspondant.

Comme nous l'avons vu, le script PHP va générer une série de fichiers PNG lors de l'enregistrement. Je vous suggère donc de créer un répertoire qui va permettre d'accueillir ces fichiers pour ne pas mettre le boxon dans votre projet <i class="far fa-smile"></i> Nous nommerons par exemple ce répertoire `frames` :

```bash
$ mkdir frames
```

Compilez et téléchargez votre code C++ sur la META pour lancer votre application, puis exécutez la commande suivante (en prenant soin de remplacer l'identifiant du port série par celui qui vous correspond) :

```bash
$ screenrecord -d frames -p /dev/cu.usbmodem141401     # <-- macOS & Linux
$ screenrecord.bat -d frames -p COM13                  # <-- Windows 10
```

Vous allez constater que le script se met en attente des données qui vont arriver sur le port série...

Sur la META, pour lancer l'enregistrement, appuyez longuement (au moins 1 seconde) sur le bouton **MENU** (puis vous pouvez relâcher). Vous remarquerez que les LEDs de la console émettent des flashs rouges à intervalles réguliers. Cela signifie qu'elle est bel et bien en train d'envoyer les données d'enregistrement sur le port série. Dans le même temps, vous verrez sur votre interpréteur de commande que le script réceptionne bien ces données et les enregistre dans l'ordre d'arrivée des captures. Vous devriez observer quelque-chose dans ce goût là :

```
$ screenrecord -d frames

Start listening to the serial port /dev/cu.usbmodem141401

Waiting for data... Start screen recording in high resolution

Decoding frame 0001... saved
Decoding frame 0002... saved
Decoding frame 0003... saved
Decoding frame 0004... saved
Decoding frame 0005... saved
Decoding frame 0006... saved
Decoding frame 0007... saved
Decoding frame 0008... saved
Decoding frame 0009... saved
Decoding frame 0010... saved
.
.
.
```

Pour arrêter l'enregistrement, rappuyez brièvement sur le bouton **MENU**... Les LEDs s'éteignent, le flux de données s'interrompt et le script s'arrête :

```
.
.
.
Decoding frame 0100... saved
Decoding frame 0101... saved
Decoding frame 0102... saved
Decoding frame 0103... saved
Decoding frame 0104... saved
Decoding frame 0105... saved
Decoding frame 0106... discarded

105 PNG files have been recorded in 160x128 (high resolution).

You must have imagemagick installed to convert these PNG files into an animated GIF file.
For a 25 fps animation, simply type the following command:

convert -delay 4 -loop 0 frames/*.png -scale 160x128 screenrecording.gif

```

Vous voyez que la dernière capture d'écran n'est pas conservée. En effet, dans le cas de la haute résolution, l'enregistrement a de grande chances de s'interrompre avant que la totalité des tranches n'aient été envoyées. Le script décide donc de jeter la dernière capture.

La commande permettant de générer une GIF animée à partir de la série de captures vous est indiquée.

> **Remarque sur l'option `delay`**
>
> La valeur `N` attendue par l'option `delay` est un entier qui correspond à une fréquence d'animation de `100/N` fps. Donc, si `N=4`, votre GIF animée aura une fréquence de `100/4 = 25` fps.

Dans le code de `Sketch.ino`, nous avions spécifié une fréquence d'exécution de `32` fps :

```cpp
gb.setFrameRate(32);
```

Donc pour que notre GIF respecte *grosso modo* cette fréquence, nous lancerons la commande suivante :

```bash
$ convert -delay 3 -loop 0 frames/*.png -scale 160x128 screenrecording.gif
```

Et nous obtenons le fichier `screenrecording.gif` que voici :

![GIF animée](../../assets/figures/andy-application-320x256.gif){: width="160" class="shadow"}

> **Remarque**
>
> J'ai un peu triché... j'ai supprimé les frames superflus pour que l'animation en boucle soit parfaitement raccord... <i class="far fa-smile-wink"></i>
>
> Il est en effet encore possible de corriger votre enregistrement, en supprimant des frames, ou en les réordonnant si nécessaire. Pratique, nan ?

Voici une petite vidéo de démonstration qui illustre toute la procédure d'enregistrement que nous venons d'appliquer :

<iframe width="560" height="315" src="https://www.youtube.com/embed/C85M7hLqs0g" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


## Automatiser l'enregistrement

Je rappelle simplement ici que, plutôt que de déclencher et d'arrêter l'enregistrement manuellement, à l'aide du bouton **MENU**, vous pouvez l'automatiser directement dans votre code C++ en plaçant les instructions suivantes aux endroits appropriés:

```cpp
ScreenRecorder::startRecording(); // pour lancer l'enregistrement
ScreenRecorder::stopRecording();  // pour stopper l'enregistrement
```

De cette manière, vous contrôlerez plus précisément les scènes que vous enregistrerez, et vous pourrez produire des GIF qui correspondent exactement à ce que vous souhaitez nous montrer <i class="far fa-smile"></i>

J'espère que cet outil vous sera utile. J'en avais moi-même un grand besoin, et je suis très content de pouvoir le partager aujourd'hui. Si vous relevez des dysfonctionnements, si vous entrevoyez des améliorations possibles, ou si vous voulez me montrer les captures dont vous êtes le plus fier, n'hésitez pas à me laisser un petit commentaire [sur la page de cette création](https://gamebuino.com/creations/meta-screen-recorder) !

Le code de la classe `ScreenRecorder` et celui du script `screenrecord` sont à votre disposition [sur mon dépôt GitHub](https://github.com/iw4rr10r/gb-meta-screen-recorder/tree/master/sources). Vous pouvez les modifier librement, et si vous les améliorez, on compte sur vous pour nous en faire profiter <i class="far fa-smile-wink"></i>