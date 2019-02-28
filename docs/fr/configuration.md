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

- [ScreenRecorder.h](https://raw.githubusercontent.com/iw4rr10r/gb-meta-screen-recorder/master/sources/ScreenRecorder.h) qui est le fichier d'en-tête déclaratif de la classe,
- [ScreenRecorder.cpp](https://raw.githubusercontent.com/iw4rr10r/gb-meta-screen-recorder/master/sources/ScreenRecorder.cpp) qui est le fichier de définition de la classe.

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
}
```

C'est tout ce que vous avez à faire !... Simple, nan ?


# Pour une application en haute résolution

Dans le cas d'une application développée pour la haute résolution, les choses sont un peu différentes. En effet, vous ne pourrez pas utiliser les méthodes d'affichage traditionnelles offertes par `gb.display`... Je vous encourage à lire l'excellent article d'[Andy](https://gamebuino.com/@aoneill) sur le sujet : [High Resolution without gb.display](https://gamebuino.com/creations/high-resolution-without-gb-display), qui vous explique sommairement les raisons pour lesquelles ça n'est pas possible et vous livre une méthode de contournement qui consiste à utiliser directement `gb.tft`. J'ai rédigé un tutoriel très complet sur l'approfondissement de cette technique. Vous pouvez le lire si cela vous intéresse (les débutants auront peut-être quelques difficultés à tout assimiler) :
[Éclairage Tamisé en Haute Résolution](https://iw4rr10r.github.io/gb-shading-effect/). Il vous permettra de comprendre en profondeur comment appliquer cette technique dans vos applications.

Pour revenir à notre `ScreenRecorder`, et pour illustrer la manière dont on doit le configurer pour la haute résolution, nous allons emprunter le code proposé par Andy dans son article, que nous allons légèrement modifier pour définir un découpage de l'écran en tranches **horizontales**. En effet, **vous devrez** appliquer ce type de découpage pour pouvoir utiliser `ScreenRecorder`.

Voici son code, que j'ai légèrement modifié. Je n'y ai volontairement pas encore inséré ce qui est nécessaire pour la configuration du `ScreenRecorder` :

<div class="filename">Sketch.ino</div>
```cpp
/*
 * Ce code est une adaptation de celui d'Andy O'Neill,
 * issu de son article "High Resolution without gb.display",
 * que j'ai légèrement remanié pour appliquer un découpage
 * de l'écran en tranches horizontales.
 */

#include <Gamebuino-Meta.h>

// initialisation des paramètres d'affichage
#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 128
#define SLICE_HEIGHT    8 /* chaque tranche mesure 160x8 pixels */

// routine magique pour utiliser `wait_for_transfers_done`
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

// double tampon pour les données d'affichage :
// on remplit l'un pendant que l'autre est envoyé à l'écran.
uint16_t buffer1[SCREEN_WIDTH * SLICE_HEIGHT];
uint16_t buffer2[SCREEN_WIDTH * SLICE_HEIGHT];

void setup() {
    gb.begin();
    // nous n'utilisons pas le tampon d'écran habituel,
    // donc il faut l'initialiser à 0x0 pixels.
    gb.display.init(0, 0, ColorMode::rgb565);
    // juste pour obtenir plus de fluidité, on augmente
    // la fréquence d'affichage à 32 images/seconde.
    gb.setFrameRate(32);
}

void loop() {
    while (!gb.update());

    // on boucle sur chaque tranche de 160x8 pixels de l'écran.
    for (
        uint8_t sliceIndex = 0;
        sliceIndex < SCREEN_HEIGHT / SLICE_HEIGHT;
        sliceIndex++
    ) {
        // on alterne entre les tampons :
        // pendant que l'un d'entre eux est envoyé à l'écran
        // avec le contrôleur DMA, l'autre peut être utilisé pour
        // mettre en mémoire tampon la tranche suivante de l'écran.
        uint16_t *buffer = sliceIndex % 2 == 0 ? buffer1 : buffer2;

        // DÉBUT DU REMPLISSAGE DU TAMPON
        // ici vous pouvez effectuer les tracés que vous voulez,
        // voici juste un exemple :
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
        // FIN DU REMPLISSAGE DU TAMPON

        // tant qu'il ne s'agit pas du premier passage
        // on s'assure que l'écriture précédente sur l'écran est terminée.
        if (sliceIndex != 0) waitForPreviousDraw();
        // et on envoie enfin le tampon de la tranche courante à l'écran !
        customDrawBuffer(0, sliceY, buffer, SCREEN_WIDTH, SLICE_HEIGHT);
    }
    // on attend que la dernière tranche soit terminée
    // avant de quitter la fonction.
    waitForPreviousDraw();
}

// on utilise des appels à gb.tft pour communiquer avec l'écran.
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

Il nous reste donc à insérer les lignes de codes nécessaires à la configuration de notre enregistreur `ScreenRecorder` :

<div class="filename">Sketch.ino</div>
```cpp
// en début de code :
#include <Gamebuino-Meta.h>
#include "ScreenRecorder.h" /* <-- insérez cette ligne */

// ...

void setup() {
    gb.begin();
    gb.display.init(0, 0, ColorMode::rgb565);
    gb.setFrameRate(32);
    ScreenRecorder::init(SLICE_HEIGHT); // <-- puis celle-ci
}

void loop() {
    while (!gb.update());

    for (
        uint8_t sliceIndex = 0;
        sliceIndex < SCREEN_HEIGHT / SLICE_HEIGHT;
        sliceIndex++
    ) {

        // ...

        customDrawBuffer(0, sliceY, buffer, SCREEN_WIDTH, SLICE_HEIGHT);
        ScreenRecorder::monitor(buffer, sliceIndex); // <-- et enfin celle-là
    }

    waitForPreviousDraw();
}

// ...
```

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