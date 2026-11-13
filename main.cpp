
#include <iostream>
#include <iomanip>
#include <chrono>
#include <map>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <ctime>
#include <memory>

using namespace std;
using namespace std::chrono;

// ======================================================
// COMPTEURS
// ======================================================

// Red-Black Tree
long long comparaisonsRB = 0;
long long rotationsRB = 0;
long long recolorationsRB = 0;
long long allocationsRB = 0;

// STL
long long comparaisonsSTL = 0;
long long allocationsSTL = 0;

// Hash Map
long long comparaisonsHash = 0;
long long appelsHash = 0;

// ======================================================
// ALLOCATEUR POUR COMPTER LES ALLOCATIONS STL
// ======================================================

template <class T>
struct AllocateurCompteur {

    using value_type = T;

    AllocateurCompteur() noexcept {}

    template<class U>
    AllocateurCompteur(const AllocateurCompteur<U>&) noexcept {}

    T* allocate(size_t n) {
        allocationsSTL++;
        return allocator<T>().allocate(n);
    }

    void deallocate(T* p, size_t n) {
        allocator<T>().deallocate(p, n);
    }

    template<class U>
    bool operator==(const AllocateurCompteur<U>&) const {
        return true;
    }

    template<class U>
    bool operator!=(const AllocateurCompteur<U>&) const {
        return false;
    }
};

// ======================================================
// COMPARATEUR POUR MAP
// ======================================================

struct Comparateur {

    bool operator()(int a, int b) const {
        comparaisonsSTL++;
        return a < b;
    }
};

// ======================================================
// HASH ET EGALITE POUR UNORDERED_MAP
// ======================================================

struct FonctionHash {

    size_t operator()(int x) const {
        appelsHash++;
        return hash<int>()(x);
    }
};

struct Egalite {

    bool operator()(int a, int b) const {
        comparaisonsHash++;
        return a == b;
    }
};

// ======================================================
// STRUCTURE RED-BLACK TREE
// ======================================================

struct Noeud {
    int valeur;
    bool rouge;
    Noeud* gauche;
    Noeud* droite;
    Noeud* parent;
};

Noeud* creerNoeud(int valeur) {

    Noeud* n = new Noeud;

    allocationsRB++;

    n->valeur = valeur;
    n->rouge = true;
    n->gauche = nullptr;
    n->droite = nullptr;
    n->parent = nullptr;

    return n;
}

// ======================================================
// GESTION DES COULEURS
// ======================================================

void changerCouleur(Noeud* n, bool couleur) {

    if (n != nullptr && n->rouge != couleur) {
        n->rouge = couleur;
        recolorationsRB++;
    }
}

bool estRouge(Noeud* n) {
    return n != nullptr && n->rouge;
}

// ======================================================
// ROTATIONS
// ======================================================

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

    rotationsRB++;
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

    rotationsRB++;
}

// ======================================================
// CORRECTION INSERTION
// ======================================================

void corrigerInsertion(Noeud*& racine, Noeud* z) {

    while (z->parent != nullptr && z->parent->rouge) {

        Noeud* grandParent = z->parent->parent;

        if (z->parent == grandParent->gauche) {

            Noeud* oncle = grandParent->droite;

            if (estRouge(oncle)) {

                changerCouleur(z->parent, false);
                changerCouleur(oncle, false);
                changerCouleur(grandParent, true);

                z = grandParent;
            }
            else {

                if (z == z->parent->droite) {
                    z = z->parent;
                    rotationGauche(racine, z);
                }

                changerCouleur(z->parent, false);
                changerCouleur(z->parent->parent, true);

                rotationDroite(racine, z->parent->parent);
            }
        }
        else {

            Noeud* oncle = grandParent->gauche;

            if (estRouge(oncle)) {

                changerCouleur(z->parent, false);
                changerCouleur(oncle, false);
                changerCouleur(grandParent, true);

                z = grandParent;
            }
            else {

                if (z == z->parent->gauche) {
                    z = z->parent;
                    rotationDroite(racine, z);
                }

                changerCouleur(z->parent, false);
                changerCouleur(z->parent->parent, true);

                rotationGauche(racine, z->parent->parent);
            }
        }
    }

    changerCouleur(racine, false);
}

// ======================================================
// INSERTION
// ======================================================

