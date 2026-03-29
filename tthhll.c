#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ----------- Structures ----------- */

typedef struct {
    char depart[20];
    char symbole[20];
    char arrivee[20];
} Transition;

typedef struct {
    char **etats;
    int nbEtats;
    int capEtats;

    char **alphabet;
    int nbAlphabet;
    int capAlphabet;

    Transition *transitions;
    int nbTransitions;
    int capTransitions;

    char etatInitial[20];

    char **etatsFinaux;
    int nbFinaux;
    int capFinaux;
} Automate;

/* ----------- Initialisation ----------- */

void initialiserAutomate(Automate *a) {
    a->nbEtats = a->nbAlphabet = a->nbTransitions = a->nbFinaux = 0;

    a->capEtats = a->capAlphabet = a->capTransitions = a->capFinaux = 2;

    a->etats = malloc(a->capEtats * sizeof(char*));
    a->alphabet = malloc(a->capAlphabet * sizeof(char*));
    a->transitions = malloc(a->capTransitions * sizeof(Transition));
    a->etatsFinaux = malloc(a->capFinaux * sizeof(char*));

    strcpy(a->etatInitial, "");
}

/* ----------- Redimension ----------- */

void resizeEtats(Automate *a) {
    a->capEtats *= 2;
    a->etats = realloc(a->etats, a->capEtats * sizeof(char*));
}

void resizeAlphabet(Automate *a) {
    a->capAlphabet *= 2;
    a->alphabet = realloc(a->alphabet, a->capAlphabet * sizeof(char*));
}

void resizeTransitions(Automate *a) {
    a->capTransitions *= 2;
    a->transitions = realloc(a->transitions, a->capTransitions * sizeof(Transition));
}

void resizeFinaux(Automate *a) {
    a->capFinaux *= 2;
    a->etatsFinaux = realloc(a->etatsFinaux, a->capFinaux * sizeof(char*));
}

/* ----------- Outils ----------- */

int existeEtat(Automate *a, char *etat) {
	int i;
    for ( i = 0; i < a->nbEtats; i++)
        if (strcmp(a->etats[i], etat) == 0)
            return 1;
    return 0;
}

void ajouterEtat(Automate *a, char *etat) {
    if (!existeEtat(a, etat)) {
        if (a->nbEtats == a->capEtats)
            resizeEtats(a);

        a->etats[a->nbEtats] = malloc(20);
        strcpy(a->etats[a->nbEtats], etat);
        a->nbEtats++;
    }
}

int existeSymbole(Automate *a, char *symbole) {
	int i;
    for ( i = 0; i < a->nbAlphabet; i++)
        if (strcmp(a->alphabet[i], symbole) == 0)
            return 1;
    return 0;
}

void ajouterSymbole(Automate *a, char *symbole) {
    if (!existeSymbole(a, symbole)) {
        if (a->nbAlphabet == a->capAlphabet)
            resizeAlphabet(a);

        a->alphabet[a->nbAlphabet] = malloc(20);
        strcpy(a->alphabet[a->nbAlphabet], symbole);
        a->nbAlphabet++;
    }
}

void ajouterFinal(Automate *a, char *etat) {
    if (a->nbFinaux == a->capFinaux)
        resizeFinaux(a);

    a->etatsFinaux[a->nbFinaux] = malloc(20);
    strcpy(a->etatsFinaux[a->nbFinaux], etat);
    a->nbFinaux++;
}
int estFinal(Automate *a, char *etat) {
    int i;
    for (i = 0; i < a->nbFinaux; i++) {
        if (strcmp(a->etatsFinaux[i], etat) == 0)
            return 1;
    }
    return 0;
}
void ajouterTransition(Automate *a, char *depart, char *symbole, char *arrivee) {
    if (a->nbTransitions == a->capTransitions)
        resizeTransitions(a);
    strcpy(a->transitions[a->nbTransitions].depart, depart);
    strcpy(a->transitions[a->nbTransitions].symbole, symbole);
    strcpy(a->transitions[a->nbTransitions].arrivee, arrivee);
    a->nbTransitions++;

    // Ajouter états et symbole à l'alphabet
    ajouterEtat(a, depart);
    ajouterEtat(a, arrivee);
    ajouterSymbole(a, symbole);
}

/* ----------- Lecture ----------- */

