---
rank: 2
layout: default
title: Préparation
permalink: /fr/preparation/
lang: fr
lang-ref: brewing
---

# Installation des logiciels prérequis

Nous avons besoin de :

- [PHP-CLI](http://php.net/manual/fr/features.commandline.php) 7.x pour exécuter le script d'enregistrement qui va réceptionner les captures d'écrans de la META,
- du module PHP [GD](http://php.net/manual/fr/book.image.php) pour le traitement des images, qui va nous permettre de convertir les données brutes binaires réceptionnées en fichiers PNG,
- de la suite logicielle [ImageMagick](https://www.imagemagick.org/) pour convertir ces fichiers PNG en GIF animée.

En fonction de votre système d'exploitation, la procédure d'installation est différente. Par conséquent, reportez-vous directement aux sections qui vous concernent.


# Installation de PHP 7 et du module GD

## macOS Mojave

Vous avez ~~raison~~ de la chance... vous êtes sur un Mac <i class="fab fa-apple"></i> et ces logiciels sont pré-installés avec macOS <i class="far fa-smile"></i> Vous n'avez donc rien à faire de spécial pour cette étape ! Vous pouvez le vérifier avec les commandes suivantes en ouvrant un Terminal :

```bash
$ php -v
PHP 7.1.23 (cli) (built: Nov  7 2018 18:20:35) ( NTS )
Copyright (c) 1997-2018 The PHP Group
Zend Engine v3.1.0, Copyright (c) 1998-2018 Zend Technologies

$ php -m | grep gd
gd  # <-- cela signifie que le module GD est bien installé
```

Vous pouvez passer directement à la section qui détaille l'installation d'ImageMagick.


## Linux Debian

Je détaille ici la procédure d'installation de PHP pour la distribution [Debian](https://www.debian.org/). Si vous possédez une autre distribution, vous devrez adapter les commandes qui suivent en utilisant le gestionnaire de paquets adéquat.

Commencez par mettre à jour votre base de données des paquets disponibles :

```bash
$ sudo apt update
```

Ensuite, effectuez la mise à jour de votre système :

```bash
$ sudo apt upgrade
```

Puis vérifiez si les paquets `php-cli` et `php-gd` ne sont pas déjà installés :

```bash
$ dpkg -l | grep -iE 'php.*-(cli|gd)'
#
#                   dépend de votre architecture
#               (dans mon cas c'est un Raspberry Pi)
#                                  |
#                                  v
ii  php7.0-cli  7.0.33-0+deb9u1  armhf  command-line interpreter for the PHP scripting language
ii  php7.0-gd   7.0.33-0+deb9u1  armhf  GD module for PHP
```

Si l'une de ces lignes (précédées de `ii`) n'apparaît pas chez vous, c'est que le paquet correspondant n'est pas encore installé. Dans ce cas, procédez comme suit pour leur installation :

```bash
# pour installer php-cli :
$ sudo apt install php-cli

# pour installer php-gd :
$ sudo apt install php-gd

# ou pour installer les deux en même temps :
$ sudo apt install php-cli php-gd
```

Une fois que les paquets auront été installés, vérifiez que tout s'est bien passé :

```bash
$ php -v
PHP 7.0.33-0+deb9u1 (cli) (built: Dec  7 2018 11:36:49) ( NTS )
Copyright (c) 1997-2017 The PHP Group
Zend Engine v3.0.0, Copyright (c) 1998-2017 Zend Technologies
    with Zend OPcache v7.0.33-0+deb9u1, Copyright (c) 1999-2017, by Zend Technologies

$ php -m | grep gd
gd  # <-- cela signifie que le module GD est bien installé
```


## Windows 10

<!-- procédure trouvée sur : http://kizu514.com/blog/install-php7-and-composer-on-windows-10/ -->

Téléchargez la dernière version *non-thread safe* de PHP 7 sur [https://windows.php.net](https://windows.php.net/download/)

![Téléchargement de PHP 7 pour Windows 10](../../assets/figures/windows-php7-download.png){: class="shadow"}

Puis désarchivez le contenu du ZIP dans `C:\PHP7`, et :

- dupliquez le fichier `C:\PHP7\php.ini-development`
- en le renommant par `C:\PHP7\php.ini`

Ouvrez ce nouveau fichier `C:\PHP7\php.ini` dans un éditeur de texte comme [Notepad++](https://notepad-plus-plus.org/fr/) et rendez-vous à la section ***Directory in which the loadable extensions (modules) reside***, puis décommentez la ligne `extension_dir = "ext"` :

```
; Directory in which the loadable extensions (modules) reside.
; http://php.net/extension-dir
;extension_dir = "./"
; On windows:
extension_dir = "ext"
```

Descendez un peu plus bas, jusqu'à la section ***Dynamic Extensions*** et décommentez les extensions que vous souhaitez activer (celle qui nous intéresse ici est l'extension `gd2`) :

```
;;;;;;;;;;;;;;;;;;;;;;
; Dynamic Extensions ;
;;;;;;;;;;;;;;;;;;;;;;

; ...

;extension=bz2
;extension=curl
;extension=fileinfo
; -----------------
; c'est ici !
extension=gd2
; -----------------
;extension=gettext
;extension=gmp
;extension=intl
;extension=imap

; ...
```

Ensuite, **ajoutez** `C:\PHP7` à votre variable d'environnement `Path`.  
Cliquez sur l'image pour l'agrandir :

[![Configuration de la variable Path][image]{: class="shadow"}][image]

[image]: ../../assets/figures/php7-windows-path.png

Enfin, dans l'interpréteur de commande **CMD**, testez que l'installation s'est bien déroulée :

```
Microsoft Windows [Version 10.0.17763.195]

noob@GAMER-PC C:\Users\noob
$ php -v  <-- pour tester la version de l'interpréteur PHP
$ php -m  <-- pour afficher la liste des extensions activées
```

Dans la liste des extensions activées, vous devez retrouver `gd2`.

# Installation d'ImageMagick

<!-- j'utilise ici un caractère spécial invisible pour distinguer les titres de sections     -->
<!-- définis avec les mêmes chaînes de caractères, de façon à ce que la table des matières,  -->
<!-- qui est gérée par un code JavaScript pour automatiser le scolling sur la page, ne les   -->
<!-- confonde pas !                                                                          -->

## macOS Mojave &#0;

Pour l'installation d'ImageMagick, je vous suggère d'utiliser l'excellent gestionnaire de paquets [Homebrew](https://brew.sh/index_fr), que vous pouvez installer facilement de la façon suivante :

```bash
$ /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Le script explique ce qu’il va faire, puis fait une pause avant de l’exécuter.

Une fois que Homebrew est installé, vous disposez de la commande `brew` pour gérer vos paquets. Pour plus de détails sur la manière de l'utiliser, vous pouvez saisir la commande suivante dans un Terminal :

```bash
$ brew help
```

Pour installer ImageMagick, il vous suffit de lancer la commande :

```bash
$ brew install imagemagick
```

Par la suite, nous aurons besoin de l'utilitaire de conversion d'images `convert`. Vous pouvez vérifier qu'il est désormais disponible en lançant la commande suivante :

```bash
$ convert -version
Version: ImageMagick 7.0.8-28 Q16 x86_64 2019-02-19 https://imagemagick.org
Copyright: © 1999-2019 ImageMagick Studio LLC
License: https://imagemagick.org/script/license.php
Features: Cipher DPC HDRI Modules OpenMP
Delegates (built-in): bzlib freetype heic jng jp2 jpeg lcms ltdl lzma openexr png tiff webp xml zlib
```


## Linux Debian &#0;

Pour Linux, la procédure est sensiblement la même, avec son propre gestionnaire de paquets :

```bash
$ sudo apt install imagemagick
$ convert -version
```


## Windows 10 &#0;

Pour Windows, [téléchargez](https://www.imagemagick.org/script/download.php#windows) la dernière version d'ImageMagick précompilée. Une fois installée, vous pouvez également ouvrir l'interpréteur de commandes **CMD** et vérifier que l'outil `convert` est bien installé :

```
Microsoft Windows [Version 10.0.17763.195]

noob@GAMER-PC C:\Users\noob
$ convert -version
```


## Tout est prêt !

Et voilà, nous avons installé tous les outils nécessaires à l'exécution du script PHP d'enregistrement qui va capturer les écrans successifs de la META et en faire des fichiers PNG, que nous convertirons ensuite en GIF animée. Passons donc à l'intégration du `ScreenRecorder` dans le code C++ de votre croquis...