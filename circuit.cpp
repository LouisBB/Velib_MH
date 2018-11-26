#include "circuit.hpp"

Circuit::Circuit(const Instance *inst, Remorque *remorque) : inst(inst) {
  // logn1("Circuit::Circuit: START " + inst.remorque(rid).name);
  // this->inst = inst;
  this->remorque = remorque;
  this->charge_init = 0;
  this->desequilibre = 0;
  this->length = 0;

  this->stations = vector<Station *>();
  this->stations.reserve(
      inst->stations.size());  // ajout le 07/12/2017 IMPORTANT
  this->depots = unordered_map<Station *, int>();
  this->charges = unordered_map<Station *, int>();
  // this->depots = vector<int>();
  // this->charges = vector<int>();
}
Circuit::Circuit(const Instance *inst, Remorque *remorque,
                 const vector<Station *> &stations)
    : Circuit(inst, remorque) {
  // On pourrait utiliser la m�thode assign plus souple
  // this->stations.assign(stations.begin(), stations.end());
  this->stations = stations;
}

Circuit::Circuit(const Circuit *other) {
  // U::die("Circuit::Circuit(const Circuit* other) NON TEST� (PLANTE) !");

  // CECI FONCTIONNE AUSSI (avec copie des listes et maps le 22/2/2016)
  this->stations = vector<Station *>(other->stations);
  this->stations.reserve(other->stations.size());  // ajout le 07/12/2017
  this->depots = unordered_map<Station *, int>(other->depots);
  this->charges = unordered_map<Station *, int>(other->charges);

  this->inst = other->inst;
  this->remorque = other->remorque;
  this->charge_init = other->charge_init;
  this->desequilibre = other->desequilibre;
  this->length = other->length;
}

Circuit::~Circuit() {}

void Circuit::copy(const Circuit *other) {
  this->inst = other->inst;
  this->remorque = other->remorque;
  this->charge_init = other->charge_init;
  this->desequilibre = other->desequilibre;
  this->length = other->length;

  this->stations.assign(other->stations.begin(), other->stations.end());
  this->depots = other->depots;
  this->charges = other->charges;
}
// Supprime les informations d'�quilibrage de ce circuit
// (ne modifie pas le vecteur de stations !)
void Circuit::partial_clear() {
  this->charge_init = 0;
  this->desequilibre = 0;
  this->length = 0;
  this->depots.clear();
  // this->depots->resize(this->stations->size(), 0); // ssi vector
  this->charges.clear();
  // this->charges->resize(this->stations->size(), 0); // ssi vector
}

// Vide enti�rement le circuit
void Circuit::clear() {
  this->partial_clear();
  this->stations.clear();
}

// Recalcule l'�quilibrage et met � jour les attributs d�riv�s
void Circuit::update() {
  logn5("Circuit::update BEGIN");
  // U::die("Circuit::update : non implant�e");
  this->partial_clear();
  logn6("Circuit::update: equilibage pour " + U::to_s(*remorque));
  this->equilibrate_eleve();

  // Mise � jour distance parcourue totale et d�s�quilibre global
  logn6("Circuit::update: mise � jour des distances du circuit " +
        U::to_s(*remorque));
  this->length = inst->get_dist(this->stations, this->remorque);
  logn5("Circuit::update END");
}

// M�thode d'�quilibrage d'un circuit
// Version stupide pour le proto des �l�ves : n'�quilibre rien du tout !
void Circuit::equilibrate_dummy() {
  logn6("Circuit::equilibrate BEGIN");

  // version de base minimaliste non optimale mais correcte :
  // - on part avec une remorque vide,
  // - rien n'est d�pos� en chaque station,
  // - on d�duire les d�s�quibres par station !
  for (const auto &station : this->stations) {
    logn7(station->to_s_long());

    // la remorque ne d�pose rien � cette station
    logn7("Circuit::equilibrate: avant maj depots");
    this->depots[station] = 0;

    // le nouveau contenu de la remorque reste donc inchang�
    logn7("Circuit::equilibrate: avant maj charges");
    this->charges[station] = charge_init;

    // incr�mentation du desequilibre du circuit
    logn7("Circuit::equilibrate: avant maj desequilibre");
    this->desequilibre += abs(station->deficit());
  }
  // Calcul savant de la charge initiale de la remorque pour garantir les
  // d�pots et retraits qui viennent d'�tre calcul�s
  this->charge_init = 0;  // m�me si c'est d�j� fait par ailleurs !
  logn6("Circuit::equilibrate END");
}

