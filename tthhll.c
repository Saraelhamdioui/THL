#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ========================================================= */
/* ====================== STRUCTURES ======================= */
/* ========================================================= */

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

/* ========================================================= */
/* ====================== OUTILS BASE ====================== */
/* ========================================================= */

char *dupliquerChaine(const char *s) {
    char *p = (char *)malloc(strlen(s) + 1);
    if (p == NULL) {
        printf("Erreur memoire.\n");
        exit(1);
    }
    strcpy(p, s);
    return p;
}

void libererTableauChaines(char **tab, int nb) {
    int i;
    if (tab == NULL) return;
    for (i = 0; i < nb; i++) {
        free(tab[i]);
    }
    free(tab);
}

void initialiserAutomate(Automate *a) {
    a->etats = NULL;
    a->nbEtats = 0;
    a->capEtats = 0;

    a->alphabet = NULL;
    a->nbAlphabet = 0;
    a->capAlphabet = 0;

    a->transitions = NULL;
    a->nbTransitions = 0;
    a->capTransitions = 0;

    strcpy(a->etatInitial, "");

    a->etatsFinaux = NULL;
    a->nbFinaux = 0;
    a->capFinaux = 0;
}

void libererAutomate(Automate *a) {
    libererTableauChaines(a->etats, a->nbEtats);
    libererTableauChaines(a->alphabet, a->nbAlphabet);
    libererTableauChaines(a->etatsFinaux, a->nbFinaux);
    free(a->transitions);

    a->etats = NULL;
    a->alphabet = NULL;
    a->etatsFinaux = NULL;
    a->transitions = NULL;

    a->nbEtats = 0;
    a->nbAlphabet = 0;
    a->nbTransitions = 0;
    a->nbFinaux = 0;

    a->capEtats = 0;
    a->capAlphabet = 0;
    a->capTransitions = 0;
    a->capFinaux = 0;

    strcpy(a->etatInitial, "");
}

void reserverEtats(Automate *a) {
    int nouvelleCap;
    char **nv;

    if (a->nbEtats < a->capEtats) return;

    nouvelleCap = (a->capEtats == 0) ? 4 : a->capEtats * 2;
    nv = (char **)realloc(a->etats, nouvelleCap * sizeof(char *));
    if (nv == NULL) {
        printf("Erreur memoire pour les etats.\n");
        exit(1);
    }

    a->etats = nv;
    a->capEtats = nouvelleCap;
}

void reserverAlphabet(Automate *a) {
    int nouvelleCap;
    char **nv;

    if (a->nbAlphabet < a->capAlphabet) return;

    nouvelleCap = (a->capAlphabet == 0) ? 4 : a->capAlphabet * 2;
    nv = (char **)realloc(a->alphabet, nouvelleCap * sizeof(char *));
    if (nv == NULL) {
        printf("Erreur memoire pour l'alphabet.\n");
        exit(1);
    }

    a->alphabet = nv;
    a->capAlphabet = nouvelleCap;
}

void reserverTransitions(Automate *a) {
    int nouvelleCap;
    Transition *nv;

    if (a->nbTransitions < a->capTransitions) return;

    nouvelleCap = (a->capTransitions == 0) ? 4 : a->capTransitions * 2;
    nv = (Transition *)realloc(a->transitions, nouvelleCap * sizeof(Transition));
    if (nv == NULL) {
        printf("Erreur memoire pour les transitions.\n");
        exit(1);
    }

    a->transitions = nv;
    a->capTransitions = nouvelleCap;
}

void reserverFinaux(Automate *a) {
    int nouvelleCap;
    char **nv;

    if (a->nbFinaux < a->capFinaux) return;

    nouvelleCap = (a->capFinaux == 0) ? 4 : a->capFinaux * 2;
    nv = (char **)realloc(a->etatsFinaux, nouvelleCap * sizeof(char *));
    if (nv == NULL) {
        printf("Erreur memoire pour les etats finaux.\n");
        exit(1);
    }

    a->etatsFinaux = nv;
    a->capFinaux = nouvelleCap;
}

int existeEtat(Automate *a, const char *etat) {
    int i;
    for (i = 0; i < a->nbEtats; i++) {
        if (strcmp(a->etats[i], etat) == 0) return 1;
    }
    return 0;
}

int existeSymbole(Automate *a, const char *symbole) {
    int i;
    for (i = 0; i < a->nbAlphabet; i++) {
        if (strcmp(a->alphabet[i], symbole) == 0) return 1;
    }
    return 0;
}

int estFinal(Automate *a, const char *etat) {
    int i;
    for (i = 0; i < a->nbFinaux; i++) {
        if (strcmp(a->etatsFinaux[i], etat) == 0) return 1;
    }
    return 0;
}

void ajouterEtat(Automate *a, const char *etat) {
    if (!existeEtat(a, etat)) {
        reserverEtats(a);
        a->etats[a->nbEtats++] = dupliquerChaine(etat);
    }
}

void ajouterSymbole(Automate *a, const char *symbole) {
    if (!existeSymbole(a, symbole) &&
        strcmp(symbole, "e") != 0 &&
        strcmp(symbole, "ε") != 0) {
        reserverAlphabet(a);
        a->alphabet[a->nbAlphabet++] = dupliquerChaine(symbole);
    }
}

void ajouterEtatFinal(Automate *a, const char *etat) {
    if (!estFinal(a, etat)) {
        reserverFinaux(a);
        a->etatsFinaux[a->nbFinaux++] = dupliquerChaine(etat);
    }
}

int transitionExiste(Automate *a, const char *dep, const char *symb, const char *arr) {
    int i;
    for (i = 0; i < a->nbTransitions; i++) {
        if (strcmp(a->transitions[i].depart, dep) == 0 &&
            strcmp(a->transitions[i].symbole, symb) == 0 &&
            strcmp(a->transitions[i].arrivee, arr) == 0) {
            return 1;
        }
    }
    return 0;
}

void ajouterTransition(Automate *a, const char *dep, const char *symb, const char *arr) {
    if (!transitionExiste(a, dep, symb, arr)) {
        reserverTransitions(a);

        strcpy(a->transitions[a->nbTransitions].depart, dep);
        strcpy(a->transitions[a->nbTransitions].symbole, symb);
        strcpy(a->transitions[a->nbTransitions].arrivee, arr);
        a->nbTransitions++;

        ajouterEtat(a, dep);
        ajouterEtat(a, arr);
        ajouterSymbole(a, symb);
    }
}

int indiceEtat(Automate *a, const char *etat) {
    int i;
    for (i = 0; i < a->nbEtats; i++) {
        if (strcmp(a->etats[i], etat) == 0) return i;
    }
    return -1;
}

