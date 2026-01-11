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


Heureusement, le lancé de rayon est un algorithme fortement parallélisable, et d'ailleurs, il est exclusivement tourné sur des cartes graphiques pour cette raison. Pour garder le rendu en c++ et éviter de devoir implémenter les structures d'accélération sur le GPU (car on ne peut pas utiliser les structures d'accélération de Vulkan ou DirectX dans ce projet, car leurs implémentation est opaque), j'ai plutôt décidé de faire tourner le lancé de rayon sur plusieurs threads. Le faire maintenant permet d'avoir une implémentation sur plusieurs threads avant d'implémenter des techniques de rendu et de post-processing complexes.

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


== Structure d'accélération: KDTree


=== Erreurs d'implémentation

Pour présenter mon implémentation de kdtree, la structure d'acceleration utilisée pour faire le rendu de maillages triangulaires, il me semble interessant de raconter (résumé bien sûr) les étapes prises avant d'y arriver. Surtout qu'il à été assez long à implémenter correctement.

D'abord, puisque le but est d'accélérer le temps de rendu, voilà @kdtree_sphere_scene la scène qui sera utilisée pour éstimer les performances. Avec ces structures d'accelerations, on s'attend à voir de très gros impacts sur les performances pour les comparaisons avec/sans, donc il n'y a pas besoin de donner plus de précisions qu'à la seconde près.

#figure(
 caption: [Image d'un maillage triangulaire d'une uv-sphère. Le temps de rendu _sans_ structure d'accélération est d'environs 73s.],
 image("image_captures/phase4/render_kdtree_yay.png", width: 40%)
) <kdtree_sphere_scene>

Tout d'abord, pour la construction du KDTree est utilisé un algorithme naif qui coupe le maillage sur chacun des axes successivement selon la médiane de la position des triangles. La position utilisée pour les triangles est leur barycentre. Le partitionnement obtenu @kdtree_visu correspond bien à la forme de la structure.

#figure(
 caption: [Visualisation des feuilles du KDTree. On affiche les AABB des feuilles ainsi que le barycentre des triangles qu'elles contiennent.],
 image("image_captures/phase4/kdtree.png", width: 50%),
) <kdtree_visu>

Pour la première implémentation du parcours de la structure... L'image @kdtree_sphere_scene était bien obtenue, mais le rendu prenait 165s ! Plus du double du temps sans utiliser le KDTree ! Ici, au moment de calculer les intersection avec tous les plans des AABB des noeuds du KDTree, je ne regardais que les plans dans le même axe que la division. Ce qui faisait que toute la structure était explorée ! D'où l'augmentation drastique dans le temps de calcul.

Le soucis est que même avec ce changement le rendu prenait toujours entre 100s et 140s (en fonction des différentes modifications que j'essayais de faire au parcours - d'ailleurs tous ne donnaient pas une sphère complète).

Pour avoir une chance de savoir d'où les problèmes de performances venaient, j'ai décidé de rendre sur une image une visualisation du temps de calcul utilisé pour chaque pixel lors du passe de raytracing (je ne compte pas le temps de post processing). Non seulement les informations données par ces images donnent plus d'informations sur l'état du rendu que les images rendues elles-mêmes, mais elles possèdent aussi une qualité artistique intéressante.


#figure(
 caption: [Coût de rendu pour chaque pixel. À gauche, sans kdtree, à droite, avec. L'échelle de couleur est purement relative au pixel qui a pris le plus de temps à être calculé *dans cette image*. Plus la couleur s'approche du rouge, plus le temps de calcul est grand.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_3.png"),
  image("image_captures/phase4/performance_map_kdtree_3.png"),
 )
) <kdtree_wrong_heatmap1>

@kdtree_wrong_heatmap1 on observe à gauche (avec KDTree) des motifs étonnants sur la sphère elle-même, qui indique que l'algorithme de parcours n'est pas bon. En effet, on devrait plutôt voir apparaitre des lignes allignées sur les axes plutôt que des axes radiaux.

Le temps de faire chaque test n'étant pas insignifiant, c'est à ce moment là que j'ai limité le parcours dans le KDTree uniquement aux rayons qui intersectent sa boite englobante alignée sur les axes. Ce qui permet aussi de tester la validité de cette partie de l'algorithme (elle ne l'était pas). Après celà le temps de rendu était réduit autours des 90s.

Les figures suivantes montrent les cartes thermiques de temps de calcul avec les images rendues associées au fur et à mesure des modifications que j'ai faites à l'algorithme de parcours de KDTree.

#figure(
 caption: [Cartes thermiques juste après avoir limité le parcours de KDTree à un rayon intersectant son AABB. Un pixel est noir quand son temps de rendu est négligeable devant le temps de calcul des autres pixels.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_kdtree_4.png"),
  image("image_captures/phase4/performance_map_kdtree_4b.png"),
 )
) <kdtree_wrong_heatmap2>


#figure(
 caption: [À gauche, la carte thermique de performance, à droite, l'image rendue.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_kdtree_5_oopsie1.png"),
  image("image_captures/phase4/render_kdtree_5_oopsie1.png"),
 )
)