void lireAutomate(Automate *a, char *nomFichier) {
    FILE *f = fopen(nomFichier, "r");
    char ligne[200];

    if (!f) {
        printf("Erreur fichier\n");
        return;
    }

    while (fgets(ligne, sizeof(ligne), f)) {

        if (strstr(ligne, "init ->")) {
            sscanf(ligne, " init -> %[^;];", a->etatInitial);
            ajouterEtat(a, a->etatInitial);
        }

        else if (strstr(ligne, "doublecircle")) {
            char etat[20];
           sscanf(ligne, " %[^ ] [shape=doublecircle];", etat);
            ajouterFinal(a, etat);
            ajouterEtat(a, etat);
        }

        else if (strstr(ligne, "label")) {
            char d[20], a2[20], s[20];

            sscanf(ligne, " %s -> %s [label=\"%[^\"]\"]", d, a2, s);

            if (a->nbTransitions == a->capTransitions)
                resizeTransitions(a);

            strcpy(a->transitions[a->nbTransitions].depart, d);
            strcpy(a->transitions[a->nbTransitions].arrivee, a2);
            strcpy(a->transitions[a->nbTransitions].symbole, s);

            ajouterEtat(a, d);
            ajouterEtat(a, a2);
            ajouterSymbole(a, s);

            a->nbTransitions++;
        }
    }

    fclose(f);
}

/* ----------- Affichage ----------- */

void afficherAutomate(Automate *a) {
    printf("\nEtats: ");
    int i;
    for (i = 0; i < a->nbEtats; i++)
        printf("%s ", a->etats[i]);

    printf("\nAlphabet: ");
    for (i = 0; i < a->nbAlphabet; i++)
        printf("%s ", a->alphabet[i]);

    printf("\nInitial: %s", a->etatInitial);

    printf("\nFinaux: ");
    for (i = 0; i < a->nbFinaux; i++)
        printf("%s ", a->etatsFinaux[i]);

    printf("\nTransitions:\n");
    for (i = 0; i < a->nbTransitions; i++)
        printf("%s --%s--> %s\n",
               a->transitions[i].depart,
               a->transitions[i].symbole,
               a->transitions[i].arrivee);

    printf("\n");
}

/* ----------- Libération mémoire ----------- */

void libererAutomate(Automate *a) {
	int i;
    for (i = 0; i < a->nbEtats; i++) free(a->etats[i]);
    for ( i = 0; i < a->nbAlphabet; i++) free(a->alphabet[i]);
    for ( i = 0; i < a->nbFinaux; i++) free(a->etatsFinaux[i]);

    free(a->etats);
    free(a->alphabet);
    free(a->etatsFinaux);
    free(a->transitions);
}

/* ---------------- Partie 2 ---------------- */

/* 5. Generer fichier .dot */
void genererDot(Automate *a, char *nomFichier) { //Utile pour vérifier graphiquement les états et transitions.
    FILE *f = fopen(nomFichier, "w"); //fopen ouvre le fichier "w" signifie mode écriture.
    int i;

    if (f == NULL) {
        printf("Erreur creation fichier\n");
        return;
    }

    fprintf(f, "digraph automate {\n"); //digraph signifie graphe orienté (comme un automate).
    fprintf(f, "    rankdir=LR;\n"); //rankdir=LR veut dire que le graphe sera dessiné de gauche vers droite.
    fprintf(f, "    init [shape=point];\n"); //On crée un point de départ appelé init.
    fprintf(f, "    init -> %s;\n", a->etatInitial); //fait une flèche vers l’état initial de l’automate.(--> q0)

    for (i = 0; i < a->nbFinaux; i++) { //boucle parcourt tous les états finaux.
        fprintf(f, "    %s [shape=doublecircle];\n", a->etatsFinaux[i]); //Chaque état final est dessiné avec double cercle q3 (( ))
    }

    for (i = 0; i < a->nbTransitions; i++) {
        fprintf(f, "    %s -> %s [label=\"%s\"];\n",
                a->transitions[i].depart,
                a->transitions[i].arrivee,
                a->transitions[i].symbole);
    }

    fprintf(f, "}\n");
    fclose(f);


}

/* 6. Etat avec plus de transitions sortantes */
void etatPlusSortantes(Automate *a) { //l’état qui possède le plus de transitions sortantes
    int i, j, nb, max = -1;
    char etat[20] = ""; //nom de l’état qui possède le maximum

    for (i = 0; i < a->nbEtats; i++) { //parcourt tous les états de l’automate
        nb = 0;
        for (j = 0; j < a->nbTransitions; j++) {
            if (strcmp(a->etats[i], a->transitions[j].depart) == 0) //Si l’état actuel = état de départ de la transition cela signifie que la transition sort de cet état
                nb++;
        }

        if (nb > max) { //Si cet état possède plus de transitions que le maximum actuel
            max = nb;
            strcpy(etat, a->etats[i]);
        }
    }

    printf("Etat avec le plus de transitions sortantes : %s\n", etat);
}

