#import "@local/template:1.0.0": *


#show: paper_template.with(
 author: [Ferdinand Souchet],
 contact: [ferdinand.souchet\@etu.umontpellier.fr],
 title: [HAI719I Programmation 3D: Projet de lancé de rayons],
 subtitle: [Phase 3: Modèles 3D et réflexions],
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


Au fur et à mesure que des fonctionnalités sont ajoutés, les images prennent de plus en plus de temps à être rendues. Pour des petites images (480x480), avec les rebonds, cela prend aux alentours de 19s 600ms, et cela va encore augmenter au cours du temps. J'ai déjà effectué quelques optimisations évidentes (eg. retrait d'une inversion de matrice par pixel), mais le temps d'execution reste assez élevé. D'ailleurs, pour capturer l'image suivante, cela a pris 167s soit presque 3 minutes ! (Et encore, c'est avec un processeur relativement puissant !)

#figure(
 image("image_captures/phase2/big_image.png")
)


Heureusement, le lancé de rayon est un algorithme fortement parallélisable, et d'ailleurs, il est exclusivement tourné sur des cartes graphiques pour cette raison. Pour garder le rendu en c++ et éviter de devoir implémenter les structures d'accélération sur le GPU (car on ne peut pas utiliser les structures d'accéleration de Vulkan ou DirectX dans ce projet, car leurs implémentation est opaque), j'ai plutôt décidé de faire tourner le lancé de rayon sur plusieurs threads. Le faire maintenant permet d'avoir une implémentation sur plusieurs threads avant d'implémenter des techniques de rendu et de post-processing complexes.

#figure(
 caption: [Temps de rendu avant et après l'implémentation de multithreading (avec 32 thread logiques sur le processeur)],
 table(
  columns: 3,
  [], [Image 480x480], [Image 1880x1052],
  [1 thread], [19.6s], [167s],
  [2 threads], [9.6s], [81s],
  [8 threads], [2.5s], [21.7s],
  [32 threads], [$9 times 10^2$ ms], [6.8s],
  [64 threads], [$9 times 10^2$ ms], [7.3s],
 )
)


== Retour sur la quête principale: des ombres douces


#figure(
 caption: [Résultat jusque là (sans réflexions).],
 image("image_captures/phase2/sharp_shadows.png", width: 50%),
)


#figure(
 caption: [Avec des ombres douces. Les positions des lumières sont déterminées aléatoirement selon une distribution dans l'espace (ici: uniforme dans une boule ouverte). Avec seulement 50 échantillons, la frontière de l'ombre est bruitée.],
 image("image_captures/phase2/fuzzy_shadows.png", width: 50%),
)


#figure(
 caption: [Ombres douces avec 5000 échantillons. Il n'y a plus de bruit d'image. Prendre autant d'achantillons, puisque cela multiplie le temps de calcul par 100.],
 image("image_captures/phase2/fuzzy_shadows_5000_samples.png", width: 50%),
)


= Phase 3


== Problèmes de transformation


En premier temps, pour la phase 3, j'ai décidé de réusiner une partie du code du projet. En particulier, j'ai changé les fonctions OpenGL utilisées pour le rendu pour utiliser des fonctions plus modèrnes, pour pouvoir avoir la possibilité d'avoir une preview de la scène qui reflète mieux les propriétés des matèriaux dans la scène (pas encore implémenté). J'ai aussi découplé les objets `Sphere` et `Square` de la classe `Mesh`, pour faciliter la manipulation de ces derniers.


#figure(
 caption: [Mauvaise transformation des quads après refactor. Pendant un moment, les fonctions de transformation des quads ne fonctionnaient plus correctement.],
 image("image_captures/phase3/square_fail.png", width: 50%)
)


== Intégration de Monté-Carlo pour les réflexions


=== Matèriaux réfléchissants


Lors de la phase précédente, j'avais déjà travaillé sur des réflexions, car c'est très peu de travail après avoir fait les premiers rebonds pour les ombres. Je me suis penché cette fois-ci pour faire des matèriaux réfléchissants. Un materiel à maintenant une composante d'intensité diffuse $p_d$ et mirroir $p_m$ pour déterminer les rebonds éventuels effectués. Ces composantes sont faites pour avoir un fort contrôlle artistique et non pour modéliser précisément des propriétés physiques des matèriaux.