#figure(
 caption: [À gauche, la carte thermique de performance, à droite, l'image rendue.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_kdtree_5_oopsie2.png"),
  image("image_captures/phase4/render_kdtree_5_oopsie2.png"),
 )
)


#figure(
 caption: [À gauche, la carte thermique de performance, à droite, l'image rendue.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_kdtree_5_oopsie3.png"),
  image("image_captures/phase4/render_kdtree_5_oopsie3.png"),
 )
)


#figure(
 caption: [À gauche, la carte thermique de performance, à droite, l'image rendue. Oops, l'intersection d'AABB ne fonctionne plus ! Mais l'image n'a pas beaucoup changée ?],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_kdtree_5_oopsie4.png"),
  image("image_captures/phase4/render_kdtree_5_oopsie4.png"),
 )
)


#figure(
 caption: [À gauche, la carte thermique de performance, à droite, l'image rendue. On se rapproche d'un algorithme correcte: on commence à voir les AABBs des feuilles apparaître.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_kdtree_5_oopsie5.png"),
  image("image_captures/phase4/render_kdtree_5_oopsie5.png"),
 )
)


#figure(
 caption: [À gauche, la carte thermique de performance, à droite, l'image rendue. Encore un peu plus de feuilles visibles ! Il reste un biais pour explorer les feuilles une direction.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_kdtree_5_oopsie6.png"),
  image("image_captures/phase4/render_kdtree_5_oopsie6.png"),
 )
)


=== Un KDTree réussi !


Après beaucoup de débug, le parcours du KDTree fonctionne ! On peut voir apparaître assez clairement @kdtree les feuilles de l'arbre, ainsi que la sphère elle même (qui demande plus de performances à cause des rebonds).

Et combien de temps prends ce rendu ? 2.6s ! Pour une division par presque 28 du temps de rendu !

#figure(
 caption: [À gauche, la carte thermique de performance, à droite, l'image rendue.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/performance_map_kdtree_yay.png"),
  image("image_captures/phase4/render_kdtree_yay.png"),
 )
) <kdtree>


Si on regardes bien la carte thermique, on peut remarquer que les temps de calculs les plus élevés se situent en dehors de l'AABB de la sphère. Il semble probable que ces pics de temps de calcul soient dues au scheduler du système d'exploitation (même si ça n'est pas certain). Au fur et à mesure des changement d'algorithmes de parcours, le temps de calcul total est devenu de plus en plus petit, et la présence de ces artéfactes explique le fait que les cartes thermiques soient devenues de plus en plus vertes, même si elles sont sensées être relatives.


== Réfractions


#figure(
 caption: [Image de sphères transparantes avec réfraction (indice de réfraction ~1.1) selon deux points de vues.],
 grid(
  columns: 2,
  column-gutter: 2pt,
  image("image_captures/phase4/refractive_sphere.png"),
  image("image_captures/phase4/scene_with_refraction.png"),
 )
)

L'implémentation des réfractions est très proche de celle des réflexions. Il suffit d'ajouter un coefficient pour la réfraction en plus des coefficients diffus et réflectifs. Ce qui permet de donner une probabilité de réfracter le rayon.

Il y a cependant deux points auquels il faut faire attention. Le premier est plutôt simple: pour éviter qu'un rayon réfléchi (de manière diffuse et mirroir) rentre en collision directement avec la surface réfléchissante à cause d'erreurs de précision, il fallait rajouter une fraction de la normale à la position. Le soucis étant qu'un rayon réfracté doit rentrer dans l'objet en question. Pour que cela puisse se faire, il faut donc rajouter une fraction de la normale _dans_ la direction du rayon qui repars (qu'il soit réfracté ou réfléchit).

Le second viens de la loie de Snell-Descartes qui mets en relation l'angle incident $i_1$, l'angle réfracté $i_2$, et les indices optiques $n_1$ et $n_2$ des deux millieux correspondants: $n_1 sin i_1 = n_2 sin i_2$. Cette formule nous permet de trouver le sinus de l'angle réfracté: $sin i_2 = n_1 / n_2 sin i_1$. Le soucis étant que si $n_1 >= n_2$, alors on peut trouver $sin i_2 > 1$, ce qui n'est pas sensé être possible. En effet, quand on obtient un tel résultat, le rayon ne peut pas être réfracté (il sera plutôt réfléchit).


== Textures


#figure(
 caption: [Scène avec un mur comprenant une texture],
 image("image_captures/phase4/scene_with_texture.png", width: 50%)
) <scene_with_texture>

#figure(
 caption: [Sphère avec une texture de la Terre],
 image("image_captures/phase4/earth.png", width: 50%),
) <earth>

Le calcul des coordonnées UV pour les modèles 3D ainsi que les quads ayant déjà été fait lors des calculs pour les intersections avec les rayons. Il suffit d'échantillonner les textures selon ces coordonnées UV pour faire le rendu avec les texture, ce qui donne les résultats @scene_with_texture et @earth.

= Accès au code


Le code de ce projet est accessible sur github: https://github.com/Khusheete/imagine_raytracing
