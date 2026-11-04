#include <iomanip>
#include <iostream>
#include <chrono>
#include <map>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Noeud {
    int valeur;
    bool rouge;
    Noeud* gauche;
    Noeud* droite;
    Noeud* parent;
};

// ================= CREATION =================

Noeud* creerNoeud(int valeur) {
    Noeud* n = new Noeud;
    n->valeur = valeur;
    n->rouge = true;
    n->gauche = nullptr;
    n->droite = nullptr;
    n->parent = nullptr;
    return n;
}

bool estRouge(Noeud* n) {
    return n != nullptr && n->rouge;
}

// ================= ROTATIONS =================

void rotationGauche(Noeud*& racine, Noeud* x) {
    Noeud* y = x->droite;

    x->droite = y->gauche;

    if (y->gauche != nullptr)
        y->gauche->parent = x;

    y->parent = x->parent;

    if (x->parent == nullptr)
        racine = y;
    else if (x == x->parent->gauche)
        x->parent->gauche = y;
    else
        x->parent->droite = y;

    y->gauche = x;
    x->parent = y;
}

void rotationDroite(Noeud*& racine, Noeud* x) {
    Noeud* y = x->gauche;

    x->gauche = y->droite;

    if (y->droite != nullptr)
        y->droite->parent = x;

    y->parent = x->parent;

    if (x->parent == nullptr)
        racine = y;
    else if (x == x->parent->droite)
        x->parent->droite = y;
    else
        x->parent->gauche = y;

    y->droite = x;
    x->parent = y;
}

// ================= INSERTION =================

void corrigerInsertion(Noeud*& racine, Noeud* z) {
    while (z->parent != nullptr && z->parent->rouge) {
        Noeud* grandParent = z->parent->parent;

        if (z->parent == grandParent->gauche) {
            Noeud* oncle = grandParent->droite;

            if (estRouge(oncle)) {
                z->parent->rouge = false;
                oncle->rouge = false;
                grandParent->rouge = true;
                z = grandParent;
            }
            else {
                if (z == z->parent->droite) {
                    z = z->parent;
                    rotationGauche(racine, z);
                }

                z->parent->rouge = false;
                z->parent->parent->rouge = true;
                rotationDroite(racine, z->parent->parent);
            }
        }
        else {
            Noeud* oncle = grandParent->gauche;

            if (estRouge(oncle)) {
                z->parent->rouge = false;
                oncle->rouge = false;
                grandParent->rouge = true;
                z = grandParent;
            }
            else {
                if (z == z->parent->gauche) {
                    z = z->parent;
                    rotationDroite(racine, z);
                }

                z->parent->rouge = false;
                z->parent->parent->rouge = true;
                rotationGauche(racine, z->parent->parent);
            }
        }
    }

    racine->rouge = false;
}

void inserer(Noeud*& racine, int valeur) {
    Noeud* nouveau = creerNoeud(valeur);

    Noeud* parent = nullptr;
    Noeud* courant = racine;

    while (courant != nullptr) {
        parent = courant;

        if (valeur < courant->valeur)
            courant = courant->gauche;
        else if (valeur > courant->valeur)
            courant = courant->droite;
        else {
            delete nouveau;
            return;
        }
    }

    nouveau->parent = parent;

    if (parent == nullptr)
        racine = nouveau;
    else if (valeur < parent->valeur)
        parent->gauche = nouveau;
    else
        parent->droite = nouveau;

    corrigerInsertion(racine, nouveau);
}

// ================= RECHERCHE =================

Noeud* rechercher(Noeud* racine, int valeur) {
    Noeud* courant = racine;

    while (courant != nullptr) {
        if (valeur == courant->valeur)
            return courant;

        if (valeur < courant->valeur)
            courant = courant->gauche;
        else
            courant = courant->droite;
    }

    return nullptr;
}

// ================= SUPPRESSION =================

Noeud* minimum(Noeud* n) {
    while (n != nullptr && n->gauche != nullptr)
        n = n->gauche;

    return n;
}

void remplacer(Noeud*& racine, Noeud* u, Noeud* v) {
    if (u->parent == nullptr)
        racine = v;
    else if (u == u->parent->gauche)
        u->parent->gauche = v;
    else
        u->parent->droite = v;

    if (v != nullptr)
        v->parent = u->parent;
}

// Correction apres suppression

