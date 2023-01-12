# Commande_numerique_TP
Bonjour, voici le TP de commande numerique de dispositifs. Nous sommes Matthieu Gozard et Kinann Guillossou-Jnaid.
Vous trouverzez sur ce lien github notre compte rendu "TP commande numérique" en pdf. Ce Readme nous sert à vous spécifier rapidement l'avancement de notre travail.

Nous avons généré la PWM de channel1 et sa complémentaire. DT=205 CCR=5312
Nous avons généré la PWM de channel2 et sa complémentaire. 

Nous avons câblé le hacheur et configurer l'ISO reset.
Nous avons fait en sorte de pouvoir modifier les PWM, avons cablé le moteur et l'avons fait tourner à différentes vitesses en changeant le rapport cyclique.

Nous avons configuré l'ADC1 et le DMA en association avec le timer2 afin de faire une mesure de courant.
Ensuite, nous avons converti la valeur numérique du courant en sa valeur réelle en A. Le courant moyen est affiché sur le terminal.

Nous avons utilisé la sonde tachymétrique afin de récupérer la vitesse du moteur et de l'afficher si besoin à travers d'une commande shell.
Pour cela, nous avons utilisé deux timers, le timer 3 pour "compter" le nombre fronts montants et descendants et le timer 5 pour effectuer des mesures de la vitesse à intervalle de temps régulier (100ms).
 