void copierAutomate(Automate *src, Automate *dest) {
    int i;

    initialiserAutomate(dest);

    for (i = 0; i < src->nbEtats; i++) ajouterEtat(dest, src->etats[i]);
    for (i = 0; i < src->nbAlphabet; i++) ajouterSymbole(dest, src->alphabet[i]);
    for (i = 0; i < src->nbTransitions; i++) {
        ajouterTransition(dest,
                          src->transitions[i].depart,
                          src->transitions[i].symbole,
                          src->transitions[i].arrivee);
    }
    strcpy(dest->etatInitial, src->etatInitial);
    for (i = 0; i < src->nbFinaux; i++) ajouterEtatFinal(dest, src->etatsFinaux[i]);
}

/* ========================================================= */
/* ====================== PARTIE 1 ========================= */
/* ========================================================= */

void lireAutomate(Automate *a, const char *nomFichier) {
    FILE *f;
    char ligne[256];

    f = fopen(nomFichier, "r");
    if (f == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", nomFichier);
        return;
    }

    libererAutomate(a);
    initialiserAutomate(a);

    while (fgets(ligne, sizeof(ligne), f)) {
        char *p;

        if (strstr(ligne, "init ->") != NULL) {
            char init[20];
            if (sscanf(ligne, " %*s -> %19[^; \n\r]", init) == 1) {
                strcpy(a->etatInitial, init);
                ajouterEtat(a, init);
            }
        }
        else if (strstr(ligne, "shape=doublecircle") != NULL) {
            char fin[20];
            if (sscanf(ligne, " %19s", fin) == 1) {
                ajouterEtat(a, fin);
                ajouterEtatFinal(a, fin);
            }
        }
        else if (strstr(ligne, "label=") != NULL) {
            char dep[20], arr[20], symb[20];

            if (sscanf(ligne, " %19s -> %19s", dep, arr) == 2) {
                p = strchr(arr, '[');
                if (p) *p = '\0';
                p = strchr(arr, ';');
                if (p) *p = '\0';

                p = strstr(ligne, "label=\"");
                if (p != NULL) {
                    p += 7;
                    sscanf(p, "%19[^\"]", symb);
                    ajouterTransition(a, dep, symb, arr);
                }
            }
        }
    }

    fclose(f);
    printf("Automate charge avec succes.\n");
}

void afficherAutomate(Automate *a) {
    int i;

    printf("\n===== INFORMATIONS DE L'AUTOMATE =====\n");

    printf("Etats : ");
    for (i = 0; i < a->nbEtats; i++) printf("%s ", a->etats[i]);
    printf("\n");

    printf("Alphabet : ");
    for (i = 0; i < a->nbAlphabet; i++) printf("%s ", a->alphabet[i]);
    printf("\n");

    printf("Etat initial : %s\n", a->etatInitial);

    printf("Etats finaux : ");
    for (i = 0; i < a->nbFinaux; i++) printf("%s ", a->etatsFinaux[i]);
    printf("\n");

    printf("Transitions :\n");
    for (i = 0; i < a->nbTransitions; i++) {
        printf("  %s --%s--> %s\n",
               a->transitions[i].depart,
               a->transitions[i].symbole,
               a->transitions[i].arrivee);
    }

    printf("======================================\n");
}

/* ========================================================= */
/* ====================== PARTIE 2 ========================= */
/* ========================================================= */

void genererDot(Automate *a, const char *nomFichier) {
    FILE *f;
    int i;

    f = fopen(nomFichier, "w");
    if (f == NULL) {
        printf("Erreur : impossible de creer le fichier %s\n", nomFichier);
        return;
    }

    fprintf(f, "digraph automate {\n");
    fprintf(f, "    rankdir=LR;\n");
    fprintf(f, "    init [shape=point];\n");
    fprintf(f, "    init -> %s;\n", a->etatInitial);

    for (i = 0; i < a->nbFinaux; i++) {
        fprintf(f, "    %s [shape=doublecircle];\n", a->etatsFinaux[i]);
    }

    for (i = 0; i < a->nbTransitions; i++) {
        fprintf(f, "    %s -> %s [label=\"%s\"];\n",
                a->transitions[i].depart,
                a->transitions[i].arrivee,
                a->transitions[i].symbole);
    }

    fprintf(f, "}\n");
    fclose(f);

    printf("Fichier .dot genere avec succes.\n");
}

void etatPlusSortantes(Automate *a) {
    int i, j, nb, max = -1;
    char meilleur[20] = "";

    for (i = 0; i < a->nbEtats; i++) {
        nb = 0;
        for (j = 0; j < a->nbTransitions; j++) {
            if (strcmp(a->etats[i], a->transitions[j].depart) == 0) nb++;
        }
        if (nb > max) {
            max = nb;
            strcpy(meilleur, a->etats[i]);
        }
    }

    if (max >= 0)
        printf("Etat avec le plus de transitions sortantes : %s (%d)\n", meilleur, max);
    else
        printf("Aucun etat.\n");
}

void etatPlusEntrantes(Automate *a) {
    int i, j, nb, max = -1;
    char meilleur[20] = "";

    for (i = 0; i < a->nbEtats; i++) {
        nb = 0;
        for (j = 0; j < a->nbTransitions; j++) {
            if (strcmp(a->etats[i], a->transitions[j].arrivee) == 0) nb++;
        }
        if (nb > max) {
            max = nb;
            strcpy(meilleur, a->etats[i]);
        }
    }

    if (max >= 0)
        printf("Etat avec le plus de transitions entrantes : %s (%d)\n", meilleur, max);
    else
        printf("Aucun etat.\n");
}

void afficherEtatsAvecLettre(Automate *a, const char *lettre) {
    int i, j, trouveGlobal = 0;

    printf("Etats ayant une transition sortante etiquetee par %s : ", lettre);

    for (i = 0; i < a->nbEtats; i++) {
        int trouve = 0;
        for (j = 0; j < a->nbTransitions; j++) {
            if (strcmp(a->etats[i], a->transitions[j].depart) == 0 &&
                strcmp(lettre, a->transitions[j].symbole) == 0) {
                trouve = 1;
                trouveGlobal = 1;
                break;
            }
        }
        if (trouve) printf("%s ", a->etats[i]);
    }

    if (!trouveGlobal) printf("aucun");
    printf("\n");
}

int motReconnu(Automate *a, const char *mot) {
    char courant[20];
    char symbole[2];
    int i, j, trouve;

    strcpy(courant, a->etatInitial);

    for (i = 0; mot[i] != '\0'; i++) {
        symbole[0] = mot[i];
        symbole[1] = '\0';
        trouve = 0;

        for (j = 0; j < a->nbTransitions; j++) {
            if (strcmp(a->transitions[j].depart, courant) == 0 &&
                strcmp(a->transitions[j].symbole, symbole) == 0) {
                strcpy(courant, a->transitions[j].arrivee);
                trouve = 1;
                break;
            }
        }

        if (!trouve) return 0;
    }

    return estFinal(a, courant);
}

