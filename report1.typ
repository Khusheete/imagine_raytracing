#import "@local/template:1.0.0": *


#show: paper_template.with(
 author: [Ferdinand Souchet],
 contact: [ferdinand.souchet\@etu.umontpellier.fr],
 title: [HAI719I Programmation 3D: Projet de lancé de rayons],
 subtitle: [Phase 1 & 2: premiers modèles],
 short_title: [Lancé de rayons],
)

= Phase 1: intersection rayon-primitives


#grid(
 columns: 2,
 column-gutter: 5pt,
 figure(
  caption: [Résultat de l'intersection rayon-sphère],
  image("image_captures/phase1/ray_sphere.png")
 ),
 figure(
  caption: [Résultat de l'interseciton rayon-carré],
  image("image_captures/phase1/ray_square.png")
 ),
)


#figure(
 caption: [Intersection entre le rayon et une scène simple],
 image("image_captures/phase1/ray_simple_scene.png", width: 50%)
)


= Phase 2


== Modèle Phong et ombres


#figure(
 caption: [Application du modèle Blinn-Phong. Avec toutes les données d'intersection que l'on obtient, ce modèle n'est pas très compliqué à implémenter. Cependant, il manque quelque chose dans cette image...],
 image("image_captures/phase2/phong_no_ambiant.png", width: 50%)
)


#figure(
 caption: [... Avec une lumière ambiante ! Il se trouve que j'ai pendant assez longtemps oublié de mettre une lumière ambiante, j'ai dut refaire les images qui suivent avec une lumière ambiante à cause de mon oublie. Ce qui - je trouve - ajoute pas mal à la qualité visuelle de la scène, même si la différence est assez subtile.],
 image("image_captures/phase2/phong_with_ambiant.png", width: 50%)
)


#figure(
 caption: [Avec le lancé de rayon, il n'est pas très compliqué de rajouter des ombres, puisuqe l'on peut tracer un rayon vers la lumière, et voir si elle est atteinte.],
 image("image_captures/phase2/sharp_shadows.png", width: 50%)
)


== Quête secondaire 1: Mappage de ton


Une fois que l'on a adapté la fonction de lancé de rayon dans la scène pour voir s'il y a des ombres causées par les différentes lumières, il est assez simple de rajouter un rebond aux rayons, pour simuler des surfaces réflectives. C'est ce que j'ai fait juste après avoir rajouté les ombres:

#figure(
 caption: [Toutes les surfaces sont des mirroirs parfaits !],
 image("image_captures/phase2/everything_reflective_no_tonemapping.png", width: 50%)
)

L'image obtenue n'est pas très belle... Il y a beaucoup de zones avec des couleurs bien trop saturées. La raison de ces couleurs est assez simple: les endroit où les couleurs sont trop saturées on en fait des composantes supèrieur à 1, et doivent donc être remises à 1 lors de l'écriture de l'image sur le disque. Il y a plusieurs manières de faire pour rendre un image plus sensée. La première option serait d'ajuster la lumière ou les matèriaux des objets pour que la plupart des composantes soient entre 0 et 1. La seconde est d'utiliser du mappage de tons pour faire passer les composantes de l'image d'une plage dynamique élevée (HDR) vers une plage dynamique entre 0 et 1. J'ai arbitrairement choisi d'utiliser l'algorithme de mapping de ton local#footnote[C'est à dire qu'il ne prends en compte que les données d'un pixel, au lieu de prendre en compte les données de toute l'image.] AgX.

#figure(
 caption: [Image après application de tone mapping avec AgX. Une grande amélioration visuelle.],
 image("image_captures/phase2/everything_reflective_tonemapping.png", width: 50%)
)


== Quête secondaire 2: Parallélisme


Au fur et à mesure que des fonctionnalités sont ajoutés, les images prennent de plus en plus de temps à être rendues. Pour des petites images (480x480), avec les rebonds, cela prend aux alentours de 19s 600ms, et cela va encore augmenter au cours du temps. J'ai déjà effectué quelques optimisations évidentes (diminution du nombre de division, précalculs de quelques valeurs, et retrait d'une inversion de matrice par pixel), mais le temps d'execution reste assez élevé. D'ailleurs, pour capturer l'image suivante, cela a pris 167s soit presque 3 minutes !

#todo[Add image]


Le lancé de rayon est un algorithme fortement parallélisable, et en production, il est exclusivement tourné sur des cartes graphiques pour cette raison. Pour garder le rendu en c++ et éviter de devoir implémenter les structures d'accélération sur le GPU (car on ne peut pas utiliser les structures d'accéleration de Vulkan ou DirectX dans ce projet, car leurs implémentation est opaque), j'ai plutôt décidé de faire tourner le lancé de rayon sur plusieurs threads. Je le fais maintenant, comme ça je n'aurai pas à l'implémenter après avoir rajouté d'autres techniques plus complexes à faire. Cela permettra aussi de grandement diminuer la vitesse de debug des méthodes par la suite (car le temps de rendu est diminué).

#todo[Add numbers]