/* 6. Etat avec plus de transitions entrantes */
void etatPlusEntrantes(Automate *a) {
    int i, j, nb, max = -1;
    char etat[20] = "";

    for (i = 0; i < a->nbEtats; i++) {
        nb = 0;
        for (j = 0; j < a->nbTransitions; j++) {
            if (strcmp(a->etats[i], a->transitions[j].arrivee) == 0) //strcmp compare deux chaînes de caractères
                nb++; //Si l’état correspond au arrivee de la transition, on augmente le compteur.
        }

        if (nb > max) {
            max = nb;
            strcpy(etat, a->etats[i]);
        }
    }

    printf("Etat avec le plus de transitions entrantes : %s\n", etat);
}

/* 7. Etats ayant une transition avec une lettre donnee */
void afficherEtatsAvecLettre(Automate *a, char *lettre) {
    int i, j, trouve, existe = 0;

    printf("Etats ayant une transition sortante avec %s : ", lettre);

    for (i = 0; i < a->nbEtats; i++) { //On regarde chaque état.
        trouve = 0;
        for (j = 0; j < a->nbTransitions; j++) {
            if (strcmp(a->etats[i], a->transitions[j].depart) == 0 &&
                strcmp(lettre, a->transitions[j].symbole) == 0) {
                trouve = 1;
                existe = 1;
                break;
            }
        }

        if (trouve) //Si la condition est vraie, l’état est affiché
            printf("%s ", a->etats[i]);
    }

    if (!existe) //Si aucun état n’est trouvé, le programme affiche
        printf("aucun etat");

    printf("\n");
}


/* 8. Tester un mot */
int motReconnu(Automate *a, char *mot) {
    char etatCourant[20];
    char symbole[2];
    int i, j, trouve;
    //L’automate lit le mot lettre par lettre et change d’état selon les transitions.
    strcpy(etatCourant, a->etatInitial); //etatCourant représente l’état actuel de l’automate

    for (i = 0; mot[i] != '\0'; i++) { //lit le mot caractère par caractère.
        symbole[0] = mot[i];
        symbole[1] = '\0';
        trouve = 0;

        for (j = 0; j < a->nbTransitions; j++) { //chercher une transition correspondante
            if (strcmp(a->transitions[j].depart, etatCourant) == 0 &&
                strcmp(a->transitions[j].symbole, symbole) == 0) {
                strcpy(etatCourant, a->transitions[j].arrivee); //etatCourant garde l’état actuel de l’automate pendant la lecture du mot À chaque lettre, on met à jour etatCourant selon la transition correspondante.
                trouve = 1;
                break;
            }
        }

        if (!trouve) //aucune transition trouvée
            return 0;
    }

    return estFinal(a, etatCourant); //si l’état est final  le mot est accepté
}

/* 9. Lire un fichier de mots */
void lireFichierMots(Automate *a, char *fichierIn, char *fichierOut) {
    FILE *fin = fopen(fichierIn, "r"); //fichier d’entrée (liste des mots)
    FILE *fout = fopen(fichierOut, "w"); //fichier de sortie (mots reconnus)
    char mot[100];

    if (fin == NULL || fout == NULL) {
        printf("Erreur fichier\n");
        return;
    }

    while (fscanf(fin, "%s", mot) == 1) { //Le programme lit un mot à la fois
        if (motReconnu(a, mot)) { //tester chaque mot
            fprintf(fout, "%s\n", mot); //Si le mot est accepté par l’automate, il est écrit dans le fichier de sortie
        }
    }

    fclose(fin);
    fclose(fout);

    printf("Mots reconnus enregistres\n");
}
/* ================= PARTIE 3 ================= */

/* ================= PARTIE 3 CORRIGEE ================= */

/* 1?? Automate simple pour une lettre */
Automate automateSimple(char c) {
    Automate a;
    initialiserAutomate(&a);

    ajouterEtat(&a, "q0");
    ajouterEtat(&a, "q1");
    strcpy(a.etatInitial, "q0");
    ajouterFinal(&a, "q1");

    ajouterTransition(&a, "q0", (char[]){c,'\0'}, "q1");

    return a;
}

