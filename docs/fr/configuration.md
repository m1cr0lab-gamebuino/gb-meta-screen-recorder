---
rank: 3
layout: default
title: Configuration
permalink: /fr/configuration/
lang: fr
lang-ref: setup
---

# Configuration du croquis

Pour enregistrer des captures d'écrans successifs d'une application qui s'exécute sur votre META, vous allez avoir besoin d'un composant spécifique qui va se charger d'effectuer cette tâche : le `ScreenRecorder`. Pour intégrer ce composant à votre projet, vous allez devoir télécharger deux fichiers qui déclarent et définissent respectivement la classe C++ `ScreenRecorder` :

- [ScreenRecorder.h](https://raw.githubusercontent.com/m1cr0lab-gamebuino/gb-meta-screen-recorder/master/sources/ScreenRecorder.h) qui est le fichier d'en-tête déclaratif de la classe,
- [ScreenRecorder.cpp](https://raw.githubusercontent.com/m1cr0lab-gamebuino/gb-meta-screen-recorder/master/sources/ScreenRecorder.cpp) qui est le fichier de définition de la classe.

> Pour les télécharger, faites un clic droit sur chacun des liens, et sélectionnez ***Enregistrer le lien sous...*** pour le sauvegarder dans le répertoire de votre projet.

Nous allons maintenant examiner comment intégrer et invoquer les services proposés par cette classe dans votre projet. Nous distinguerons le cas d'une application développée pour une résolution standard de celui d'une application développée pour la haute résolution, car les instructions diffèrent légèrement et surtout, le moment opportun pour invoquer la classe `ScreenRecorder` doit être choisi soigneusement.


# Pour une application en résolution standard

Il s'agit du cas le plus simple.

Éditez le croquis principal de votre application (ici, je le nommerai `Sketch.ino`) et insérez les instructions signalées dans le code ci-dessous :

<div class="filename">Sketch.ino</div>
```cpp
#include <Gamebuino-Meta.h>
#include "ScreenRecorder.h" /* <-- insérez cette ligne */

void setup() {
    gb.begin();

    // vos instructions
    // d'initialisation

    ScreenRecorder::init(); // <-- insérez cette ligne
}

void loop() {
    while (!gb.update());
    
    // vos instructions qui peuvent
    // effectuer des tracés à l'écran
    // en utilisant gb.display

    ScreenRecorder::monitor(gb.display._buffer); // <-- insérez cette ligne

    // ---------------------------------------------------------
    // !!! insérez aussi celle-ci si vous êtes sur Windows : !!!
    // 
    ScreenRecorder::setForWindows();
    // ---------------------------------------------------------
}
```

C'est tout ce que vous avez à faire !... Simple, nan ?

> **Remarque importante à propos de Windows**
> 
> L'implémentation de la fonction PHP `fread` sous Windows est buguée : en effet, le tampon de lecture ne révèle les données accumulées que par paquets de 8 ko. Par conséquent, nous sommes contraints d'employer une astuce pour nous affranchir de ce bug. Donc il est **impératif** d'ajouter la ligne suivante pour préciser que la réception des données sera effectuée sur Windows :
> 
> `ScreenRecorder::setForWindows();`
> 
> Si vous êtes sur macOS ou Linux, **ne rajoutez pas** cette ligne...

# Pour une application en haute résolution

Dans le cas d'une application développée pour la haute résolution, les choses sont un peu différentes. En effet, vous ne pourrez pas utiliser les méthodes d'affichage traditionnelles offertes par `gb.display`... Je vous encourage à lire l'excellent article d'[Andy](https://gamebuino.com/@aoneill) sur le sujet : [High Resolution without gb.display](https://gamebuino.com/creations/high-resolution-without-gb-display), qui vous explique sommairement les raisons pour lesquelles ça n'est pas possible et vous livre une méthode de contournement qui consiste à utiliser directement `gb.tft`. J'ai rédigé un tutoriel très complet sur l'approfondissement de cette technique. Vous pouvez le lire si cela vous intéresse (les débutants auront peut-être quelques difficultés à tout assimiler) :
[Éclairage Tamisé en Haute Résolution](https://m1cr0lab-gamebuino.github.io/gb-shading-effect/). Il vous permettra de comprendre en profondeur comment appliquer cette technique dans vos applications.

Pour revenir à notre `ScreenRecorder`, et pour illustrer la manière dont on doit le configurer pour la haute résolution, nous allons emprunter le code proposé par Andy dans son article, que j'ai légèrement modifié pour définir un découpage de l'écran en tranches **horizontales**. En effet, **vous devrez** appliquer ce type de découpage pour pouvoir utiliser `ScreenRecorder`.

> J'y ai déjà inséré les lignes nécessaire pour la configuration de `ScreenRecorder`.

Vous pouvez [télécharger cette version modifiée](https://raw.githubusercontent.com/m1cr0lab-gamebuino/gb-meta-screen-recorder/master/sources/SketchExampleForHD.ino) et l'examiner en détail :

<div class="filename">SketchExampleForHD.ino</div>
```cpp
/*
 * This code is an adaptation of Andy O'Neill's,
 * from his article "High Resolution without gb.display",
 * which I slightly modified to apply a division
 * of the screen into horizontal slices.
 */

#include <Gamebuino-Meta.h>
#include "ScreenRecorder.h"

// Initialization of display constants
#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 128
#define SLICE_HEIGHT    8 /* each slice measures 160x8 pixels */

// Magic to get access to wait_for_transfers_done
namespace Gamebuino_Meta {
    #define DMA_DESC_COUNT (3)
    extern volatile uint32_t dma_desc_free_count;
    static inline void wait_for_transfers_done(void) {
        while (dma_desc_free_count < DMA_DESC_COUNT);
    }
    static SPISettings tftSPISettings = SPISettings(
        24000000,
        MSBFIRST,
        SPI_MODE0
    );
};

// Double buffers for screen data. Fill one while the other is being sent to the screen.
uint16_t buffer1[SCREEN_WIDTH * SLICE_HEIGHT];
uint16_t buffer2[SCREEN_WIDTH * SLICE_HEIGHT];

void setup() {
    gb.begin();
    // We aren't using the normal screen buffer, so initialize it to 0x0 pixels.
    gb.display.init(0, 0, ColorMode::rgb565);
    // Just to push things to the limit for this example, increase to 40fps.
    gb.setFrameRate(32);
    // 
    // ------------------------------------------
    // Initialization of the screen recorder
    ScreenRecorder::init(SLICE_HEIGHT);
    // !!! ADD THIS LINE IF YOU ARE ON WINDOWS !!!
    ScreenRecorder::setForWindows();
    // ------------------------------------------
    // 
}

void loop() {
    while (!gb.update());

    // Loop over each 160x8 pixels slice of the screen.
    for (
        uint8_t sliceIndex = 0;
        sliceIndex < SCREEN_HEIGHT / SLICE_HEIGHT;
        sliceIndex++
    ) {
        // Alternate between buffers. While one is being sent to
        // the screen with the DMA controller, the other can be
        // used for buffering the next slice of the screen.
        uint16_t *buffer = sliceIndex % 2 == 0 ? buffer1 : buffer2;

        // BEGIN DRAWING TO BUFFER
        uint16_t sliceY  = sliceIndex * SLICE_HEIGHT;
        uint16_t initRed = sliceY + gb.frameCount;
        uint16_t blue    = gb.frameCount % 32;
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
                uint16_t red = initRed;
                ++blue %= 32;
                for (uint8_t y = 0; y < SLICE_HEIGHT; y++) {
                        ++red %= 32;
                        uint16_t green = x + y + initRed;
                        buffer[x + y * SCREEN_WIDTH] =
                            (blue  <<  8) |
                            (red   <<  3) |
                            (green << 13) |
                            ((0b111000 & green) >> 3);
                }
        }
        // END DRAWING TO BUFFER

        // As long as this isn't the first time through the loop,
        // make sure the previous write to the screen is done.
        if (sliceIndex != 0) waitForPreviousDraw();
        // And finally send the current buffer slice to the screen!
        customDrawBuffer(0, sliceY, buffer, SCREEN_WIDTH, SLICE_HEIGHT);
        // 
        // ------------------------------------------
        // Record the buffer slice
        ScreenRecorder::monitor(buffer, sliceIndex);
        // ------------------------------------------
        // 
    }
    // Wait for the final slice to complete before leaving the function.
    waitForPreviousDraw();
}

// Use gb.tft calls to communicate with the screen.
void customDrawBuffer(
    int16_t x,
    int16_t y,
    uint16_t *buffer,
    uint16_t w,
    uint16_t h) {
    gb.tft.setAddrWindow(x, y, x + w - 1, y + h - 1);
    SPI.beginTransaction(Gamebuino_Meta::tftSPISettings);
    gb.tft.dataMode();
    gb.tft.sendBuffer(buffer, w*h);
}

void waitForPreviousDraw() {
    Gamebuino_Meta::wait_for_transfers_done();
    gb.tft.idleMode();
    SPI.endTransaction();
}
```

Si vous lancez ce code sur la META, vous obtiendrez l'animation suivante :

![Animation HD](../../assets/figures/andy-application-320x256.gif){: width="160" class="shadow"}

Vous remarquerez que, contrairement à la configuration pour une résolution standard :

- on initialise `ScreenRecorder` en lui transmettant la hauteur des tranches de découpage de l'écran,
- et on lui transmet également l'indice de la tranche courante pendant le *monitoring* de l'écran : c'est nécessaire pour qu'il démarre la transmission vers le script PHP dès qu'il détecte la première tranche (celle d'indice zéro).

Par ailleurs, vous voyez que c'est uniquement après avoir terminé de remplir le tampon de la tranche courante que l'on invoque le `ScreenRecorder`.

> **Remarque importante**
>
> Lors de l'initialisation du `ScreenRecorder`, notez que la hauteur des tranches doit nécessairement être une puissance de `2` non nulle, dont la valeur maximale est `16`... donc seules les valeurs `2`, `4`, `8` et `16` seront prises en compte. Si vous utilisez une valeur supérieure à `16`, l'enregistrement sera simplement désactivé. Et si vous utilisez une valeur inférieure à `16` qui n'est pas une puissance de `2`, vous obtiendrez un enregistrement... plutôt bizarre <i class="far fa-smile"></i>
>
> Je vous conseille d'utiliser la valeur `8` qui ne nécessite que `2` x `2,5` = `5` ko en RAM.


# Configurer l'enregistrement automatique

Nous verrons dans le chapitre suivant comment déclencher manuellement l'enregistrement et l'arrêter. Mais sachez que vous pouvez également le faire de manière automatique en insérant les instructions correspondantes directement dans votre code. Ceci peut s'avérer très pratique lorsque vous souhaitez déclencher ou stopper l'enregistrement à des moments précis, ou encore lorsque certains événements se produisent. Pour cela, il vous suffit d'insérer chacune des deux instructions suivantes où bon vous semble :

```cpp
ScreenRecorder::startRecording(); // pour lancer l'enregistrement
ScreenRecorder::stopRecording();  // pour stopper l'enregistrement
```


Et voilà ! Vous savez désormais comment configurer votre croquis pour être en mesure d'enregistrer, de manière continue, ce qui est affiché sur l'écran de la META. Voyons maintenant comment effectuer véritablement cet enregistrement pour obtenir une GIF animée...