void Circuit::equilibrate() {
  this->equilibrate_dummy();
  // this->equilibrate_eleve();
}

void Circuit::equilibrate_eleve() {
  logn6("Circuit::equilibrate full BEGIN");
  int charge_min = 0;
  int charge_max = this->remorque->capa;

  // on cherche le premier entrepot en d�ficit
  int i0 = 0;
  int charge_depart = 0;
  for(const auto &station : this->stations){
    int diff = station->nbvp - station->ideal;

    // stop � la premi�re station en d�ficit
    if(diff < 0)
      break;

    // on ajoute le nombre de v�los � ajouter � la station dans le camion
    charge_depart += diff;
    i0++;
  }

  // si la charge max est d�pass�e, on la reset.
  if(charge_depart > charge_max)
    charge_depart = charge_max;

  this->charge_init = charge_depart;

  // on proc�de alors � l'�quilibrage des stations
  int charge_actuelle = charge_depart;
  for(const auto &station : this->stations) {
    int diff = station->nbvp - station->ideal;

    // station en surplus
    if(diff >= 0) {
      int charge_supplementaire = min(diff, charge_max - charge_actuelle);
      charge_actuelle += charge_supplementaire;

      station->nbvp -= charge_supplementaire;
    } else {
      int charge_deposee = min(abs(diff), charge_actuelle);
      charge_actuelle -= charge_deposee;
      station->nbvp += charge_deposee;
    }

    this->charges[station] = charge_actuelle;
    this->desequilibre += abs(station->deficit());
  }

  // logn6("Circuit::equilibrate END");
}

// Insertion d'une station dans un circuit � la position indiqu�e.
// - pos est un entier >= -1
// - si pos vaut -1 : ins�re � la fin
// - si pos vaut 0 (ou est absent) : ins�re au d�but
// - si pos vaut stations->size() : ins�re � la fin
// - si pos vaut plus que stations->size(), alors pos est forc� pour �tre
//   entre 0 et stations->size() (inclu !) par un modulo.
//
// L'appel � update est � la charge de l'appelant
//
void Circuit::insert(Station *station, int pos) {
  logn6("Circuit::insert BEGIN " + station->name + " pos=" + U::to_s(pos));
  if (pos == -1) {
    this->stations.push_back(station);
  } else {
    // On impose � pos d'�tre entre 0 et size (inclue)
    pos = pos % (1 + this->stations.size());

    auto it = this->stations.begin();
    // on avance l'it�rateur jusqu'� la position pos
    std::advance(it, pos);  // version STL
    // for (int i = 0; i < pos; ++i) {
    //     it++; // version manuelle
    // }

    // on proc�de � l'insertion
    this->stations.insert(it, station);
  }
  logn6("Circuit::insert END");
}

// Ajout d'une station � la fin du circuit.
// L'appel � update est � la charge de l'appelant
//
void Circuit::append(Station *station) { this->insert(station, -1); }

// Insertion d'une station dans un circuit � une position est al�atoire.
//
// L'appel � update est � la charge de l'appelant
//
void Circuit::insert_rand(Station *station) {
  int pos = 0;
  if (this->stations.size() != 0) {
    pos = rand() % this->stations.size() + 1;
  }
  if (log5()) {
    logn5("Circuit::insert_rand BEGIN " + this->remorque->name + ": " +
          station->name + " pos=" + U::to_s(pos));
  }
  auto it = this->stations.begin();
  // on avance l'it�rateur jusqu'� la position pos
  std::advance(it, pos);

  // on proc�de � l'insertion
  this->stations.insert(it, station);
  logn5("Circuit::insert_rand END");
}

