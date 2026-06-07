# Lab 23 : JNI + Protection Anti-Debug Native



## Objectif personnel

Ce laboratoire a permis de comprendre comment renforcer la sécurité d'une application Android en déplaçant des contrôles anti-debug dans une bibliothèque native C++ via JNI. L'idée principale est de rendre plus difficile la rétro-ingénierie et l'analyse dynamique de l'application en cachant une partie de la logique défensive dans une couche native, là où l'accès est moins direct qu'en Java pur.

## Ce qui a été appris

- Utiliser `ptrace` pour détecter si un débogueur est attaché au processus
- Lire et analyser le fichier `/proc/self/maps` pour repérer des signatures suspectes (Frida, Xposed, gdbserver, Magisk, etc.)
- Remonter un résultat natif vers Java sous forme de booléen via JNI
- Adapter dynamiquement l'interface utilisateur en fonction de l'état de sécurité détecté
- Journaliser des événements depuis le code natif vers Logcat
- Comprendre les limites des protections natives (faux positifs, contournements possibles)

## L'application développée

L'application réalise les actions suivantes :

- Au démarrage, elle charge une bibliothèque native (`libnative-lib.so`)
- Elle appelle une méthode native `isDebugDetected()` qui exécute deux contrôles de sécurité
- Le premier contrôle tente d'utiliser `ptrace` pour savoir si le processus est surveillé
- Le deuxième contrôle ouvre le fichier `/proc/self/maps` et parcourt chaque ligne à la recherche de noms de bibliothèques connus pour être utilisés dans des outils d'instrumentation
- Si l'un des deux contrôles échoue (ou détecte une anomalie), la méthode native retourne `true` à Java
- En fonction de cette valeur, l'interface s'affiche en vert avec "État securite : OK" ou en rouge avec "environnement suspect detecte"
- Dans le cas suspect, les autres fonctions natives (`helloFromJNI()` et `factorial()`) sont volontairement désactivées et remplacées par des messages d'alerte
- Toute la vie de l'application est tracée dans Logcat avec le tag `ANTI_DEBUG`, ce qui permet de suivre pas à pas les décisions prises par le code natif

## Résultats obtenus

| Situation | Comportement observé |
|-----------|----------------------|
| Lancement normal (sans débogueur) | Statut vert "OK", message JNI affiché, calcul factoriel de 10 = 3628800 |
| Lancement avec débogueur attaché (Android Studio) | Statut rouge "environnement suspect detecte", fonctions natives bloquées |
| Logs en mode normal | Messages : "Aucun trace/debug detecte" + "Aucune signature suspecte" |
| Logs en mode debug | Messages : "Etat suspect : trace/debug detecte" dans Logcat |

## Difficultés rencontrées

- La fonction `ptrace(PTRACE_TRACEME, 0, 0, 0)` ne fonctionne pas toujours de la même façon selon les versions d'Android et les politiques de sécurité du noyau
- L'ouverture de `/proc/self/maps` peut échouer sur certains appareils verrouillés ou sous certaines conditions
- Il a fallu bien distinguer le moment où la bibliothèque est chargée (dans le `static {}` Java) et le moment où les méthodes sont appelées
- La compilation NDK nécessite que CMake soit correctement configuré, avec la bibliothèque `log` liée pour que `__android_log_print` fonctionne