void lireFichierMots(Automate *a, const char *fichierIn, const char *fichierOut) {
    FILE *fin, *fout;
    char mot[200];

    fin = fopen(fichierIn, "r");
    fout = fopen(fichierOut, "w");

    if (fin == NULL || fout == NULL) {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        if (fin != NULL) fclose(fin);
        if (fout != NULL) fclose(fout);
        return;
    }

    while (fscanf(fin, "%199s", mot) == 1) {
        if (motReconnu(a, mot)) fprintf(fout, "%s\n", mot);
    }

    fclose(fin);
    fclose(fout);

    printf("Les mots reconnus ont ete enregistres dans %s\n", fichierOut);
}

/* ========================================================= */
/* ====================== PARTIE 3 ========================= */
/* ========================================================= */

int compteurEtatsAuto = 0;

void nouveauNomEtat(char *nom) {
    sprintf(nom, "E%d", compteurEtatsAuto++);
}

void copierAvecPrefixe(Automate *src, Automate *dest, const char *prefixe) {
    int i;
    char dep[50], arr[50], etat[50];

    for (i = 0; i < src->nbEtats; i++) {
        sprintf(etat, "%s%s", prefixe, src->etats[i]);
        ajouterEtat(dest, etat);
    }

    for (i = 0; i < src->nbAlphabet; i++) {
        ajouterSymbole(dest, src->alphabet[i]);
    }

    for (i = 0; i < src->nbTransitions; i++) {
        sprintf(dep, "%s%s", prefixe, src->transitions[i].depart);
        sprintf(arr, "%s%s", prefixe, src->transitions[i].arrivee);
        ajouterTransition(dest, dep, src->transitions[i].symbole, arr);
    }
}

Automate concatenationAutomates(Automate *a1, Automate *a2) {
    Automate res;
    int i;
    char e1[50], e2[50];

    initialiserAutomate(&res);

    copierAvecPrefixe(a1, &res, "A1_");
    copierAvecPrefixe(a2, &res, "A2_");

    sprintf(res.etatInitial, "A1_%s", a1->etatInitial);

    for (i = 0; i < a2->nbFinaux; i++) {
        sprintf(e2, "A2_%s", a2->etatsFinaux[i]);
        ajouterEtatFinal(&res, e2);
    }

    for (i = 0; i < a1->nbFinaux; i++) {
        sprintf(e1, "A1_%s", a1->etatsFinaux[i]);
        sprintf(e2, "A2_%s", a2->etatInitial);
        ajouterTransition(&res, e1, "e", e2);
    }

    return res;
}

Automate unionAutomates(Automate *a1, Automate *a2) {
    Automate res;
    int i;
    char init[20], fin[20], tmp[50];

    initialiserAutomate(&res);

    copierAvecPrefixe(a1, &res, "U1_");
    copierAvecPrefixe(a2, &res, "U2_");

    nouveauNomEtat(init);
    nouveauNomEtat(fin);

    ajouterEtat(&res, init);
    ajouterEtat(&res, fin);

    strcpy(res.etatInitial, init);
    ajouterEtatFinal(&res, fin);

    sprintf(tmp, "U1_%s", a1->etatInitial);
    ajouterTransition(&res, init, "e", tmp);

    sprintf(tmp, "U2_%s", a2->etatInitial);
    ajouterTransition(&res, init, "e", tmp);

    for (i = 0; i < a1->nbFinaux; i++) {
        sprintf(tmp, "U1_%s", a1->etatsFinaux[i]);
        ajouterTransition(&res, tmp, "e", fin);
    }

    for (i = 0; i < a2->nbFinaux; i++) {
        sprintf(tmp, "U2_%s", a2->etatsFinaux[i]);
        ajouterTransition(&res, tmp, "e", fin);
    }

    return res;
}

/* ---------- REGEX -> AUTOMATE ---------- */

Automate automateSymbole(const char *symbole) {
    Automate a;
    char s0[20], s1[20];

    initialiserAutomate(&a);

    nouveauNomEtat(s0);
    nouveauNomEtat(s1);

    ajouterEtat(&a, s0);
    ajouterEtat(&a, s1);

    strcpy(a.etatInitial, s0);
    ajouterEtatFinal(&a, s1);

    ajouterTransition(&a, s0, symbole, s1);

    return a;
}

Automate etoileAutomate(Automate *a) {
    Automate res;
    int i;
    char init[20], fin[20], tmp[50], oldInit[50];

    initialiserAutomate(&res);
    copierAvecPrefixe(a, &res, "K_");

    nouveauNomEtat(init);
    nouveauNomEtat(fin);

    ajouterEtat(&res, init);
    ajouterEtat(&res, fin);

    strcpy(res.etatInitial, init);
    ajouterEtatFinal(&res, fin);

    sprintf(oldInit, "K_%s", a->etatInitial);

    ajouterTransition(&res, init, "e", oldInit);
    ajouterTransition(&res, init, "e", fin);

    for (i = 0; i < a->nbFinaux; i++) {
        sprintf(tmp, "K_%s", a->etatsFinaux[i]);
        ajouterTransition(&res, tmp, "e", fin);
        ajouterTransition(&res, tmp, "e", oldInit);
    }

    return res;
}

int estSymboleRegex(char c) {
    return isalnum((unsigned char)c);
}

int fautConcat(char c1, char c2) {
    int g = estSymboleRegex(c1) || c1 == ')' || c1 == '*';
    int d = estSymboleRegex(c2) || c2 == '(';
    return g && d;
}

void ajouterConcatExplicite(const char *infixe, char *res) {
    int i, j = 0;
    int n = (int)strlen(infixe);

    for (i = 0; i < n; i++) {
        if (infixe[i] == ' ') continue;

        res[j++] = infixe[i];

        if (i < n - 1) {
            char c1 = infixe[i];
            char c2 = infixe[i + 1];
            if (c2 != ' ' && fautConcat(c1, c2)) {
                res[j++] = '.';
            }
        }
    }
    res[j] = '\0';
}

int priorite(char op) {
    if (op == '*') return 3;
    if (op == '.') return 2;
    if (op == '|') return 1;
    return 0;
}