// Cette brique d'insertion est op�rationnelle (mais suppose que la brique
// d'�quilibrage est faite), mais pas n�cessairement efficace !
//
void Circuit::insert_best(Station *station) {
  // Log::level += 0; // on peut modifier le level juste pour cette m�thode...
  string pref = "Circuit::insert_best (" + this->remorque->name + ") ";
  logn5(pref + "BEGIN " + " stations.size: " + U::to_s(this->stations.size()) +
        " insertion de:" + station->name);
  vector<Station *>::iterator best_it = this->stations.begin();
  int best_pos = 0;
  int best_cost = 999999999;

  int pos = 0;
  if (this->stations.size() == 0) {
    // circuit vide. Pas besoin de faire de pr�-insertion pour trouver la
    // meilleure position : il suffit d'ins�rer la nouvelle station � la fin
    this->stations.insert(this->stations.end(),
                          station);  // equivalent � push_bak(...)
    this->update();
  } else {
    auto it = this->stations.begin();
    // On va tester n+1 positions d'insertion (y compris apr�s la derni�re)
    while (it != this->stations.end()) {
      auto it2 = this->stations.insert(it, station);
      // On doit mettre � jour ce circuit avant d'en extraire le co�t !
      this->update();
      int cost = this->get_cost();
      if (cost < best_cost) {
        best_cost = cost;
        best_pos = pos;
        best_it = it;
      } else {
      }
      // On remet le circuit en �tat avant de passer � station suivante
      this->stations.erase(it2);
      it++;  // valable m�me si it==end()
      pos++;
    }
    // On proc�de effectivement � la meilleure insertion
    this->stations.insert(best_it, station);
    this->update();
  }
  if (log6()) {
    string msg =
        "circuit APRES insertion en bestpos=" + U::to_s(best_pos) + " :\n";
    logn6(pref + msg + this->to_s());
    logn6(pref + msg + this->to_s_long());
  }
}
// Le comportement de l'insertion est d�finie par l'option pass�
// au lancement du programme (--station-inserter alias --sinserter)
void Circuit::insert_from_option(Station *station) {
  const string sinserter = Options::args->station_inserter;
  if (sinserter == "FRONT") {
    this->insert(station);
  } else if (sinserter == "BACK") {
    this->insert(station, -1);
  } else if (sinserter == "BEST") {
    this->insert_best(station);
  } else if (sinserter == "RAND") {
    this->insert_rand(station);
  } else {
    if (sinserter == "NONE") {
      cout << "La valeur par d�faut NONE doit �tre modif�e par le solveur.\n";
    }
    U::die("station_inserter inconnu : " + U::to_s(sinserter));
  }
}

// Supprime et retourne la station en position pos ;  pos doit �tre
// entre 0 et stations.size()-1
//
Station *Circuit::erase(int pos) {
  logn5("Circuit::erase BEGIN pos=" + U::to_s(pos));
  if (pos < 0 || pos > this->stations.size() - 1) {
    raise("ERREUR Circuit::erase : pos hors borne : " + U::to_s(pos));
  }
  auto it = this->stations.begin();
  // on avance l'it�rateur jusqu'� la position pos
  std::advance(it, pos);
  Station *station = *it;  // La station � retourner avant son effacement
  this->stations.erase(it);
  logn5("Circuit::erase END");
  return station;
}

// D�place la station de la position i1 devant la station en position i2.
// i2 sera la nouvelle position apr�s insertion.
void Circuit::move(int i1, int i2) {
  if (log5()) {
    cout << "Circuit::move BEGIN "
         << " i1=" << i1 << " i2=" << i2 << endl;
  }
  if (i1 == i2) {
    return;  // rien � faire !
  }
  if (i1 < 0 || i1 > this->stations.size() - 1) {
    raise("ERREUR Circuit::move : i1 hors borne : " + U::to_s(i1));
  }
  if (i2 < 0 || i2 > this->stations.size() - 1) {
    raise("ERREUR Circuit::move : i2 hors borne : " + U::to_s(i2));
  }
  auto it = this->stations.begin();
  // on avance l'it�rateur jusqu'� la position i
  std::advance(it, i1);
  Station *station = *it;
  this->stations.erase(it);
  this->insert(station, i2);
  logn5("Circuit::move END");
}

// Supprime la station de la position i1 et la r�ins�re � la meilleure position
// possible.
void Circuit::move_best(int i1) {
  if (log5()) {
    cout << "Circuit::move_best BEGIN "
         << " i1=" << i1 << endl;
  }
  if (i1 < 0 || i1 > this->stations.size() - 1) {
    raise("ERREUR Circuit::move_best : i1 hors borne : " + U::to_s(i1));
  }
  auto it = this->stations.begin();
  // on avance l'it�rateur jusqu'� la position i
  std::advance(it, i1);
  Station *station = *it;
  this->stations.erase(it);
  this->insert_best(station);
  logn5("Circuit::move_best END");
}