/* 2?? Concaténation de deux automates */
Automate concatenation(Automate *a1, Automate *a2) {
    Automate res;
    int i;

    initialiserAutomate(&res);

    // Copier a1
    for(i=0;i<a1->nbEtats;i++) ajouterEtat(&res,a1->etats[i]);
    for(i=0;i<a1->nbTransitions;i++)
        ajouterTransition(&res,a1->transitions[i].depart,
                          a1->transitions[i].symbole,
                          a1->transitions[i].arrivee);

    // Copier a2 (préfixer les états pour éviter collision)
    for(i=0;i<a2->nbEtats;i++){
        char nom[30]; sprintf(nom,"a2_%s",a2->etats[i]);
        ajouterEtat(&res,nom);
    }
    for(i=0;i<a2->nbTransitions;i++){
        char dep[30], arr[30];
        sprintf(dep,"a2_%s",a2->transitions[i].depart);
        sprintf(arr,"a2_%s",a2->transitions[i].arrivee);
        ajouterTransition(&res,dep,a2->transitions[i].symbole,arr);
    }

    // Ajouter epsilon de tous les états finaux de a1 vers init de a2
    for(i=0;i<a1->nbFinaux;i++){
        char arrivee[30]; sprintf(arrivee,"a2_%s",a2->etatInitial);
        ajouterTransition(&res,a1->etatsFinaux[i],"eps",arrivee);
    }

    // Nouveau initial = initial de a1
    strcpy(res.etatInitial,a1->etatInitial);

    // Ajouter états finaux de a2
    for(i=0;i<a2->nbFinaux;i++){
        char fin[30]; sprintf(fin,"a2_%s",a2->etatsFinaux[i]);
        ajouterFinal(&res,fin);
    }

    return res;
}

/* 3?? Union de deux automates */
Automate unionAutomates(Automate *a1, Automate *a2) {
    Automate res;
    int i;

    initialiserAutomate(&res);

    // Etat initial unique
    strcpy(res.etatInitial,"init");
    ajouterEtat(&res,"init");

    // Copier a1
    for(i=0;i<a1->nbEtats;i++) ajouterEtat(&res,a1->etats[i]);
    for(i=0;i<a1->nbTransitions;i++)
        ajouterTransition(&res,a1->transitions[i].depart,
                          a1->transitions[i].symbole,
                          a1->transitions[i].arrivee);

    // Copier a2 avec préfixe
    for(i=0;i<a2->nbEtats;i++){
        char nom[30]; sprintf(nom,"a2_%s",a2->etats[i]);
        ajouterEtat(&res,nom);
    }
    for(i=0;i<a2->nbTransitions;i++){
        char dep[30], arr[30];
        sprintf(dep,"a2_%s",a2->transitions[i].depart);
        sprintf(arr,"a2_%s",a2->transitions[i].arrivee);
        ajouterTransition(&res,dep,a2->transitions[i].symbole,arr);
    }

    // Epsilon depuis init vers init de a1 et a2
    ajouterTransition(&res,"init","eps",a1->etatInitial);
    char init2[30]; sprintf(init2,"a2_%s",a2->etatInitial);
    ajouterTransition(&res,"init","eps",init2);

    // Copier états finaux
    for(i=0;i<a1->nbFinaux;i++) ajouterFinal(&res,a1->etatsFinaux[i]);
    for(i=0;i<a2->nbFinaux;i++){
        char fin[30]; sprintf(fin,"a2_%s",a2->etatsFinaux[i]);
        ajouterFinal(&res,fin);
    }

    return res;
}