void infixeVersPostfixe(const char *infixe, char *postfixe) {
    char pile[500];
    int top = -1;
    int i, j = 0;
    char c;

    for (i = 0; infixe[i] != '\0'; i++) {
        c = infixe[i];

        if (estSymboleRegex(c)) {
            postfixe[j++] = c;
        }
        else if (c == '(') {
            pile[++top] = c;
        }
        else if (c == ')') {
            while (top >= 0 && pile[top] != '(') {
                postfixe[j++] = pile[top--];
            }
            if (top >= 0) top--;
        }
        else if (c == '*' || c == '.' || c == '|') {
            while (top >= 0 && priorite(pile[top]) >= priorite(c)) {
                postfixe[j++] = pile[top--];
            }
            pile[++top] = c;
        }
    }

    while (top >= 0) postfixe[j++] = pile[top--];
    postfixe[j] = '\0';
}

Automate expressionVersAutomate(const char *expression) {
    char exprAvecConcat[500];
    char postfixe[500];
    Automate pile[200];
    int top = -1;
    int i;
    char symb[2];
    Automate a1, a2, res;

    ajouterConcatExplicite(expression, exprAvecConcat);
    infixeVersPostfixe(exprAvecConcat, postfixe);

    for (i = 0; postfixe[i] != '\0'; i++) {
        if (estSymboleRegex(postfixe[i])) {
            symb[0] = postfixe[i];
            symb[1] = '\0';
            pile[++top] = automateSymbole(symb);
        }
        else if (postfixe[i] == '.') {
            a2 = pile[top--];
            a1 = pile[top--];
            res = concatenationAutomates(&a1, &a2);
            pile[++top] = res;
        }
        else if (postfixe[i] == '|') {
            a2 = pile[top--];
            a1 = pile[top--];
            res = unionAutomates(&a1, &a2);
            pile[++top] = res;
        }
        else if (postfixe[i] == '*') {
            a1 = pile[top--];
            res = etoileAutomate(&a1);
            pile[++top] = res;
        }
    }

    return pile[top];
}

/* ---------- SUPPRESSION EPSILON ---------- */

void fermetureEpsilon(Automate *a, int idxEtat, int *fermeture) {
    int *pile;
    int top = -1;
    int i, idxArr;

    pile = (int *)malloc(a->nbEtats * sizeof(int));
    if (pile == NULL) {
        printf("Erreur memoire.\n");
        exit(1);
    }

    for (i = 0; i < a->nbEtats; i++) fermeture[i] = 0;

    fermeture[idxEtat] = 1;
    pile[++top] = idxEtat;

    while (top >= 0) {
        int courant = pile[top--];

        for (i = 0; i < a->nbTransitions; i++) {
            if (strcmp(a->transitions[i].depart, a->etats[courant]) == 0 &&
                (strcmp(a->transitions[i].symbole, "e") == 0 ||
                 strcmp(a->transitions[i].symbole, "ε") == 0)) {
                idxArr = indiceEtat(a, a->transitions[i].arrivee);
                if (idxArr != -1 && !fermeture[idxArr]) {
                    fermeture[idxArr] = 1;
                    pile[++top] = idxArr;
                }
            }
        }
    }

    free(pile);
}