void corrigerSuppression(Noeud*& racine, Noeud* x, Noeud* parent) {
    while (x != racine && !estRouge(x)) {

        if (parent == nullptr)
            break;

        if (x == parent->gauche) {
            Noeud* frere = parent->droite;

            if (estRouge(frere)) {
                frere->rouge = false;
                parent->rouge = true;
                rotationGauche(racine, parent);
                frere = parent->droite;
            }

            if (frere == nullptr) {
                x = parent;
                parent = x->parent;
            }
            else if (!estRouge(frere->gauche) &&
                     !estRouge(frere->droite)) {

                frere->rouge = true;
                x = parent;
                parent = x->parent;
            }
            else {
                if (!estRouge(frere->droite)) {
                    if (frere->gauche != nullptr)
                        frere->gauche->rouge = false;

                    frere->rouge = true;
                    rotationDroite(racine, frere);
                    frere = parent->droite;
                }

                frere->rouge = parent->rouge;
                parent->rouge = false;

                if (frere->droite != nullptr)
                    frere->droite->rouge = false;

                rotationGauche(racine, parent);

                x = racine;
                parent = nullptr;
            }
        }
        else {
            Noeud* frere = parent->gauche;

            if (estRouge(frere)) {
                frere->rouge = false;
                parent->rouge = true;
                rotationDroite(racine, parent);
                frere = parent->gauche;
            }

            if (frere == nullptr) {
                x = parent;
                parent = x->parent;
            }
            else if (!estRouge(frere->gauche) &&
                     !estRouge(frere->droite)) {

                frere->rouge = true;
                x = parent;
                parent = x->parent;
            }
            else {
                if (!estRouge(frere->gauche)) {
                    if (frere->droite != nullptr)
                        frere->droite->rouge = false;

                    frere->rouge = true;
                    rotationGauche(racine, frere);
                    frere = parent->gauche;
                }

                frere->rouge = parent->rouge;
                parent->rouge = false;

                if (frere->gauche != nullptr)
                    frere->gauche->rouge = false;

                rotationDroite(racine, parent);

                x = racine;
                parent = nullptr;
            }
        }
    }

    if (x != nullptr)
        x->rouge = false;
}

// Suppression d'un element

void supprimer(Noeud*& racine, int valeur) {
    Noeud* z = rechercher(racine, valeur);

    if (z == nullptr)
        return;

    Noeud* y = z;
    Noeud* x = nullptr;
    Noeud* parentX = nullptr;

    bool couleurOriginale = y->rouge;

    if (z->gauche == nullptr) {
        x = z->droite;
        parentX = z->parent;

        remplacer(racine, z, z->droite);
    }
    else if (z->droite == nullptr) {
        x = z->gauche;
        parentX = z->parent;

        remplacer(racine, z, z->gauche);
    }
    else {
        y = minimum(z->droite);
        couleurOriginale = y->rouge;
        x = y->droite;

        if (y->parent == z) {
            parentX = y;

            if (x != nullptr)
                x->parent = y;
        }
        else {
            parentX = y->parent;

            remplacer(racine, y, y->droite);

            y->droite = z->droite;
            y->droite->parent = y;
        }

        remplacer(racine, z, y);

        y->gauche = z->gauche;
        y->gauche->parent = y;

        y->rouge = z->rouge;
    }

    delete z;

    if (!couleurOriginale)
        corrigerSuppression(racine, x, parentX);
}

// ================= LIBERATION MEMOIRE =================

void libererArbre(Noeud* racine) {
    if (racine == nullptr)
        return;

    libererArbre(racine->gauche);
    libererArbre(racine->droite);

    delete racine;
}

// ================= AFFICHAGE =================

void afficherNoeud(Noeud* n, string relation, int niveau) {
    if (n == nullptr)
        return;

    for (int i = 0; i < niveau; i++)
        cout << "    ";

    cout << relation << n->valeur;

    if (n->rouge)
        cout << "(R)";
    else
        cout << "(N)";

    cout << endl;

    afficherNoeud(n->gauche, "Fils gauche : ", niveau + 1);
    afficherNoeud(n->droite, "Fils droit  : ", niveau + 1);
}

// ================= TESTS =================
// ================= TESTS =================

