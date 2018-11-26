#include "mutator.hpp"

Mutator::Mutator(Solution* sol) {
  cerr << "Mutator non instanciable (pour l'instant) !" << endl;
  cerr << "Ne propose que des variables statiques." << endl;
  exit(1);
}
Mutator::~Mutator() {}

// Effectue une mutation 2opt sur un seul circuit tiré au hazard.
//
void Mutator::mutate_intra_circuit_2opt(Solution* sol) {
  logn4("Mutator::mutate_intra_circuit_2opt BEGIN");
  // U::die("Mutator::mutate_intra_circuit_2opt NON IMPLÉMENTÉE");
  Circuit* c1 = sol->circuits[rand() % sol->circuits.size()];
  // Attention : on suppose qu'au moins un des circuits est de taille suffisante
  // !
  while (c1->stations.size() < 2) {
    if (log5()) {
      cout << "mutate_intra_circuit_2opt : c1=" + c1->remorque->name +
                  " ignoré car de taille : " + U::to_s(c1->stations.size()) +
                  "\n";
    }
    c1 = sol->circuits[rand() % sol->circuits.size()];
  }
  // 1. choisir len une longueur de branche entre 1 et c1.size-1
  //   (size-1 pour ne pas vider le circuit entièrement)
  int len = U::randint(1, c1->stations.size() - 1);
  int i1 = U::randint(0, c1->stations.size() - len);
  int i2 = i1 + len;

  if (log5()) {
    cout << c1->remorque->name << ".reverse_branch(" << i1 << "," << i2
         << ")..." << endl;
  }
  c1->reverse_branch(i1, i2);
  c1->update();
  sol->update_lite();
  logn4("Mutator::mutate_intra_circuit_2opt END");
}
// Effectue une mutation par déplacement d'une station à l'intérieur
// d'un circuit tiré au hazard.
void Mutator::mutate_intra_circuit_move_station(Solution* sol) {
  logn4("Mutator::mutate_intra_circuit_move_station BEGIN");
  // U::die("Mutator::mutate_intra_circuit_move_station NON IMPLÉMENTÉE");
  Circuit* c1 = sol->circuits[rand() % sol->circuits.size()];
  // Attention : on suppose qu'au moins un des circuits est de taille suffisante
  // !
  while (c1->stations.size() < 2) {
    if (log5()) {
      cout << "mutate_intra_circuit_move_station : c1=" + c1->remorque->name +
                  " ignoré car de taille : " + U::to_s(c1->stations.size()) +
                  "\n";
    }
    c1 = sol->circuits[rand() % sol->circuits.size()];
  }

  int i1 = U::randint(0, c1->stations.size() - 1);
  int i2 = U::randint(0, c1->stations.size() - 1);
  while (i2 == i1) {
    i2 = U::randint(0, c1->stations.size() - 1);
  }
  if (log5()) {
    cout << c1->remorque->name << ".move(" << i1 << "," << i2 << ")..."
         << endl;
  }
  c1->move(i1, i2);
  c1->update();
  sol->update_lite();
  logn4("Mutator::mutate_intra_circuit_move_station END");
}
// Effectue une mutation par déplacement d'un station d'un circuit
// tiré au hazard vers un circuit différent.
void Mutator::mutate_inter_circuit_move_station(Solution* sol) {
  logn4("Mutator::mutate_inter_circuit_move_station BEGIN");
  // U::die("Mutator::mutate_inter_circuit_move_station NON IMPLÉMENTÉE");

  // Cas particulier d'une instance mono-remorque
  if (sol->circuits.size() == 1) {
    return Mutator::mutate_intra_circuit_move_station(sol);
  }

  Circuit* c1 = sol->circuits[rand() % sol->circuits.size()];
  // Attention : on suppose qu'au moins un des circuits est de taille suffisante
  // !
  while (c1->stations.size() < 2) {
    if (log5()) {
      cout << "mutate_inter_circuit_move_station : c1=" + c1->remorque->name +
                  " ignoré car de taille : " + U::to_s(c1->stations.size()) +
                  "\n";
    }
    c1 = sol->circuits[rand() % sol->circuits.size()];
  }
  int i1 = U::randint(0, c1->stations.size() - 1);  // size1 exclu

  // Choix d'un numéro de circuit destination (i.e. remorque) aléatoire
  Circuit* c2 = sol->circuits[rand() % sol->circuits.size()];
  while (c1 == c2) {
    c2 = sol->circuits[rand() % sol->circuits.size()];
  }

  int i2 = U::randint(0, c2->stations.size());  // size2 inclu
  if (log5()) {
    cout << c1->remorque->name << ".move_to(" << i1 << ","
         << c2->remorque->name << "," << i2 << ")..." << endl;
  }
  c1->move_to(i1, c2, i2);
  c1->update();
  c2->update();
  sol->update_lite();

  logn4("Mutator::mutate_inter_circuit_move_station END");
}
// Effectue une mutation par déplacement d'une branche de station d'un
// circuit tiré au hazard vers un circuit différent.
void Mutator::mutate_inter_circuit_move_branch(Solution* sol) {
  logn4("Mutator::mutate_inter_circuit_move_branch BEGIN");
  // U::die("Mutator::mutate_inter_circuit_move_branch NON IMPLÉMENTÉE");

  // Cas particulier d'une instance mono-remorque
  if (sol->circuits.size() == 1) {
    return Mutator::mutate_intra_circuit_2opt(sol);
  }

  // Choix d'un numéro de circuit source (i.e. remorque) aléatoire
  Circuit* c1 = sol->circuits[rand() % sol->circuits.size()];
  // Attention : on suppose qu'au moins un des circuit est de taille suffisante
  // !
  while (c1->stations.size() < 2) {
    if (log5()) {
      cout << "mutate_inter_circuit_move_branch : c1=" + c1->remorque->name +
                  " ignoré car de taille : " + U::to_s(c1->stations.size()) +
                  "\n";
    }
    c1 = sol->circuits[rand() % sol->circuits.size()];
  }
  // Choix d'un numéro de circuit destination (i.e. remorque) aléatoire
  Circuit* c2 = sol->circuits[rand() % sol->circuits.size()];
  while (c1 == c2) {
    c2 = sol->circuits[rand() % sol->circuits.size()];
  }

  // DEMARCHE POUR LE CHOIX DE LA BRANCHE ALEATOIRE A TRANSFERER :
  // 1. choisir len, une longueur de branche entre 1 et c1.size
  // 2. choisir i1 le début de la branche entre 0 et c1.size - length
  // 3. déduire  i1 := i1+len
  // Autres solutions possibles :
  // - choisir i1 dans [0,c1.size)
  //   et i2 dans [i1+1, c1.size+1)
  // - choisir tmp1 et tmp2 dans [0,c1.size)
  //   puis i1=min(tmp1,tmp2) i2= max(tmp1,tmp2)+1

  // // 1. choisir len une longueur de branche entre 1 et c1.size
  // int len = U::randint(1, c1->stations.size()) ;
  // int i1 = U::randint(0, c1->stations.size() - len) ;
  // int i2 = i1 + len ;

  // 1. choisir len une longueur de branche entre 1 et c1.size-1
  //   (size-1 pour ne pas vider le circuit entièrement)
  int len = U::randint(1, c1->stations.size() - 1);
  int i1 = U::randint(0, c1->stations.size() - len);
  int i2 = i1 + len;

  // i3: nouvelle position de la branche insérée
  int i3 = rand() % (c2->stations.size() + 1);  // de 0 à size inclu!
  // rev: rev: si true on inverse la branche insérée
  bool rev = (rand() % 2 == 0);
  if (log5()) {
    cout << c1->remorque->name << ".move_branch_to(" << i1 << "," << i2 << ","
         << c2->remorque->name << "," << i3 << "," << rev << ")..." << endl;
  }
  c1->move_branch_to(i1, i2, c2, i3, rev);
  c1->update();
  c2->update();
  sol->update_lite();

  logn4("Mutator::mutate_inter_circuit_move_branch END");
}

//./