Pour faire de l'anti-aliasing, on intégrait déjà les effets de la lumière sur chaque pixel avec l'intégration de Monté-Carlo: on envoie aléatoirement plusieurs rayons de lumière dans le pixel, et la couleur résultante est la moyenne des couleurs de tous les rayons de lumière envoyé. On peut utiliser ce système pour simuler les réflexions diffuses et mirroir. On fait aléatoirement une réflexion diffuse (le nouveau rayon est tiré aléatoirement dans l'hémisphère dans la direction de la normale à la surface) ou bien une réflexion mirroir (le rayon est simplement réfléchit avec la normale). La probabilité d'avoir l'une ou l'autre réflexion est déterminée par les paramètres $p_d$ et $p_m$ à l'aide des formules suivantes:

$
 "Probabilité de réflexion diffuse" = p_d / (p_d + p_m)
$
$
 "Probabilité de réflexion mirroir" = p_m / (p_d + p_m)
$

Les paramètres $p_d$ et $p_m$ déterminent aussi la contribution de la réflexion à la couleur finale. C'est pourquoi ce ne sont pas dirrectement des probabilités, mais ils sont normalisés pour déterminer une probabilité de réflexion.


#figure(
 caption: [Effet des réflexions sur la scène. On remarque le bruit crée sur toute l'image, qui n'est pas très fort malgré les 50 échantillons de Monté-Carlo car une partie de l'illumination est apprioximée par le modèle Binn-Phong. On remarque aussi les légers effets des réflexions diffuses dans les coins de la boîte de Cornell.],
 image("image_captures/phase3/reflections.png", width: 50%)
)


#figure(
 caption: [Effet des réflexions avec 500 échantillons. Le bruit d'image est beaucoup moins présent.],
 image("image_captures/phase3/reflection_500_samples.png", width: 50%)
)


=== Problèmes de ballance de blanc


#figure(
 caption: [Lors du changement des valeurs d'illumination, il était parfois difficile de trouver des valeurs qui donneraient un bon rendu. Voilà une scène où les couleurs tendent vers le blanc, car le poid des réflexions était trop grandes par rapport à l'albedo des objets.],
 image("image_captures/phase3/white_ballance_out_the_window.png", width: 50%)
)


== Intersection rayon-maillage


=== L'intersection en théorie


Pour faire l'interseciton entre un rayon et un maillage, on fait l'intersection entre le rayon et tous les triangles du maillage, et on garde l'intersection avec le triangle le plus proche (comme si l'on faisait l'intersection avec une scène composée de triangles). Donc pour cela, il faut pouvoir faire l'intersection entre un rayon et un triangle. Pour effectuer cette intersection, il suffit d'intersecter le rayon avec le plan formé par le triangle, calculer les coordonnées barycentriques (qui seront de toute façon nécessaire pour interpoler les propriétés des sommets), et regarder si ces coordonnées représentent des points dans le triangle (c'est à dire si elles sont toutes dans $[0, 1]$ et si leurs somme est 1).


#figure(
 caption: [Coordonnées barycentriques sur une sphère.],
 image("image_captures/phase3/barycentric_coordinates.png", width: 50%)
)


#figure(
 caption: [Interpolation des uv sur une sphère grâce aux coordonnés barycentriques.],
 grid(
  columns: (1fr, 1fr),
  column-gutter: 3pt,
  image("image_captures/phase3/uv_sphere1.png"),
  image("image_captures/phase3/uv_sphere2.png")
 )
)

Maintenant que l'on a la certitude que l'intersection rayon-triangle fonctionne, on peut rajouter un cube à la boite de Cornell:


#figure(
 caption: [Coordonnées barycentriques d'une boite rajoutée dans la boite de Cornell.],
 image("image_captures/phase3/barycentric_coordinates_box.png", width: 50%)
)

Il suffit ensuite de traiter les meshs comme n'importe quel autre objet, et...

#figure(
 caption: [Ombrage incorrect sur le cube...],
 image("image_captures/phase3/purple_cube_wrong_shadows.png", width: 60%)
)


=== Attention aux intersections bi-directionnelles


Il est clair que le problème précédent est due à une intersection avec la scène qui ne devrait pas être faite lors du lancement du rayon pour savoir si un point est caché par un autre objet. C'est d'ailleurs confirmé par le rendu suivant:

#figure(
 caption: [Rendu avec les parties illuminées colorées en blanc, et les parties cachées collorés normalement.],
 image("image_captures/phase3/shadow_error.png", width: 50%)
)


En fait, ce soucis venait de l'intersection rayon-plan: si le plan était situé dèrière la caméra, mais avec la normale dans le bon sens pour que l'intersection soit prise en compte, alors l'intersection se faisait. Il se trouve que ce cas de figure ne s'était jamais présenté avant, ce qui pouvait faire penser que l'intersection rayon-plan fonctionnait correctement. Ce qui a rendu la cause de ce bug longue à trouver.

#figure(
 caption: [Rendu avec la boite de Cornell dèrière la caméra.],
 image("image_captures/phase3/backwards_render.png")
)


=== Rendu d'un mesh


Une fois le bug précédent résolu, il est possible de faire le rendu de la boite de Cornell avec un cube en plus:

#figure(
 caption: [Le joli cube vu de près :D],
 image("image_captures/phase3/purple_cube.png", width: 50%)
)

#figure(
 caption: [Nouvelle boîte de Cornell],
 image("image_captures/phase3/with_cube_model.png")
)


= Phase 4


== Structure d'acceleration: KDTree

Premier essai: 164s 587ms avec ; 73s 353ms sans :(

-> Problème au niveau du parcours de la structure: au lieu de regarder tout l'AABB, je regardais seulement les plans dans le même axe que la division. Ce qui faisait que toute la structure était explorée.

Second essai: 109s 474ms :(


Troixième essai: 133s 887ms

Avec le test principal d'AABB: 93s 753ms

= Accès au code


Le code de ce projet est accessible sur github: https://github.com/Khusheete/imagine_raytracing