// D�place la station de la position i1 de l'objet courant devant la station
// en position i2 de l'objet other
// i2 sera la nouvelle position apr�s insertion
// i1 \in [0, size1-1]
// i2 \in [0, size2]  (size2 inclu pour ajout en fin du circuit c2)
void Circuit::move_to(int i1, Circuit *other, int i2) {
  if (log5()) {
    cout << "Circuit::move_to BEGIN "
         << " i1=" << this->remorque->name << "/" << i1
         << " i2=" << other->remorque->name << "/" << i2 << endl;
  }
  if (i1 < 0 || i1 > this->stations.size() - 1) {
    raise("ERREUR Circuit::move_to : i1 hors borne : " + U::to_s(i1));
  }
  if (i2 < 0 || i2 > other->stations.size()) {
    raise("ERREUR Circuit::move_to : i2 hors borne : " + U::to_s(i2));
  }
  auto it = this->stations.begin();
  // on avance l'it�rateur jusqu'� la position i
  std::advance(it, i1);
  Station *station = *it;
  this->stations.erase(it);
  other->insert(station, i2);
  logn5("Circuit::move_to END");
}

// Supprime la station de la position i1 de l'objet courant et l'ins�re dans
// le circuit other � la meilleures position possible.
// i1 \in [0, size1-1]
//
void Circuit::move_best_to(int i1, Circuit *other) {
  if (log5()) {
    cout << "Circuit::move_best_to BEGIN "
         << " i1=" + this->remorque->name + "/" << i1 << endl;
  }
  if (i1 < 0 || i1 > this->stations.size() - 1) {
    raise("ERREUR Circuit::move_best_to : i1 hors borne : " + U::to_s(i1));
  }
  auto it = this->stations.begin();
  // on avance l'it�rateur jusqu'� la position i
  std::advance(it, i1);
  Station *station = *it;
  this->stations.erase(it);
  other->insert_best(station);
  logn5("Circuit::move_best_to END");
}