/* 4?? Expression régulière postfix -> automate (support + . *) */
Automate expressionToAutomate(char *exp) {
    Automate pile[100];
    int top=-1, i;

    for(i=0; exp[i]; i++){
        if(isalpha(exp[i])){
            pile[++top]=automateSimple(exp[i]);
        }
        else if(exp[i]=='.'){ //concat
            Automate a2=pile[top--];
            Automate a1=pile[top--];
            pile[++top]=concatenation(&a1,&a2);
        }
        else if(exp[i]=='+'){ //union
            Automate a2=pile[top--];
            Automate a1=pile[top--];
            pile[++top]=unionAutomates(&a1,&a2);
        }
        else if(exp[i]=='*'){ //Kleene
            Automate a=pile[top--];
            Automate res;
            initialiserAutomate(&res);
            ajouterEtat(&res,"init");
            strcpy(res.etatInitial,"init");

            // Copier a avec préfixe
            int j;
            for(j=0;j<a.nbEtats;j++){
                char nom[30]; sprintf(nom,"k_%s",a.etats[j]);
                ajouterEtat(&res,nom);
            }
            for( j=0;j<a.nbTransitions;j++){
                char dep[30],arr[30];
                sprintf(dep,"k_%s",a.transitions[j].depart);
                sprintf(arr,"k_%s",a.transitions[j].arrivee);
                ajouterTransition(&res,dep,a.transitions[j].symbole,arr);
            }

            // epsilon init -> initial de a
            char initA[30]; sprintf(initA,"k_%s",a.etatInitial);
            ajouterTransition(&res,"init","eps",initA);

            // epsilon de finaux vers initial de a et init
            for(j=0;j<a.nbFinaux;j++){
                char fin[30]; sprintf(fin,"k_%s",a.etatsFinaux[j]);
                ajouterTransition(&res,fin,"eps",initA);
                ajouterTransition(&res,fin,"eps","init");
            }

            // init devient final
            ajouterFinal(&res,"init");

            pile[++top]=res;
        }
    }

    return pile[top];
}

/* 5?? Supprimer les epsilon transitions */
void supprimerEpsilon(Automate *a){
    int i,j,k;

    int nbTrans= a->nbTransitions;
    int changed=1;

    while(changed){
        changed=0;
        for(i=0;i<a->nbTransitions;i++){
            if(strcmp(a->transitions[i].symbole,"eps")==0){
                // Propager transitions
                for(j=0;j<a->nbTransitions;j++){
                    if(strcmp(a->transitions[j].depart,a->transitions[i].arrivee)==0 &&
                       strcmp(a->transitions[j].symbole,"eps")!=0){
                        ajouterTransition(a,a->transitions[i].depart,
                                          a->transitions[j].symbole,
                                          a->transitions[j].arrivee);
                    }
                }
                // Etat depart devient final si arrivee est final
                if(estFinal(a,a->transitions[i].arrivee))
                    ajouterFinal(a,a->transitions[i].depart);

                // Supprimer epsilon
                for(k=i;k<a->nbTransitions-1;k++)
                    a->transitions[k]=a->transitions[k+1];
                a->nbTransitions--;
                changed=1;
                break;
            }
        }
    }
}
/* ----------- Automate -> Expression Reguliere ----------- */

char* automateToRegex(Automate *a) {
    int i, j, k;

    // matrice expressions
    char R[50][50][100];

    // initialisation
    for(i = 0; i < a->nbEtats; i++) {
        for(j = 0; j < a->nbEtats; j++) {
            strcpy(R[i][j], "");
        }
    }

    // remplir avec transitions
    for(i = 0; i < a->nbTransitions; i++) {
        int dep = -1, arr = -1;

        for(j = 0; j < a->nbEtats; j++) {
            if(strcmp(a->etats[j], a->transitions[i].depart) == 0)
                dep = j;
            if(strcmp(a->etats[j], a->transitions[i].arrivee) == 0)
                arr = j;
        }

        if(dep != -1 && arr != -1) {
            if(strlen(R[dep][arr]) == 0)
                strcpy(R[dep][arr], a->transitions[i].symbole);
            else {
                strcat(R[dep][arr], "+");
                strcat(R[dep][arr], a->transitions[i].symbole);
            }
        }
    }

    // ajout epsilon sur diagonale
    for(i = 0; i < a->nbEtats; i++) {
        if(strlen(R[i][i]) == 0)
            strcpy(R[i][i], "eps");
    }

    // elimination des états
    for(k = 0; k < a->nbEtats; k++) {
        for(i = 0; i < a->nbEtats; i++) {
            for(j = 0; j < a->nbEtats; j++) {

                if(i != k && j != k) {
                    char temp[200] = "";

                    if(strlen(R[i][k]) && strlen(R[k][j])) {

                        strcat(temp, "(");
                        strcat(temp, R[i][k]);
                        strcat(temp, ")(");
                        strcat(temp, R[k][k]);
                        strcat(temp, ")*(");
                        strcat(temp, R[k][j]);
                        strcat(temp, ")");

                        if(strlen(R[i][j]) == 0)
                            strcpy(R[i][j], temp);
                        else {
                            strcat(R[i][j], "+");
                            strcat(R[i][j], temp);
                        }
                    }
                }
            }
        }
    }

    // récupérer initial -> final
    int init = -1, fin = -1;

    for(i = 0; i < a->nbEtats; i++) {
        if(strcmp(a->etats[i], a->etatInitial) == 0)
            init = i;
    }

    // prendre premier état final
    for(i = 0; i < a->nbFinaux; i++) {
        for(j = 0; j < a->nbEtats; j++) {
            if(strcmp(a->etats[j], a->etatsFinaux[i]) == 0) {
                fin = j;
                break;
            }
        }
    }

    if(init != -1 && fin != -1)
        return strdup(R[init][fin]);

    return strdup("");
}
/* ---------------- Menu ---------------- */