Automate supprimerEpsilonTransitions(Automate *a) {
    Automate res;
    int i, j, k, t, m;
    int *fermP, *fermQ;

    initialiserAutomate(&res);

    for (i = 0; i < a->nbEtats; i++) ajouterEtat(&res, a->etats[i]);
    for (i = 0; i < a->nbAlphabet; i++) ajouterSymbole(&res, a->alphabet[i]);

    strcpy(res.etatInitial, a->etatInitial);

    fermP = (int *)malloc(a->nbEtats * sizeof(int));
    fermQ = (int *)malloc(a->nbEtats * sizeof(int));
    if (fermP == NULL || fermQ == NULL) {
        printf("Erreur memoire.\n");
        exit(1);
    }

    for (i = 0; i < a->nbEtats; i++) {
        fermetureEpsilon(a, i, fermP);
        for (j = 0; j < a->nbEtats; j++) {
            if (fermP[j] && estFinal(a, a->etats[j])) {
                ajouterEtatFinal(&res, a->etats[i]);
                break;
            }
        }
    }

    for (i = 0; i < a->nbEtats; i++) {
        fermetureEpsilon(a, i, fermP);

        for (j = 0; j < a->nbAlphabet; j++) {
            for (k = 0; k < a->nbEtats; k++) {
                if (fermP[k]) {
                    for (t = 0; t < a->nbTransitions; t++) {
                        if (strcmp(a->transitions[t].depart, a->etats[k]) == 0 &&
                            strcmp(a->transitions[t].symbole, a->alphabet[j]) == 0) {

                            int idxArr = indiceEtat(a, a->transitions[t].arrivee);
                            if (idxArr != -1) {
                                fermetureEpsilon(a, idxArr, fermQ);

                                for (m = 0; m < a->nbEtats; m++) {
                                    if (fermQ[m]) {
                                        ajouterTransition(&res,
                                                          a->etats[i],
                                                          a->alphabet[j],
                                                          a->etats[m]);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    free(fermP);
    free(fermQ);

    return res;
}

/* ---------- AUTOMATE -> REGEX ---------- */

char *copieStr(const char *s) {
    return dupliquerChaine(s);
}

int estVideRegex(const char *s) {
    return (s == NULL || s[0] == '\0');
}

char *regexUnion(const char *r1, const char *r2) {
    char buffer[4000];

    if (estVideRegex(r1) && estVideRegex(r2)) return NULL;
    if (estVideRegex(r1)) return copieStr(r2);
    if (estVideRegex(r2)) return copieStr(r1);
    if (strcmp(r1, r2) == 0) return copieStr(r1);

    snprintf(buffer, sizeof(buffer), "(%s|%s)", r1, r2);
    return copieStr(buffer);
}

char *regexConcat(const char *r1, const char *r2) {
    char buffer[4000];

    if (estVideRegex(r1) || estVideRegex(r2)) return NULL;
    if (strcmp(r1, "e") == 0) return copieStr(r2);
    if (strcmp(r2, "e") == 0) return copieStr(r1);

    snprintf(buffer, sizeof(buffer), "(%s%s)", r1, r2);
    return copieStr(buffer);
}

char *regexStar(const char *r) {
    char buffer[4000];

    if (estVideRegex(r)) return copieStr("e");
    if (strcmp(r, "e") == 0) return copieStr("e");

    snprintf(buffer, sizeof(buffer), "(%s)*", r);
    return copieStr(buffer);
}

void libererMatriceRegex(char ***R, int n) {
    int i, j;
    if (R == NULL) return;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) free(R[i][j]);
        free(R[i]);
    }
    free(R);
}

char *automateVersExpressionReguliere(Automate *a) {
    int i, j, k;
    int n = a->nbEtats + 2;
    int start = 0;
    int end = n - 1;

    char **noms;
    char ***R;

    noms = (char **)malloc(n * sizeof(char *));
    R = (char ***)malloc(n * sizeof(char **));
    if (noms == NULL || R == NULL) {
        printf("Erreur memoire.\n");
        exit(1);
    }

    noms[0] = copieStr("INIT_GLOBAL");
    for (i = 0; i < a->nbEtats; i++) noms[i + 1] = copieStr(a->etats[i]);
    noms[n - 1] = copieStr("FINAL_GLOBAL");

    for (i = 0; i < n; i++) {
        R[i] = (char **)malloc(n * sizeof(char *));
        if (R[i] == NULL) {
            printf("Erreur memoire.\n");
            exit(1);
        }
        for (j = 0; j < n; j++) R[i][j] = NULL;
    }

    i = start;
    j = indiceEtat(a, a->etatInitial) + 1;
    R[i][j] = regexUnion(R[i][j], "e");

    for (i = 0; i < a->nbFinaux; i++) {
        int idx = indiceEtat(a, a->etatsFinaux[i]);
        if (idx != -1) {
            R[idx + 1][end] = regexUnion(R[idx + 1][end], "e");
        }
    }

    for (i = 0; i < a->nbTransitions; i++) {
        int d = indiceEtat(a, a->transitions[i].depart) + 1;
        int ar = indiceEtat(a, a->transitions[i].arrivee) + 1;
        R[d][ar] = regexUnion(R[d][ar], a->transitions[i].symbole);
    }

    for (k = 1; k <= a->nbEtats; k++) {
        char *RkkStar = regexStar(R[k][k]);

        for (i = 0; i < n; i++) {
            if (i == k) continue;
            for (j = 0; j < n; j++) {
                char *part1, *part2, *tempUnion;

                if (j == k) continue;
                if (estVideRegex(R[i][k]) || estVideRegex(R[k][j])) continue;

                part1 = regexConcat(R[i][k], RkkStar);
                part2 = regexConcat(part1, R[k][j]);
                free(part1);

                tempUnion = regexUnion(R[i][j], part2);
                free(R[i][j]);
                R[i][j] = tempUnion;

                free(part2);
            }
        }

        for (i = 0; i < n; i++) {
            if (i != k) {
                free(R[i][k]);
                R[i][k] = NULL;
                free(R[k][i]);
                R[k][i] = NULL;
            }
        }

        free(R[k][k]);
        R[k][k] = NULL;
        free(RkkStar);
    }

    {
        char *resultat;
        if (R[start][end] == NULL) resultat = copieStr("∅");
        else resultat = copieStr(R[start][end]);

        libererMatriceRegex(R, n);
        for (i = 0; i < n; i++) free(noms[i]);
        free(noms);

        return resultat;
    }
}

/* ========================================================= */
/* ====================== PARTIE 4 ========================= */
/* ========================================================= */

/* ---------- OUTILS POUR DETERMINISATION ---------- */

int contientEtatDansListe(char **liste, int nb, const char *etat) {
    int i;
    for (i = 0; i < nb; i++) {
        if (strcmp(liste[i], etat) == 0) return 1;
    }
    return 0;
}

void ajouterEtatDansListe(char ***liste, int *nb, const char *etat) {
    char **nv = (char **)realloc(*liste, (*nb + 1) * sizeof(char *));
    if (nv == NULL) {
        printf("Erreur memoire.\n");
        exit(1);
    }
    *liste = nv;
    (*liste)[*nb] = dupliquerChaine(etat);
    (*nb)++;
}

void libererListeEtatsTemp(char **liste, int nb) {
    int i;
    if (liste == NULL) return;
    for (i = 0; i < nb; i++) free(liste[i]);
    free(liste);
}

void trierListeEtats(char **liste, int nb) {
    int i, j;
    for (i = 0; i < nb - 1; i++) {
        for (j = i + 1; j < nb; j++) {
            if (strcmp(liste[i], liste[j]) > 0) {
                char *tmp = liste[i];
                liste[i] = liste[j];
                liste[j] = tmp;
            }
        }
    }
}

void nomEtatCompose(char *dest, char **liste, int nb) {
    int i;
    dest[0] = '\0';

    if (nb == 0) {
        strcpy(dest, "VIDE");
        return;
    }

    trierListeEtats(liste, nb);

    for (i = 0; i < nb; i++) {
        strcat(dest, liste[i]);
        if (i < nb - 1) strcat(dest, "|");
    }
}

int estFinalDansListe(Automate *a, char **liste, int nb) {
    int i;
    for (i = 0; i < nb; i++) {
        if (estFinal(a, liste[i])) return 1;
    }
    return 0;
}

void fermetureEpsilonDepuisListe(Automate *a, char **entree, int nbEntree, char ***sortie, int *nbSortie) {
    int changement = 1;
    int i, j;

    *sortie = NULL;
    *nbSortie = 0;

    for (i = 0; i < nbEntree; i++) {
        if (!contientEtatDansListe(*sortie, *nbSortie, entree[i])) {
            ajouterEtatDansListe(sortie, nbSortie, entree[i]);
        }
    }

    while (changement) {
        changement = 0;

        for (i = 0; i < *nbSortie; i++) {
            for (j = 0; j < a->nbTransitions; j++) {
                if (strcmp(a->transitions[j].depart, (*sortie)[i]) == 0 &&
                    (strcmp(a->transitions[j].symbole, "e") == 0 ||
                     strcmp(a->transitions[j].symbole, "ε") == 0)) {

                    if (!contientEtatDansListe(*sortie, *nbSortie, a->transitions[j].arrivee)) {
                        ajouterEtatDansListe(sortie, nbSortie, a->transitions[j].arrivee);
                        changement = 1;
                    }
                }
            }
        }
    }
}

void deplacerParSymbole(Automate *a, char **entree, int nbEntree, const char *symbole, char ***sortie, int *nbSortie) {
    int i, j;

    *sortie = NULL;
    *nbSortie = 0;

    for (i = 0; i < nbEntree; i++) {
        for (j = 0; j < a->nbTransitions; j++) {
            if (strcmp(a->transitions[j].depart, entree[i]) == 0 &&
                strcmp(a->transitions[j].symbole, symbole) == 0) {

                if (!contientEtatDansListe(*sortie, *nbSortie, a->transitions[j].arrivee)) {
                    ajouterEtatDansListe(sortie, nbSortie, a->transitions[j].arrivee);
                }
            }
        }
    }
}

void decomposerNomEtatCompose(const char *nom, char ***liste, int *nb) {
    char copie[500];
    char *token;

    *liste = NULL;
    *nb = 0;

    if (strcmp(nom, "VIDE") == 0) return;

    strcpy(copie, nom);
    token = strtok(copie, "|");
    while (token != NULL) {
        ajouterEtatDansListe(liste, nb, token);
        token = strtok(NULL, "|");
    }
}

/* 14. Produit = intersection */
Automate produitAutomates(Automate *a1, Automate *a2) {
    Automate res;
    int i, j;
    char etat[100], dep[100], arr[100];

    initialiserAutomate(&res);

    for (i = 0; i < a1->nbEtats; i++) {
        for (j = 0; j < a2->nbEtats; j++) {
            sprintf(etat, "(%s,%s)", a1->etats[i], a2->etats[j]);
            ajouterEtat(&res, etat);
        }
    }

    sprintf(res.etatInitial, "(%s,%s)", a1->etatInitial, a2->etatInitial);

    for (i = 0; i < a1->nbFinaux; i++) {
        for (j = 0; j < a2->nbFinaux; j++) {
            sprintf(etat, "(%s,%s)", a1->etatsFinaux[i], a2->etatsFinaux[j]);
            ajouterEtatFinal(&res, etat);
        }
    }

    for (i = 0; i < a1->nbTransitions; i++) {
        for (j = 0; j < a2->nbTransitions; j++) {
            if (strcmp(a1->transitions[i].symbole, a2->transitions[j].symbole) == 0 &&
                strcmp(a1->transitions[i].symbole, "e") != 0 &&
                strcmp(a1->transitions[i].symbole, "ε") != 0) {

                sprintf(dep, "(%s,%s)", a1->transitions[i].depart, a2->transitions[j].depart);
                sprintf(arr, "(%s,%s)", a1->transitions[i].arrivee, a2->transitions[j].arrivee);

                ajouterTransition(&res, dep, a1->transitions[i].symbole, arr);
            }
        }
    }

    return res;
}

/* 15. Déterminisation */
Automate determiniserAutomate(Automate *a) {
    Automate res;
    char **etatInitListe = NULL, **fermInit = NULL;
    int nbEtatInitListe = 0, nbFermInit = 0;
    char nomInit[500];

    char **aTraiter = NULL;
    int nbATraiter = 0;
    int indiceTraitement = 0;

    initialiserAutomate(&res);

    ajouterEtatDansListe(&etatInitListe, &nbEtatInitListe, a->etatInitial);
    fermetureEpsilonDepuisListe(a, etatInitListe, nbEtatInitListe, &fermInit, &nbFermInit);
    nomEtatCompose(nomInit, fermInit, nbFermInit);

    ajouterEtat(&res, nomInit);
    strcpy(res.etatInitial, nomInit);
    if (estFinalDansListe(a, fermInit, nbFermInit)) ajouterEtatFinal(&res, nomInit);

    ajouterEtatDansListe(&aTraiter, &nbATraiter, nomInit);

    while (indiceTraitement < nbATraiter) {
        char *etatCourantNom = aTraiter[indiceTraitement];
        char **listeCourante = NULL;
        int nbListeCourante = 0;
        int i;

        decomposerNomEtatCompose(etatCourantNom, &listeCourante, &nbListeCourante);

        for (i = 0; i < a->nbAlphabet; i++) {
            char **move = NULL, **ferm = NULL;
            int nbMove = 0, nbFerm = 0;
            char nomDest[500];

            deplacerParSymbole(a, listeCourante, nbListeCourante, a->alphabet[i], &move, &nbMove);

            if (nbMove == 0) {
                libererListeEtatsTemp(move, nbMove);
                continue;
            }

            fermetureEpsilonDepuisListe(a, move, nbMove, &ferm, &nbFerm);
            nomEtatCompose(nomDest, ferm, nbFerm);

            if (!existeEtat(&res, nomDest)) {
                ajouterEtat(&res, nomDest);
                if (estFinalDansListe(a, ferm, nbFerm)) ajouterEtatFinal(&res, nomDest);
                ajouterEtatDansListe(&aTraiter, &nbATraiter, nomDest);
            }

            ajouterTransition(&res, etatCourantNom, a->alphabet[i], nomDest);

            libererListeEtatsTemp(move, nbMove);
            libererListeEtatsTemp(ferm, nbFerm);
        }

        libererListeEtatsTemp(listeCourante, nbListeCourante);
        indiceTraitement++;
    }

    libererListeEtatsTemp(etatInitListe, nbEtatInitListe);
    libererListeEtatsTemp(fermInit, nbFermInit);
    libererListeEtatsTemp(aTraiter, nbATraiter);

    return res;
}

int destinationUnique(Automate *a, const char *etat, const char *symbole, char *dest) {
    int i;
    for (i = 0; i < a->nbTransitions; i++) {
        if (strcmp(a->transitions[i].depart, etat) == 0 &&
            strcmp(a->transitions[i].symbole, symbole) == 0) {
            strcpy(dest, a->transitions[i].arrivee);
            return 1;
        }
    }
    return 0;
}

Automate automateAccessible(Automate *a) {
    Automate res;
    int *visite;
    int changement = 1;
    int i, j;

    initialiserAutomate(&res);

    visite = (int *)calloc(a->nbEtats, sizeof(int));
    if (visite == NULL) {
        printf("Erreur memoire.\n");
        exit(1);
    }

    {
        int idxInit = indiceEtat(a, a->etatInitial);
        if (idxInit != -1) visite[idxInit] = 1;
    }

    while (changement) {
        changement = 0;
        for (i = 0; i < a->nbTransitions; i++) {
            int d = indiceEtat(a, a->transitions[i].depart);
            int ar = indiceEtat(a, a->transitions[i].arrivee);
            if (d != -1 && ar != -1 && visite[d] && !visite[ar]) {
                visite[ar] = 1;
                changement = 1;
            }
        }
    }

    for (i = 0; i < a->nbEtats; i++) {
        if (visite[i]) ajouterEtat(&res, a->etats[i]);
    }

    for (i = 0; i < a->nbAlphabet; i++) ajouterSymbole(&res, a->alphabet[i]);

    strcpy(res.etatInitial, a->etatInitial);

    for (i = 0; i < a->nbFinaux; i++) {
        int idx = indiceEtat(a, a->etatsFinaux[i]);
        if (idx != -1 && visite[idx]) ajouterEtatFinal(&res, a->etatsFinaux[i]);
    }

    for (i = 0; i < a->nbTransitions; i++) {
        int d = indiceEtat(a, a->transitions[i].depart);
        int ar = indiceEtat(a, a->transitions[i].arrivee);
        if (d != -1 && ar != -1 && visite[d] && visite[ar]) {
            ajouterTransition(&res,
                              a->transitions[i].depart,
                              a->transitions[i].symbole,
                              a->transitions[i].arrivee);
        }
    }

    free(visite);
    return res;
}

/* ---------- 16. Moore ---------- */

int signaturesEgales(int *a, int *b, int taille) {
    int i;
    for (i = 0; i < taille; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

Automate minimiserAutomateMoore(Automate *a) {
    Automate dfa, accessible, res;
    int *partition, *nouvellePartition;
    int **signatures;
    int i, j, k, nbClasses, change;

    dfa = determiniserAutomate(a);
    accessible = automateAccessible(&dfa);

    initialiserAutomate(&res);

    partition = (int *)malloc(accessible.nbEtats * sizeof(int));
    nouvellePartition = (int *)malloc(accessible.nbEtats * sizeof(int));
    signatures = (int **)malloc(accessible.nbEtats * sizeof(int *));
    if (partition == NULL || nouvellePartition == NULL || signatures == NULL) {
        printf("Erreur memoire.\n");
        exit(1);
    }

    for (i = 0; i < accessible.nbEtats; i++) {
        signatures[i] = (int *)malloc((accessible.nbAlphabet + 1) * sizeof(int));
        if (signatures[i] == NULL) {
            printf("Erreur memoire.\n");
            exit(1);
        }
    }

    /* partition initiale : finaux / non finaux */
    for (i = 0; i < accessible.nbEtats; i++) {
        partition[i] = estFinal(&accessible, accessible.etats[i]) ? 1 : 0;
    }

    change = 1;
    while (change) {
        change = 0;

        /* construire la signature de chaque état */
        for (i = 0; i < accessible.nbEtats; i++) {
            signatures[i][0] = estFinal(&accessible, accessible.etats[i]) ? 1 : 0;

            for (k = 0; k < accessible.nbAlphabet; k++) {
                char dest[100];
                if (destinationUnique(&accessible, accessible.etats[i], accessible.alphabet[k], dest)) {
                    int idxDest = indiceEtat(&accessible, dest);
                    signatures[i][k + 1] = partition[idxDest];
                } else {
                    signatures[i][k + 1] = -1;
                }
            }
        }

        nbClasses = 0;
        for (i = 0; i < accessible.nbEtats; i++) {
            int trouve = 0;

            for (j = 0; j < i; j++) {
                if (signaturesEgales(signatures[i], signatures[j], accessible.nbAlphabet + 1)) {
                    nouvellePartition[i] = nouvellePartition[j];
                    trouve = 1;
                    break;
                }
            }

            if (!trouve) {
                nouvellePartition[i] = nbClasses;
                nbClasses++;
            }
        }

        for (i = 0; i < accessible.nbEtats; i++) {
            if (partition[i] != nouvellePartition[i]) {
                change = 1;
                break;
            }
        }

        for (i = 0; i < accessible.nbEtats; i++) {
            partition[i] = nouvellePartition[i];
        }
    }

    for (i = 0; i < nbClasses; i++) {
        char nom[20];
        sprintf(nom, "M%d", i);
        ajouterEtat(&res, nom);
    }

    for (i = 0; i < accessible.nbEtats; i++) {
        char nomClasse[20];
        sprintf(nomClasse, "M%d", partition[i]);

        if (strcmp(accessible.etats[i], accessible.etatInitial) == 0) {
            strcpy(res.etatInitial, nomClasse);
        }

        if (estFinal(&accessible, accessible.etats[i])) {
            ajouterEtatFinal(&res, nomClasse);
        }
    }

    for (i = 0; i < accessible.nbEtats; i++) {
        char dep[20];
        sprintf(dep, "M%d", partition[i]);

        for (k = 0; k < accessible.nbAlphabet; k++) {
            char dest[100];
            if (destinationUnique(&accessible, accessible.etats[i], accessible.alphabet[k], dest)) {
                int idxDest = indiceEtat(&accessible, dest);
                char arr[20];
                sprintf(arr, "M%d", partition[idxDest]);
                ajouterTransition(&res, dep, accessible.alphabet[k], arr);
            }
        }
    }

    for (i = 0; i < accessible.nbAlphabet; i++) ajouterSymbole(&res, accessible.alphabet[i]);

    for (i = 0; i < accessible.nbEtats; i++) free(signatures[i]);
    free(signatures);
    free(partition);
    free(nouvellePartition);

    libererAutomate(&dfa);
    libererAutomate(&accessible);

    return res;
}

/* 17. Génération des fichiers .dot */
void genererFichiersOptimisation(Automate *a) {
    Automate det, min;

    det = determiniserAutomate(a);
    min = minimiserAutomateMoore(a);

    genererDot(a, "automate_initial.dot");
    genererDot(&det, "automate_deterministe.dot");
    genererDot(&min, "automate_minimal.dot");

    printf("Fichiers generes : automate_initial.dot, automate_deterministe.dot, automate_minimal.dot\n");

    libererAutomate(&det);
    libererAutomate(&min);
}

/* 18. Afficher les mots reconnus par l'automate minimal */
void afficherMotsAutomateMinimal(Automate *a, const char *nomFichier) {
    Automate min;
    FILE *f;
    char mot[200];
    int trouve = 0;

    min = minimiserAutomateMoore(a);

    f = fopen(nomFichier, "r");
    if (f == NULL) {
        printf("Erreur : impossible d'ouvrir %s\n", nomFichier);
        libererAutomate(&min);
        return;
    }

    printf("Mots reconnus par l'automate minimal :\n");
    while (fscanf(f, "%199s", mot) == 1) {
        if (motReconnu(&min, mot)) {
            printf("%s\n", mot);
            trouve = 1;
        }
    }

    if (!trouve) printf("Aucun mot reconnu.\n");

    fclose(f);
    libererAutomate(&min);
}

/* ========================================================= */
/* ======================== MENU =========================== */
/* ========================================================= */

void afficherMenu() {
    printf("\n================= MENU PRINCIPAL =================\n");
    printf("PARTIE 1 : LECTURE ET AFFICHAGE\n");
    printf("1. Lire un automate depuis un fichier .dot\n");
    printf("2. Afficher l'automate charge\n");

    printf("\nPARTIE 2 : MANIPULATION\n");
    printf("3. Generer un fichier .dot\n");
    printf("4. Etat avec le plus de transitions sortantes\n");
    printf("5. Etat avec le plus de transitions entrantes\n");
    printf("6. Afficher les etats ayant une transition avec une lettre\n");
    printf("7. Tester si un mot est reconnu\n");
    printf("8. Lire un fichier de mots et enregistrer les mots acceptes\n");

    printf("\nPARTIE 3 : OPERATIONS SUR LES AUTOMATES\n");
    printf("9. Concatenation de deux automates\n");
    printf("10. Union de deux automates\n");
    printf("11. Construire un automate depuis une expression reguliere\n");
    printf("12. Supprimer les epsilon-transitions de l'automate\n");
    printf("13. Transformer l'automate courant en expression reguliere\n");

    printf("\nPARTIE 4 : OPTIMISATION D'AUTOMATE\n");
    printf("14. Produit de deux automates (intersection)\n");
    printf("15. Determiniser un automate\n");
    printf("16. Minimiser un automate (Moore)\n");
    printf("17. Generer les fichiers .dot : initial / deterministe / minimal\n");
    printf("18. Afficher les mots d'un fichier txt reconnus par l'automate minimal\n");

    printf("\n0. Quitter\n");
    printf("==================================================\n");
    printf("Choix : ");
}

/* ========================================================= */
/* ========================= MAIN ========================== */
/* ========================================================= */

int main() {
    Automate a, b, res, det, min, prod;
    int choix;

    char fichier[200];
    char fichier2[200];
    char mot[200];
    char lettre[20];
    char expr[500];
    char *regexResultat;

    initialiserAutomate(&a);
    initialiserAutomate(&b);
    initialiserAutomate(&res);
    initialiserAutomate(&det);
    initialiserAutomate(&min);
    initialiserAutomate(&prod);

    do {
        afficherMenu();
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                printf("Nom du fichier .dot : ");
                scanf("%199s", fichier);
                lireAutomate(&a, fichier);
                break;

            case 2:
                afficherAutomate(&a);
                break;

            case 3:
                printf("Nom du fichier .dot a generer : ");
                scanf("%199s", fichier);
                genererDot(&a, fichier);
                break;

            case 4:
                etatPlusSortantes(&a);
                break;

            case 5:
                etatPlusEntrantes(&a);
                break;

            case 6:
                printf("Donner la lettre : ");
                scanf("%19s", lettre);
                afficherEtatsAvecLettre(&a, lettre);
                break;

            case 7:
                printf("Donner le mot : ");
                scanf("%199s", mot);
                if (motReconnu(&a, mot))
                    printf("Mot reconnu.\n");
                else
                    printf("Mot non reconnu.\n");
                break;

            case 8:
                printf("Nom du fichier d'entree : ");
                scanf("%199s", fichier);
                printf("Nom du fichier de sortie : ");
                scanf("%199s", fichier2);
                lireFichierMots(&a, fichier, fichier2);
                break;

            case 9:
                printf("Fichier .dot du premier automate : ");
                scanf("%199s", fichier);
                printf("Fichier .dot du deuxieme automate : ");
                scanf("%199s", fichier2);

                lireAutomate(&a, fichier);
                lireAutomate(&b, fichier2);

                libererAutomate(&res);
                initialiserAutomate(&res);
                res = concatenationAutomates(&a, &b);

                printf("Automate resultat de la concatenation :\n");
                afficherAutomate(&res);

                genererDot(&res, "automate_concatenation.dot");
                printf("Fichier genere : automate_concatenation.dot\n");
                break;

            case 10:
                printf("Fichier .dot du premier automate : ");
                scanf("%199s", fichier);
                printf("Fichier .dot du deuxieme automate : ");
                scanf("%199s", fichier2);

                lireAutomate(&a, fichier);
                lireAutomate(&b, fichier2);

                libererAutomate(&res);
                initialiserAutomate(&res);
                res = unionAutomates(&a, &b);

                printf("Automate resultat de l'union :\n");
                afficherAutomate(&res);

                genererDot(&res, "automate_union.dot");
                printf("Fichier genere : automate_union.dot\n");
                break;

            case 11:
                printf("Donner l'expression reguliere : ");
                scanf("%499s", expr);

                libererAutomate(&res);
                initialiserAutomate(&res);
                res = expressionVersAutomate(expr);

                printf("Automate genere a partir de l'expression reguliere :\n");
                afficherAutomate(&res);

                genererDot(&res, "automate_expression.dot");
                printf("Fichier genere : automate_expression.dot\n");
                break;

            case 12:
                printf("Entrer le nom du fichier .dot a supprimer les epsilon-transitions : ");
                scanf("%199s", fichier);

                lireAutomate(&a, fichier);

                libererAutomate(&res);
                initialiserAutomate(&res);
                res = supprimerEpsilonTransitions(&a);

                printf("Automate sans epsilon-transitions :\n");
                afficherAutomate(&res);

                genererDot(&res, "automate_sans_epsilon.dot");
                printf("Fichier genere : automate_sans_epsilon.dot\n");
                break;

            case 13:
                regexResultat = automateVersExpressionReguliere(&a);
                printf("Expression reguliere equivalente : %s\n", regexResultat);
                free(regexResultat);
                break;

            case 14:
                printf("Fichier .dot du premier automate : ");
                scanf("%199s", fichier);
                printf("Fichier .dot du deuxieme automate : ");
                scanf("%199s", fichier2);

                lireAutomate(&a, fichier);
                lireAutomate(&b, fichier2);

                libererAutomate(&prod);
                initialiserAutomate(&prod);
                prod = produitAutomates(&a, &b);

                printf("Automate produit (intersection) :\n");
                afficherAutomate(&prod);

                genererDot(&prod, "automate_produit.dot");
                printf("Fichier genere : automate_produit.dot\n");
                break;

            case 15:
                printf("Entrer le nom du fichier .dot a determiniser : ");
                scanf("%199s", fichier);

                lireAutomate(&a, fichier);

                libererAutomate(&det);
                initialiserAutomate(&det);
                det = determiniserAutomate(&a);

                printf("Automate deterministe :\n");
                afficherAutomate(&det);

                genererDot(&det, "automate_deterministe.dot");
                printf("Fichier genere : automate_deterministe.dot\n");
                break;

            case 16:
                printf("Entrer le nom du fichier .dot a minimiser : ");
                scanf("%199s", fichier);

                lireAutomate(&a, fichier);

                libererAutomate(&min);
                initialiserAutomate(&min);
                min = minimiserAutomateMoore(&a);

                printf("Automate minimal (Moore) :\n");
                afficherAutomate(&min);

                genererDot(&min, "automate_minimal.dot");
                printf("Fichier genere : automate_minimal.dot\n");
                break;

            case 17:
                printf("Entrer le nom du fichier .dot de l'automate initial : ");
                scanf("%199s", fichier);

                lireAutomate(&a, fichier);
                genererFichiersOptimisation(&a);
                break;

            case 18:
                printf("Entrer le nom du fichier .dot de l'automate : ");
                scanf("%199s", fichier);
                printf("Entrer le nom du fichier texte contenant les mots : ");
                scanf("%199s", fichier2);

                lireAutomate(&a, fichier);
                afficherMotsAutomateMinimal(&a, fichier2);
                break;

            case 0:
                printf("Fin du programme.\n");
                break;

            default:
                printf("Choix invalide.\n");
        }

    } while (choix != 0);

    libererAutomate(&a);
    libererAutomate(&b);
    libererAutomate(&res);
    libererAutomate(&det);
    libererAutomate(&min);
    libererAutomate(&prod);

    return 0;
}