void inserer(Noeud*& racine, int valeur) {

    Noeud* nouveau = creerNoeud(valeur);

    Noeud* parent = nullptr;
    Noeud* courant = racine;

    while (courant != nullptr) {

        parent = courant;

        comparaisonsRB++;

        if (valeur < courant->valeur)
            courant = courant->gauche;

        else {

            comparaisonsRB++;

            if (valeur > courant->valeur)
                courant = courant->droite;

            else {
                delete nouveau;
                allocationsRB--;
                return;
            }
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

// ======================================================
// RECHERCHE
// ======================================================

Noeud* rechercher(Noeud* racine, int valeur) {

    Noeud* courant = racine;

    while (courant != nullptr) {

        comparaisonsRB++;

        if (valeur == courant->valeur)
            return courant;

        comparaisonsRB++;

        if (valeur < courant->valeur)
            courant = courant->gauche;
        else
            courant = courant->droite;
    }

    return nullptr;
}

// ======================================================
// MINIMUM ET REMPLACEMENT
// ======================================================

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

// ======================================================
// CORRECTION SUPPRESSION
// ======================================================

void corrigerSuppression(Noeud*& racine,
                         Noeud* x,
                         Noeud* parent) {

    while (x != racine && !estRouge(x)) {

        if (parent == nullptr)
            break;

        if (x == parent->gauche) {

            Noeud* frere = parent->droite;

            if (estRouge(frere)) {

                changerCouleur(frere, false);
                changerCouleur(parent, true);

                rotationGauche(racine, parent);

                frere = parent->droite;
            }

            if (frere == nullptr) {

                x = parent;
                parent = x->parent;
            }
            else if (!estRouge(frere->gauche) &&
                     !estRouge(frere->droite)) {

                changerCouleur(frere, true);

                x = parent;
                parent = x->parent;
            }
            else {

                if (!estRouge(frere->droite)) {

                    changerCouleur(frere->gauche, false);
                    changerCouleur(frere, true);

                    rotationDroite(racine, frere);

                    frere = parent->droite;
                }

                changerCouleur(frere, parent->rouge);
                changerCouleur(parent, false);

                changerCouleur(frere->droite, false);

                rotationGauche(racine, parent);

                x = racine;
                parent = nullptr;
            }
        }
        else {

            Noeud* frere = parent->gauche;

            if (estRouge(frere)) {

                changerCouleur(frere, false);
                changerCouleur(parent, true);

                rotationDroite(racine, parent);

                frere = parent->gauche;
            }

            if (frere == nullptr) {

                x = parent;
                parent = x->parent;
            }
            else if (!estRouge(frere->gauche) &&
                     !estRouge(frere->droite)) {

                changerCouleur(frere, true);

                x = parent;
                parent = x->parent;
            }
            else {

                if (!estRouge(frere->gauche)) {

                    changerCouleur(frere->droite, false);
                    changerCouleur(frere, true);

                    rotationGauche(racine, frere);

                    frere = parent->gauche;
                }

                changerCouleur(frere, parent->rouge);
                changerCouleur(parent, false);

                changerCouleur(frere->gauche, false);

                rotationDroite(racine, parent);

                x = racine;
                parent = nullptr;
            }
        }
    }

    changerCouleur(x, false);
}

// ======================================================
// SUPPRESSION
// ======================================================

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

        changerCouleur(y, z->rouge);
    }

    delete z;

    if (!couleurOriginale)
        corrigerSuppression(racine, x, parentX);
}

// ======================================================
// LIBERATION MEMOIRE
// ======================================================

void libererArbre(Noeud* racine) {

    if (racine == nullptr)
        return;

    libererArbre(racine->gauche);
    libererArbre(racine->droite);

    delete racine;
}

// ======================================================
// GENERATION DES DONNEES
// ======================================================

void genererDonnees(int N,
                    int triees[],
                    int presqueTriees[],
                    int aleatoires[]) {

    // Donnees triees

    for (int i = 0; i < N; i++) {
        triees[i] = i + 1;
    }

    // Donnees presque triees

    for (int i = 0; i < N; i++) {
        presqueTriees[i] = triees[i];
    }

    for (int i = 0; i + 1 < N; i += 5) {

        int temp = presqueTriees[i];

        presqueTriees[i] = presqueTriees[i + 1];

        presqueTriees[i + 1] = temp;
    }

    // Donnees aleatoires

    for (int i = 0; i < N; i++) {
        aleatoires[i] = triees[i];
    }

    for (int i = 0; i < N; i++) {

        int j = rand() % N;

        int temp = aleatoires[i];

        aleatoires[i] = aleatoires[j];

        aleatoires[j] = temp;
    }
}

// ======================================================
// PROGRAMME PRINCIPAL
// ======================================================

int main() {

    srand(42);

    const int tailles[] = {
        10, 1000, 10000, 100000
    };

    const int nbTests = sizeof(tailles) / sizeof(tailles[0]);

    const string nomsScenarios[] = {
        "DONNEES TRIEES",
        "DONNEES PRESQUE TRIEES",
        "DONNEES ALEATOIRES"
    };

    for (int t = 0; t < nbTests; t++) {

        int N = tailles[t];

        int* triees = new int[N];
        int* presqueTriees = new int[N];
        int* aleatoires = new int[N];

        genererDonnees(N, triees, presqueTriees, aleatoires);

        int* scenarios[] = {
            triees, presqueTriees, aleatoires
        };

        for (int s = 0; s < 3; s++) {

            int* donnees = scenarios[s];

            cout << "\n\n";
            cout << "==============================================\n";
            cout << nomsScenarios[s] << " - N = " << N << "\n";
            cout << "==============================================\n";

            // ==========================================
            // 1. RED-BLACK TREE
            // ==========================================

            Noeud* racine = nullptr;

            comparaisonsRB = 0;
            rotationsRB = 0;
            recolorationsRB = 0;
            allocationsRB = 0;

            auto debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                inserer(racine, donnees[i]);

            auto fin = high_resolution_clock::now();

            long long tempsInsRB =
                duration_cast<microseconds>(fin - debut).count();

            long long compInsRB = comparaisonsRB;
            long long rotInsRB = rotationsRB;
            long long recolInsRB = recolorationsRB;
            long long allocInsRB = allocationsRB;

            // RECHERCHE

            comparaisonsRB = 0;
            rotationsRB = 0;
            recolorationsRB = 0;
            allocationsRB = 0;

            debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                rechercher(racine, donnees[i]);

            fin = high_resolution_clock::now();

            long long tempsRechRB =
                duration_cast<microseconds>(fin - debut).count();

            long long compRechRB = comparaisonsRB;

            // SUPPRESSION

            comparaisonsRB = 0;
            rotationsRB = 0;
            recolorationsRB = 0;
            allocationsRB = 0;

            debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                supprimer(racine, donnees[i]);

            fin = high_resolution_clock::now();

            long long tempsSupRB =
                duration_cast<microseconds>(fin - debut).count();

            long long compSupRB = comparaisonsRB;
            long long rotSupRB = rotationsRB;
            long long recolSupRB = recolorationsRB;

            libererArbre(racine);

            // ==========================================
            // 2. MAP
            // ==========================================

            using MaMap = map<int, int, Comparateur,
                              AllocateurCompteur<pair<const int, int>>>;

            comparaisonsSTL = 0;
            allocationsSTL = 0;

            MaMap mapInsertion;

            debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                mapInsertion.insert(make_pair(donnees[i], donnees[i]));

            fin = high_resolution_clock::now();

            long long tempsInsMap =
                duration_cast<microseconds>(fin - debut).count();

            long long compInsMap = comparaisonsSTL;
            long long allocInsMap = allocationsSTL;

            // RECHERCHE

            MaMap mapRecherche;

            for (int i = 0; i < N; i++)
                mapRecherche.insert(make_pair(donnees[i], donnees[i]));

            comparaisonsSTL = 0;

            debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                mapRecherche.find(donnees[i]);

            fin = high_resolution_clock::now();

            long long tempsRechMap =
                duration_cast<microseconds>(fin - debut).count();

            long long compRechMap = comparaisonsSTL;

            // SUPPRESSION

            MaMap mapSuppression;

            for (int i = 0; i < N; i++)
                mapSuppression.insert(make_pair(donnees[i], donnees[i]));

            comparaisonsSTL = 0;

            debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                mapSuppression.erase(donnees[i]);

            fin = high_resolution_clock::now();

            long long tempsSupMap =
                duration_cast<microseconds>(fin - debut).count();

            long long compSupMap = comparaisonsSTL;

            // ==========================================
            // 3. UNORDERED MAP
            // ==========================================

            using MaHash = unordered_map<int, int,
                          FonctionHash, Egalite,
                          AllocateurCompteur<pair<const int, int>>>;

            comparaisonsHash = 0;
            appelsHash = 0;
            allocationsSTL = 0;

            MaHash hashInsertion;

            debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                hashInsertion.insert(make_pair(donnees[i], donnees[i]));

            fin = high_resolution_clock::now();

            long long tempsInsHash =
                duration_cast<microseconds>(fin - debut).count();

            long long compInsHash = comparaisonsHash;
            long long allocInsHash = allocationsSTL;

            // COLLISIONS

            long long collisions = 0;

            for (size_t i = 0; i < hashInsertion.bucket_count(); i++) {

                size_t n = hashInsertion.bucket_size(i);

                if (n > 1)
                    collisions += n - 1;
            }

            // RECHERCHE

            MaHash hashRecherche;

            for (int i = 0; i < N; i++)
                hashRecherche.insert(make_pair(donnees[i], donnees[i]));

            comparaisonsHash = 0;

            debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                hashRecherche.find(donnees[i]);

            fin = high_resolution_clock::now();

            long long tempsRechHash =
                duration_cast<microseconds>(fin - debut).count();

            long long compRechHash = comparaisonsHash;

            // SUPPRESSION

            MaHash hashSuppression;

            for (int i = 0; i < N; i++)
                hashSuppression.insert(make_pair(donnees[i], donnees[i]));

            comparaisonsHash = 0;

            debut = high_resolution_clock::now();

            for (int i = 0; i < N; i++)
                hashSuppression.erase(donnees[i]);

            fin = high_resolution_clock::now();

            long long tempsSupHash =
                duration_cast<microseconds>(fin - debut).count();

            long long compSupHash = comparaisonsHash;

            // ==========================================
            // AFFICHAGE
            // ==========================================

            cout << "\nTEMPS EN MICROSECONDES\n";

            cout << left << setw(20) << "Structure"
                 << setw(12) << "Insertion"
                 << setw(12) << "Recherche"
                 << setw(12) << "Suppression" << "\n";

            cout << "--------------------------------------------------------\n";

            cout << left << setw(20) << "Red-Black Tree"
                 << setw(12) << tempsInsRB
                 << setw(12) << tempsRechRB
                 << setw(12) << tempsSupRB << "\n";

            cout << left << setw(20) << "std::map"
                 << setw(12) << tempsInsMap
                 << setw(12) << tempsRechMap
                 << setw(12) << tempsSupMap << "\n";

            cout << left << setw(20) << "unordered_map"
                 << setw(12) << tempsInsHash
                 << setw(12) << tempsRechHash
                 << setw(12) << tempsSupHash << "\n";


            cout << "\nRED-BLACK TREE : METRIQUES\n";

            cout << "Insertion : Comparaisons = " << compInsRB
                 << " | Rotations = " << rotInsRB
                 << " | Recolorations = " << recolInsRB
                 << " | Allocations = " << allocInsRB << "\n";

            cout << "Recherche : Comparaisons = " << compRechRB << "\n";

            cout << "Suppression : Comparaisons = " << compSupRB
                 << " | Rotations = " << rotSupRB
                 << " | Recolorations = " << recolSupRB << "\n";


            cout << "\nSTD::MAP : METRIQUES\n";

            cout << "Insertion : Comparaisons = " << compInsMap
                 << " | Allocations = " << allocInsMap << "\n";

            cout << "Recherche : Comparaisons = " << compRechMap << "\n";

            cout << "Suppression : Comparaisons = " << compSupMap << "\n";

            cout << "Rotations et recolorations : Non disponible\n";


            cout << "\nUNORDERED_MAP : METRIQUES\n";

            cout << "Insertion : Comparaisons d'egalite = " << compInsHash
                 << " | Allocations = " << allocInsHash << "\n";

            cout << "Recherche : Comparaisons d'egalite = " << compRechHash << "\n";

            cout << "Suppression : Comparaisons d'egalite = " << compSupHash << "\n";

            cout << "Collisions = " << collisions << "\n";

            cout << "Rotations et recolorations : Non disponible\n";

            cout << "--------------------------------------------------------\n";
        }

        delete[] triees;
        delete[] presqueTriees;
        delete[] aleatoires;
    }

    cout << "\nFIN DE L'ETUDE EXPERIMENTALE\n";

    return 0;
}