// inversion d'un chemin dans le circuit (2opt)
// inverse le chemin de parcours des stations comprises entre i1 (inclue) et
// i2 (exclue)
//
// Principe :
// - on swap les stations deux � deux en partant des extr�mit�s
//   de la branche (i1 et i2-1) et en se rapprochant jusqu'au centre.
//
// - EFFETS DE BORD : exit() si bornes incorrectes
// - LA SOLUTION N'EST PAS MISE � JOUR ICI ! (pas de update)
//
void Circuit::reverse_branch(int i1, int i2) {
  //
  // 1. v�rification des param�ters i1 et i2 (on assure i1 < i2)
  //
  if (i1 < 0 || i1 >= this->stations.size()) {  //  ATTENTION >= ici
    raise("ERREUR reverse_branch : i1 hors borne : " + U::to_s(i1));
  }
  if (i2 < 0 || i2 > this->stations.size()) {  //  ATTENTION > ici
    raise("ERREUR reverse_branch : i2 hors borne : " + U::to_s(i2));
  }
  if (i1 > i2) {
    int tmp = i2;
    i2 = i1;
    i1 = tmp;
  } else if (i1 == i2) {
    return;  // rien � faire
  } else if (i2 - i1 <= 1) {
    return;  // rien � faire si �gale ou si diff�rence de 1
  }
  // assert i1 < i2

  //
  // 2. inversion de la branche
  //
  i2--;  // pour pointer sur le dernier �l�ment, et non pas le post-dernier !
  while (i2 > i1) {
    Station *s_tmp = this->stations[i1];
    this->stations[i1] = this->stations[i2];
    this->stations[i2] = s_tmp;
    i1++;
    i2--;
  }
}
// Suppression d'une branche de ce circuit de i1 (inclu) � i2 (exclue) et
// insertion dans le circuit other devant la position i3.
// Si reverse vaut true, alors la branche est invers�e avant insertion
// Domaines des param�tres :
// - i1 : [0,size1[ (size1 exclu)
// - i2 : [1,size1] (size1 inclu)
// - i3 : [0,size2] (size2 inclu)
// - i1 <= i2
// Si |i2-i1| == 1 alors on d�place l'�l�ment [i1]
// Si i1 == i2     alors on ne fait rien
//
// - EFFETS DE BORD : exit() si bornes incorrectes ou si this==other
// - LA SOLUTION N'EST PAS MISE � JOUR ! (pas de update)
//
void Circuit::move_branch_to(int i1, int i2, Circuit *other, int i3,
                             bool reverse) {
  // U::die("Circuit::move_branch_to non implant�e !\n");
  if (this == other) {
    cerr << "move_branch_to ERREUR : this et other m�me object ! " << endl;
    exit(1);
  }
  // 07/12/2017 TODO (n�cessite post-v�rification : mettre i1 < 1)
  if (i1 < 0 || i1 >= this->stations.size()) {  //  ATTENTION >= ici
    cerr << "move_branch_to ERREUR : i1 hors borne : " << i1 << endl;
    exit(1);
  }
  if (i2 < 0 || i2 > this->stations.size()) {  //  ATTENTION > ici
    cerr << "move_branch_to ERREUR : i2 hors borne : " << i2 << endl;
    exit(1);
  }
  if (i3 < 0 || i3 > other->stations.size()) {  //  ATTENTION > ici
    cerr << "move_branch_to ERREUR : i3 hors borne : " << i3 << endl;
    exit(1);
  }
  if (i1 > i2) {
    cerr << "move_branch_to ERREUR : i1=" << i1 << " > i2=" << i2 << endl;
    exit(1);
  }

  auto it1 = this->stations.begin();
  advance(it1, i1);

  auto it2 = this->stations.begin();  // optimisable en partant de it2 := it1)
  advance(it2, i2);

  auto it3 = other->stations.begin();
  advance(it3, i3);

  ////////////////////////////////////
  // 1. Construction du vector de la tranche � transf�rer
  vector<Station *> tmp_stations = vector<Station *>(i2 - i1);
  for (auto i = i1; i < i2; i++) {
    // si reverse est true : on commence par la fin
    auto dest_i = reverse ? (i2 - 1 - i) : i - i1;
    tmp_stations[dest_i] = this->stations[i];
  }
  // 2. suppression de la branche du circuit source
  this->stations.erase(this->stations.begin() + i1,
                       this->stations.begin() + i2);
  // 3. insertion de la branche dans le circuit destination
  //    http://fr.cppreference.com/w/cpp/container/vector/insert
  other->stations.insert(it3, tmp_stations.begin(), tmp_stations.end());
  ////////////////////////////////////

}
// D�placement d'une branche [i1,i2[ de ce circuit en position i3
// avec inversion �ventuelle.
// Le bloc contenant la branche [i1,i2[ commence donc en i3
// Pr�condition : i3 <= size-(i2-i1).
//
// Domaines des param�tres :
// - i1 : [0,size[ (size exclu)
// - i2 : [1,size] (size inclu)
// - i3 : [0,size] (size inclu)
// - i1 <= i2
//
// Principe :
// 0. si (i1=i3 et reverse==false) return (rien � faire).
// 1. Cr�er un vector temporaire
// 2. y copier la branche [i1,i2[
//    (avec inversion �ventuelle selon option reverse).
// si i3 < i1
//   3.1 avancer les �l�ments depuis i3 jusqu'� i1-1 de +(i2-i1)
// sinon si i3 > i1
//   3.2 reculer les �l�ments depuis i2 jusqu'� (i2-1)+(i3-i1) de (i2-i1)
// fin
// 4 recopier le vector tmp depuis i3 (jusqu'� i3+i2-i1)
//
// Exemple
//  circuit :     0 1 2 a b c d e 8 9    3,8,_
//  circuit :     0 1 2 8 a b c d e 9    3,8,4
//  circuit :     0 1 2 8 9 a b c d e    3,8,5  (i3=6 interdit)
//  circuit :     0 1 a b c d e 2 8 9    3,8,2
//  circuit :     0 a b c d e 1 2 8 9    3,8,1
//  circuit :     a b c d e 0 1 2 8 9    3,8,0
//
//  circuit :     0 1 2 3 4 5 a b 8 9    6,8,_
//  circuit :     0 1 2 a b 3 4 5 8 9    6,8,3
//  circuit :     a b 0 1 2 3 4 5 8 9    6,8,0
//  circuit :     0 1 2 3 4 5 8 9 a b    6,8,8
//
//
void Circuit::move_branch(int i1, int i2, int i3, bool reverse) {
  if (i1 == i3 && reverse == false) return;     // rien � faire !
  if (i1 < 0 || i1 >= this->stations.size()) {  //  ATTENTION >= ici
    cerr << "move_branch ERREUR : i1 hors borne : " << i1 << endl;
    exit(1);
  }
  if (i2 < 1 || i2 > this->stations.size()) {  //  ATTENTION > ici
    cerr << "move_branch ERREUR : i2 hors borne : " << i2 << endl;
    exit(1);
  }
  if (i3 < 0 || i3 > this->stations.size()) {  //  ATTENTION > ici
    cerr << "move_branch ERREUR : i3 hors borne : " << i3 << endl;
    exit(1);
  }
  if (i1 > i2) {
    cerr << "move_branch ERREUR : i1=" << i1 << " > i2=" << i2 << endl;
    exit(1);
  }

  // Construction du vecteur temporaire d ela tranche � d�placer
  vector<Station *> tmp_stations = vector<Station *>(i2 - i1);
  for (auto i = i1; i < i2; i++) {
    // si reverse est true : on commence par la fin
    auto dest_i = reverse ? (i2 - 1 - i) : i - i1;
    tmp_stations[dest_i] = this->stations[i];
  }
  int len = i2 - i1;  // longueur de la chaine
  // Trois cas possibles :
  if (i3 < i1) {
    // il faut avancer le bloc [i3, i1-1] de len
    for (int i = i1 - 1; i >= i3; i--) {
      this->stations[i + len] = this->stations[i];
    }
  } else if (i2 <= i3) {
    // il faut reculer le bloc [i2, i3+len-1] de len
    for (int i = i2; i <= i3 + len - 1; i++) {
      this->stations[i - len] = this->stations[i];
    }
  } else {  // assert (i1 < i3 < i2)
    // il faut avancer le petit bloc [i2, i2+(i3-i1-1)] de len
    // (ce petit bloc passe de gauche � droite de la branche)
    for (int i = i2; i <= i2 + (i3 - i1 - 1); i++) {
      // A_FINIR();
      this->stations[i - len] = this->stations[i];
    }
  }
  // R�ins�rer le tableau tampon
  for (int i = 0; i < tmp_stations.size(); i++) {
    this->stations[i3 + i] = tmp_stations[i];
  }
}