int main() {

    int tailles[] = {10, 1000, 10000, 100000, 1000000};
    int nbTests = 5;

    cout << "\n";
    cout << "============================================================\n";
    cout << "     COMPARAISON RED-BLACK TREE VS MAP STL\n";
    cout << "============================================================\n\n";

    cout << left
         << setw(10) << "N"
         << setw(12) << "RB-Insert"
         << setw(12) << "RB-Search"
         << setw(12) << "RB-Delete"
         << setw(12) << "Map-Insert"
         << setw(12) << "Map-Search"
         << setw(12) << "Map-Delete"
         << endl;

    cout << "--------------------------------------------------------------------------\n";

    for (int t = 0; t < nbTests; t++) {

        int N = tailles[t];

        // ==================================================
        // ETUDE 1 : RED-BLACK TREE FROM SCRATCH
        // ==================================================

        // INSERTION
        Noeud* racineInsertion = nullptr;

        auto debutInsertion = high_resolution_clock::now();

        for (int i = 1; i <= N; i++)
            inserer(racineInsertion, i);

        auto finInsertion = high_resolution_clock::now();

        auto tempsInsertion =
            duration_cast<microseconds>(
                finInsertion - debutInsertion
            ).count();


        // RECHERCHE
        Noeud* racineRecherche = nullptr;

        for (int i = 1; i <= N; i++)
            inserer(racineRecherche, i);

        auto debutRecherche = high_resolution_clock::now();

        for (int i = 1; i <= N; i++)
            rechercher(racineRecherche, i);

        auto finRecherche = high_resolution_clock::now();

        auto tempsRecherche =
            duration_cast<microseconds>(
                finRecherche - debutRecherche
            ).count();


        // SUPPRESSION
        Noeud* racineSuppression = nullptr;

        for (int i = 1; i <= N; i++)
            inserer(racineSuppression, i);

        auto debutSuppression = high_resolution_clock::now();

        for (int i = 1; i <= N; i++)
            supprimer(racineSuppression, i);

        auto finSuppression = high_resolution_clock::now();

        auto tempsSuppression =
            duration_cast<microseconds>(
                finSuppression - debutSuppression
            ).count();


        // ==================================================
        // ETUDE 2 : CONTENEUR STL MAP
        // ==================================================

        // INSERTION
        map<int, int> arbreSTLInsertion;

        auto debutMapInsertion = high_resolution_clock::now();

        for (int i = 1; i <= N; i++)
            arbreSTLInsertion.insert(make_pair(i, i));

        auto finMapInsertion = high_resolution_clock::now();

        auto tempsMapInsertion =
            duration_cast<microseconds>(
                finMapInsertion - debutMapInsertion
            ).count();


        // RECHERCHE
        map<int, int> arbreSTLRecherche;

        for (int i = 1; i <= N; i++)
            arbreSTLRecherche.insert(make_pair(i, i));

        auto debutMapRecherche = high_resolution_clock::now();

        for (int i = 1; i <= N; i++)
            arbreSTLRecherche.find(i);

        auto finMapRecherche = high_resolution_clock::now();

        auto tempsMapRecherche =
            duration_cast<microseconds>(
                finMapRecherche - debutMapRecherche
            ).count();


        // SUPPRESSION
        map<int, int> arbreSTLSuppression;

        for (int i = 1; i <= N; i++)
            arbreSTLSuppression.insert(make_pair(i, i));

        auto debutMapSuppression = high_resolution_clock::now();

        for (int i = 1; i <= N; i++)
            arbreSTLSuppression.erase(i);

        auto finMapSuppression = high_resolution_clock::now();

        auto tempsMapSuppression =
            duration_cast<microseconds>(
                finMapSuppression - debutMapSuppression
            ).count();


        // ==================================================
        // AFFICHAGE
        // ==================================================

        cout << left
             << setw(10) << N
             << setw(12) << tempsInsertion
             << setw(12) << tempsRecherche
             << setw(12) << tempsSuppression
             << setw(12) << tempsMapInsertion
             << setw(12) << tempsMapRecherche
             << setw(12) << tempsMapSuppression
             << endl;


        // LIBERATION MEMOIRE
        libererArbre(racineInsertion);
        libererArbre(racineRecherche);
        libererArbre(racineSuppression);
    }

    cout << "--------------------------------------------------------------------------\n";

    cout << "Temps en microsecondes (us)\n";
    cout << "Insertion, recherche, suppression : O(log n) par operation\n";
    cout << "Pour n operations : O(n log n)\n";

    return 0;
}