int main() {
    Automate a, b, res;
    char exp[100], fichier[100], out[100], mot[100], lettre[20];
    int choix;

    initialiserAutomate(&a);

    do {
        printf("\n===== MENU =====\n");
        printf("1. Lire automate\n");
        printf("2. Afficher automate\n");
        printf("3. Generer fichier dot\n");
        printf("4. Etat avec plus de transitions sortantes\n");
        printf("5. Etat avec plus de transitions entrantes\n");
        printf("6. Etats avec une lettre\n");
        printf("7. Tester un mot\n");
        printf("8. Lire fichier mots et enregistrer la liste des mots acceptes\n");
        printf("9. Concatenation\n");
        printf("10. Union\n");
        printf("11. Expression -> automate\n");
        printf("12. Supprimer epsilon\n");
        printf("13. Automate -> Expression reguliere\n");
        printf("14. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choix);

        switch(choix){
            case 1:
                printf("Nom fichier .dot : ");
                scanf("%s", fichier);
                lireAutomate(&a,fichier);
                break;

            case 2:
                afficherAutomate(&a);
                break;

            case 3:
                printf("Nom du fichier a generer : ");
                scanf("%s", fichier);
                genererDot(&a,fichier);
                break;

            case 4:
                etatPlusSortantes(&a);
                break;

            case 5:
                etatPlusEntrantes(&a);
                break;

            case 6:
                printf("Donner la lettre : ");
                scanf("%s", lettre);
                afficherEtatsAvecLettre(&a,lettre);
                break;

            case 7:
                printf("Donner le mot : ");
                scanf("%s", mot);
                if(motReconnu(&a,mot)) printf("Mot reconnu\n");
                else printf("Mot non reconnu\n");
                break;

            case 8:
                printf("Fichier entree : ");
                scanf("%s", fichier);
                printf("Fichier sortie : ");
                scanf("%s", out);
                lireFichierMots(&a,fichier,out);
                break;

           case 9: {
    Automate a1, a2;
    initialiserAutomate(&a1);
    initialiserAutomate(&a2);

    char fichier1[100], fichier2[100];
    printf("Nom du premier automate (.dot) : ");
    scanf("%s", fichier1);
    lireAutomate(&a1, fichier1);

    printf("Nom du deuxieme automate (.dot) : ");
    scanf("%s", fichier2);
    lireAutomate(&a2, fichier2);

    res = concatenation(&a1, &a2);

    libererAutomate(&a1); // libérer automates lus
    libererAutomate(&a2);

    a = res; // res devient l'automate courant
    afficherAutomate(&a);
}
break;

           case 10: { // Union
    Automate a1, a2, resUnion;
    initialiserAutomate(&a1);
    initialiserAutomate(&a2);

    char f1[100], f2[100];
    printf("Nom du premier automate (.dot) : ");
    scanf("%s", f1);
    lireAutomate(&a1, f1);

    printf("Nom du deuxieme automate (.dot) : ");
    scanf("%s", f2);
    lireAutomate(&a2, f2);

    resUnion = unionAutomates(&a1, &a2);

    libererAutomate(&a1); // libérer automates temporaires
    libererAutomate(&a2);

    libererAutomate(&a);   // libérer automate courant si déjà chargé
    a = resUnion;          // résultat devient l'automate courant

    afficherAutomate(&a);
}
break;
            case 11:
                printf("Expression postfixe (ex: ab.+*): ");
                scanf("%s", exp);
                res = expressionToAutomate(exp);
                libererAutomate(&a);
                a = res;
                afficherAutomate(&a);
                break;

            case 12:
                supprimerEpsilon(&a);
                afficherAutomate(&a);
                break;
                case 13:
    printf("Expression reguliere : %s\n", automateToRegex(&a));
    break;

            case 14:
                printf("Fin du programme\n");
                break;

            default:
                printf("Choix invalide\n");
        }

    } while(choix != 14);

    libererAutomate(&a);
    return 0;
}