string Circuit::to_s() const {
  stringstream buf;
  buf << "# Circuit associ� � la remorque " << this->remorque->name
      << " de capa " << this->remorque->capa << endl;
  buf << "#       id, charge_init, desequ, longueur\n";
  buf << "circuit " << this->remorque->name << "        " << this->charge_init
      << "        " << this->desequilibre << "       " << this->length << endl;
  for (const auto &station : stations) {
    // TODO : � AJOUTER CECI DANS MA FAQ (rubrique : "probl�mes avec const"!
    // ATTENTION : on doit utiliser depots.at() au lieu de depots[]
    //             car le r�sultat de this->depots[] n'est pas const !
    // ATTENTION : this-> depots[station] est interdit ici
    //             car to_s() declar�e const !!
    // buf << "  " << station->name << " " << this->depots[station] << endl;
    buf << "  " << station->name << " " << this->depots.at(station) << endl;
  }
  buf << "end" << endl;
  return buf.str();
}
string Circuit::to_s_lite() const {
  ostringstream buf;
  buf << this->remorque->name << ": " << this->charge_init << "/"
      << this->remorque->capa << " deseq=" << this->desequilibre
      << " length=" << this->length << " " << inst->to_s(this->stations)
      << endl;
  return buf.str();
}

// Affichage d�taill� d'une solution (format non standard mais plus d�taill�) !
string Circuit::to_s_long() const {
  stringstream buf;
  buf << "# Circuit d�taill� associ� � la remorque " << this->remorque->name;
  buf << " de capa " << this->remorque->capa << endl;
  buf << "#   charge_init=" << this->charge_init << endl;
  buf << "#   desequilibre=" << this->desequilibre << endl;
  buf << "#   distance=" << this->length << endl;

  Place *src = this->remorque;
  for (const auto &dst : stations) {
    const Arc *arc = this->inst->get_arc(src, dst);
    // this-> depots[arc.dst.id] interdit ici car to_s_long() declar�e const !!
    // int depot = this->depots.at(static_cast<Station*>(arc->dst));
    int depot = this->depots.at((Station *)(arc->dst));
    int charge = this->charges.at((Station *)(arc->dst));
    buf << "   " << arc->to_s_long() << " depot de " << depot
        << " => charge = " << charge << endl;
    src = dst;
  }
  if (stations.size() != 0) {
    buf << "   " << inst->get_arc(stations.back(), this->remorque)->to_s_long();
  }
  return buf.str();
}
// G�n�re une erreur sp�cifique au circuit, et stoppe le programmme
void Circuit::raise(string str) {
  stringstream buf;
  buf << str << endl;
  buf << this->remorque->name << endl;
  buf << this->to_s_lite() << endl;
  throw buf.str();
}
//./
