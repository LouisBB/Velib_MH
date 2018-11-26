#include "instance.hpp"
#include <regex>

// Voir fichier instance.hpp pour les exemples de fichier d'instance et de
// solution
//

// Retourne le meilleur cout connu de l'instance
int Instance::best_known_cost() const {
  string abbrev = this->get_shortname();
  if (abbrev == "v0") return 355;         //  v0-0-355.sol
  if (abbrev == "v1") return 1000000000;  //
  if (abbrev == "v2") return 1000000000;  //
  if (abbrev == "v3") return 1000000000;  //
  if (abbrev == "v4") return 1000000000;  //
  if (abbrev == "v5") return 1000000000;  //
  if (abbrev == "v6") return 1000000000;  //
  if (abbrev == "v7") return 1000000000;  //
  if (abbrev == "v8") return 1000000000;  //
  if (abbrev == "v9") return 1000000000;  //

  return U::MY_INT_MAX;  // en cas d'erreur dans l'abbrev
}

void Instance::init() {
  this->name = "";
  this->filename = "";

  this->remorques = vector<Remorque*>();
  this->stations = vector<Station*>();
  this->places = vector<Place*>();

  // this->best_sol = new Solution(this);
}

Instance::Instance() {
  logn0("Instance constructeur BEGIN (d=sans fichier) \"");
  this->init();
  logn1("Instance constructeur FIN\n");
}

/// Instance::Instance(string filename, Args* args) {
Instance::Instance(string const filename) {
  logn0("Instance constructeur BEGIN pour \"" + filename + "\" ...\n");
  this->init();
  this->filename = filename;
  this->read_velib_instance();
  this->build_arcs();
  if (log6()) {
    cout << "Regénération de l'instance\n";
    cout << this->to_s();
  }
  logn1("Instance constructeur FIN\n");
}

Instance::~Instance() {
  // Les destructions à faire
  // - delete des places et vecteur de places (stations et remorques)
  // - delete des objets de arcs et de arcs
  // - vider la map arcs_map et delete de arcs_map
  // - vider le tableau dist et delete dist
  // Mais pour l'instant on ne crée qu'une seule Instance par exécution
  // => destruction automatique en fin de programme

  for (auto&& remorque : this->remorques) {
    delete remorque;
  }
  for (auto&& arc : this->arcs) {
    delete arc;
  }

  // VERSIONS EXPLICITES (SANS LES "auto")
  // vector<Place*>::iterator it = places.begin();
  // while (it != places.end()) {
  //     delete *it;
  //     it++;
  // }
  // delete places;
  // delete remorques;
  // delete stations;
  // for (vector<Arc*>::iterator it = arcs.begin(); it != arcs.end(); it++) {
  //     delete *it;
  // }
  // delete arcs;
  // delete arcs_grid;
}

void Instance::read_velib_instance() {
  Log::level -= 3;

  // Initialisation de la variable d'état "state" (INUTILE ICI !)
  //
  // Cette variable d'état de type énuméré (ou assimilé) permet d'adapter
  // le traitement du fichier en fonction de la zone du fichier qui a déjà été
  // lu.
  //
  // Cependant, dans le format d'instance de ce problème (ie. velib), chaque
  // ligne est indépendante : on n'a pas besoin connaitre l'historique de
  // l'analyse pour savoir comment interpréter une ligne.  On n'a donc pas
  // besoin d'une variable d'état pour représenter le passé.
  //
  // Exemples de différents états possibles :
  // "HEADER_STATE" :
  //      état initial, et en cours lecture en-tete
  // "STATION_STATE" :
  //      en cours lecture de la liste de noeuds (stations)
  // "DONE_STATE" :
  //      quand l'ensemble des stations est lu
  //
  // string state = "HEADER_STATE";

  int line_idx = 0;
  int nb_remorques = 0;  // nb de remorques prétendue dans le fichier d'instance
  int nb_stations = 0;   // nb de stations prétendue dans le fichier d'instance

  vector<string> lines = U::read_lines(this->filename);
  // vector<string>::iterator  it_line = lines.begin();
  auto it_line = lines.begin();  // C++11 seulement
  while (it_line != lines.end()) {
    string line = *it_line;
    it_line++;
    line_idx++;
    if (log5()) {
      cout << "#### line:" << line_idx << " brute (" << line.size()
           << " chars) => \"" << line << "\"\n";
    }
    line = U::trim_string(line);
    if (log4()) {
      cout << "#### line:" << line_idx << " (" << line.size() << " chars) => \""
           << line << "\"\n";
    }

    if (line.empty()) {
      logn4("#### Ligne blanche : on saute.");
      continue;
    }
    if (line.find("//") == 0 || line.find("#") == 0) {
      logn4("#### Ligne commentaire : on saute.");
      continue;
    }
    if (line == "END") {
      logn4("#### Fin normale de fichier (mot clé \"END\")");
      break;
    }
    // On doit traiter une ligne utile

    // La ligne doit être de la forme "key xxxx xxx xxx"
    string key = "";
    string val = "";
    if (!U::extract_key_val(line, key, val)) {
      cerr << "\nFormat de ligne incorrect pour la ligne \"" << line << "\""
           << endl;
      exit(1);
    }
    if (log4()) {
      cout << "     Détection key=" << key << " val=\"" << val << "\"\n";
    }
    //
    // On connait les chaines key et val
    // - key peut contenir "version",  "remorque", ...
    // - val peut contenir "1.0", "r1  25  25  6", "s1  10  50  11  8  8"...
    //

    if (key == "velib") {
      stringstream buf(val);
      buf >> name;
      buf >> nb_remorques;
      buf >> nb_stations;
      logn4("     Extraction de name => name=" + name +
            " nb_remorques=" + ::to_s(nb_remorques) +
            " nb_stations=" + ::to_s(nb_stations) + "\n");
      continue;

    } else if (key == "version") {
      stringstream buf(val);
      float version;  // ne sera pas utilisé
      buf >> version;
      logn4("     Extraction de version=" + ::to_s(version) + "\n");
      continue;

    } else if (key == "remorque") {
      stringstream buf(val);
      this->add_remorque(val);
      continue;

    } else if (key == "station") {
      this->add_station(val);
      continue;

      // } else if (key == "end") {
      //     // On ignore le reste (éventuel) du fichier
      //     break;

    } else {
      cout << "clé inconnu pour la ligne \"" << line << "\n" << endl;
      exit(1);
    }
  }  // end while

  //
  // Test de cohérence de l'instance
  //
  if ((unsigned)nb_remorques != remorques.size()) {
    cerr << "ATTENTION nb_remorques=" << nb_remorques
         << " INCOHERENT AVEC LE NOMBRE DE "
         << "REMORQUES LUES : " << remorques.size() << endl;
  }
  if ((unsigned)nb_stations != stations.size()) {
    cerr << "ATTENTION nb_stations=" << nb_stations
         << " INCOHERENT AVEC LE NOMBRE DE "
         << "STATIONS LUES : " << stations.size() << endl;
  }
  logn2(::to_s(line_idx) + " lignes ont ete lues.");
  Log::level += 3;
}

void Instance::add_remorque(const string& partial_line) {
  stringstream stm(partial_line);
  string name;
  stm >> name;
  int x;
  stm >> x;
  int y;
  stm >> y;
  int capa;
  stm >> capa;

  // int id = this->remorques.size(); // premier id commence en 0
  int id = this->remorques.size() + this->stations.size();
  Remorque* remorque = new Remorque(id, name, x, y, capa);
  logn4("Remorque créé : " + remorque->to_s_long());
  remorques.push_back(remorque);
  places.push_back(remorque);
}
void Instance::add_station(const string& partial_line) {
  stringstream stm(partial_line);
  string name;
  stm >> name;
  int x;
  stm >> x;
  int y;
  stm >> y;
  int capa;
  stm >> capa;
  int ideal;
  stm >> ideal;
  int nbvp;
  stm >> nbvp;

  // int id = this->stations->size(); // premier id commence en 0
  int id = this->remorques.size() + this->stations.size();
  Station* station = new Station(id, name, x, y, capa, ideal, nbvp);
  logn4("Station créée : " + station->to_s_long());
  stations.push_back(station);
  places.push_back(station);
}
// Construction des collections (listes, ...) d'arcs de l'instance.
// On suppose que tous les places (Remorques ou Stations) sont créés.
//
// Rappel :
//   Le champs id est unique pour une place donc une Remorques et une Station ne
//   peuvent jamais avoir le même id.
//
// - arcs_grid est un vector représentant une matrice dont les coordonnées
//   sont les ids (internes) des places et dont la valeur est l'arc entre ces
//   deux places. Sera utilisé pour accéder à un arc correspondant à partir de
//   deux places connus
//
// - arcs est un vector des arcs créés
//
// Principe :
// On crée **tous** les arcs (même si non valides)
// Les arcs inverses sont également créés car les deux boucles imbriquées
// vont de 0 à nb_place-1.
//
void Instance::build_arcs() {
  // arcs_map = new map<int, Arc*>();
  // Log::level += 5;
  int nb_places = this->get_nb_places();
  // cout << "xxxxx nb_places=" << nb_places <<"\n";
  this->arcs_grid = vector<Arc*>(nb_places * nb_places);
  this->arcs = vector<Arc*>();  // liste des arcs valides
  this->dists_grid = vector<int>(nb_places * nb_places);
  for (int i1 = 0; i1 < nb_places; i1++) {
    // cout << "i1=" << i1 << "\n";
    Place* place1 = places[i1];
    for (int i2 = 0; i2 < nb_places; i2++) {
      // cout << "  i2=" << i2 << "\n";
      Place* place2 = places[i2];
      // const Arc& arc = Arc(place1, place2);
      Arc* arc = new Arc(place1, place2);
      logn7("Arc créé : " + arc->to_s_long());
      // logn7("    src : " + arc->src->to_s_long());
      // logn7("    dst : " + arc->dst->to_s_long());
      // arcs_grid[nb_places*place1.id + place2.id] = arc;
      this->arcs_grid[nb_places * place1->id + place2->id] = arc;
      this->dists_grid[nb_places * place1->id + place2->id] = arc->dist;
      if (arc->is_feasible()) {
        arcs.push_back(arc);
      }
    }
  }
  // Log::level -= 5;
}

// Attention, l'arc retourné n'est pas forcément valide !
Arc* Instance::get_arc(int i1, int i2) const {
  // return (*arcs_grid)[i1 * this->get_nb_places() + i2];
  return arcs_grid[i1 * this->get_nb_places() + i2];
}

Arc* Instance::get_arc(const Place* p1, const Place* p2) const {
  return get_arc(p1->id, p2->id);
}

int Instance::get_dist(const Place* p1, const Place* p2) const {
  // return dists_grid[p1->id * this->get_nb_sites() + p2->id];;
  return this->get_arc(p1, p2)->dist;
}

// Construit les arcs correspondant au circuit fermé partant de la remorque et
// déservant les stations
//
// La gestion du vector arcs (création et suppression) est entièrement à la
// charge de l'appelant.
//
void Instance::fill_arcs(vector<Arc*>& arcs, const vector<Station*>& stations,
                         const Remorque* remorque) {
  logn6("Instance::fill_arcs with remorque BEGIN");
  arcs.clear();
  if (stations.empty()) {
    return;
  }
  arcs.push_back(this->get_arc(remorque, stations.front()));
  for (int i = 0; i <= stations.size() - 2; ++i) {
    Arc* arc = this->get_arc(stations[i], stations[i + 1]);
    arcs.push_back(arc);
  }
  arcs.push_back(this->get_arc(stations.back(), remorque));
  logn7("Instance::fill_arcs stations: " + this->to_s(stations));
  logn7("Instance::fill_arcs arcs: " + this->to_s(arcs));
  logn6("Instance::fill_arcs with remorque END");
}

// Calcule la distance de parcours correspondant aux stations et à la remorque
// en paramètre
//
// Si remorque est non NULL, la distance correspond aux circuit fermé de la
// Remorque desservant les stations.
// Si remorque est NULL, la distance correspond au chemin ouvert reliant les
// places (qui sont des stations) dans l'ordre.
//
int Instance::get_dist(const vector<Station*>& stations,
                       const Remorque* remorque) const {
  logn6("Instance::get_dist stations BEGIN");
  if (stations.empty()) {
    return 0;
  }
  int dist = 0;
  Place* src = stations.front();
  Place* dst = nullptr;
  // On commence à partir de la seconde station grâce au ++ initial
  for (auto it = ++stations.begin(); it != stations.end(); it++) {
    dst = *it;
    // dist += this->get_dist(src, dst);
    dist += this->get_arc(src, dst)->dist;
    src = dst;
  }
  if (remorque != nullptr) {
    dist += this->get_dist(remorque, stations.front());
    dist += this->get_dist(stations.back(), remorque);
  }

  logn6("Instance::get_dist END avec dist=" + U::to_s(dist));
  return dist;
}

// nombre de vols de velib (peut-être négatif s'il y a trop de vélos)
int Instance::get_nb_vols() const {
  U::die("ERREUR: Instance::get_nb_vols non implantée !");
  return 0;
}

// Capacité de la plus grosse remorque
int Instance::get_max_remorque_capa() const {
  int capa_max = 0;
  for (auto& r : this->remorques) {
    capa_max = max(capa_max, r->capa);
  }
  return capa_max;
}
// Total des capacités des remorques
int Instance::get_total_remorque_capa() const {
  U::die("ERREUR: Instance::get_nb_vols non implantée !");
  return 0;
}

// Valeur maxi du déséquilibre sur l'ensemble des stations
int Instance::get_max_station_desequilibre() const {
  int max_deseq = 0;
  for (auto& s : this->stations) {
    // max_deseq = max(max_deseq, abs(s->ideal - s->nbvp));
    max_deseq = max(max_deseq, abs(s->deficit()));
  }
  return max_deseq;
}
// Cumul des excès des déséquilibres des stations
// L'excès d'un déséquilibre est la valeur qui ne pourra jamais être
// ré-équilibrée compte tenu de la capacité de la plus grossse remorque.
int Instance::get_total_excess_station_desequilibre() const {
  U::die("ERREUR: Instance::get_total_excess_station_desequilibre non implantée !");
  return 0;
}

// Cumul des déséquilibres dus au vols.
// Dépend de la capacité totale des remorques diponibles
int Instance::get_total_vols_desequilibre() const {
  U::die("ERREUR: Instance::get_total_vols_desequilibre non implantée !");
  return 0;
}

int Instance::get_lowerbound_dist() const {
  int minlength = 0;
  for (auto& s : this->stations) {
    int mindist1 = U::MY_INT_MAX;
    int mindist2 = U::MY_INT_MAX;
    for (auto& p : this->places) {
      if (p == s) continue;
      int dist = this->get_dist(s, p);
      if (dist < mindist1) {
        mindist2 = mindist1;
        mindist1 = dist;
      } else if (dist < mindist2) {
        mindist2 = dist;
      }
    }
    minlength += mindist1 + mindist2;
  }
  return minlength / 2;
}

// Affiche l'instance courante dans un format réutilisable en entrée
//
void Instance::print_instance(ostream& os) const { os << to_s(); }

// Enregistre l'instance courante dans un fichier
// Attention : si le fichier existe déjà : il sera ecrasé !
//
void Instance::print_instance(const string& filename) const {
  ofstream fid(filename.c_str(), ios::out);
  if (!fid) {
    string msg = "impossible de créer \"" + filename + "\"";
    cerr << msg;
    exit(1);
  }
  fid << to_s();
}

// Nom court de l'instance de l'instance construit à partir du nom complet.
// Si le nom court ne peut pas être déduit, retourne le nom complet.
//
// L'abréviation est construite en prenant la premiere lettre (e.g. v de velib)
// puis tous les chiffres encadrés par les deux "_" suivants
// e.g : on extrait v2 à partir de velib_2_s020_r2_k5..11_v15
//
// Utilise les regexp de c++ : voir // http://en.cppreference.com/w/cpp/regex
//
string Instance::get_shortname() const {
  // logn1("Instance::get_shortname START");
  // std::string abbrev_pat = "^(\\w)\\w+_([[:digit:]]+)_.*$"; //ok
  std::string abbrev_pat = "^(\\w)\\w+_([\\d]+).*$";
  // logn1("Instance::get_shortname START" + U::to_s(abbrev_pat));
  std::regex abbrev_re(abbrev_pat);

  std::smatch m;

  string shortname = "";
  regex_search(this->name, m, abbrev_re);
  if (!m.empty()) {
    shortname = m[1].str() + m[2].str();
    return shortname;
  } else {
    return this->name;
  }
}

string Instance::to_s() const {
  stringstream buf;
  buf << "# Instance Velib regeneree" << endl;
  buf << "velib " << name << " " << remorques.size() << " " << stations.size()
      << endl;
  buf << "version 1.0 " << endl;
  buf << endl;
  buf << "#remorque  id    x   y    K" << endl;
  for (unsigned i = 0; i < remorques.size(); i++) {
    buf << remorques[i]->to_s() << endl;
  }
  buf << endl;
  buf << "#station   id    x   y    capa ideal nbvp" << endl;
  for (unsigned i = 0; i < stations.size(); i++) {
    buf << stations[i]->to_s() << endl;
  }
  return buf.str();
}

string Instance::to_s_long() const {
  stringstream buf;
  buf << "# Instance Velib regeneree" << endl;
  buf << "velib " << name << " " << remorques.size() << " " << stations.size()
      << endl;
  buf << "version 1.0 " << endl;
  buf << endl;
  buf << "# détail de chaque place dans l'ordre de création" << endl;
  for (const auto& place : this->places) {
    buf << place->to_s_long() << endl;
  }
  return buf.str();
}

// void Instance::test_get_arc(const Instance* inst)  {
//     cout << "Test acces arc (1,2) : to_s, src, dst et dist\n";
//     // Edge* edge = b->get_edge(1,2);
//     cout << "pp\n";
//     cout << "to_s()=     " << inst->get_arc(1,2)->to_s() << endl;
//     cout << "src->to_s()=" << inst->get_arc(1,2)->src.to_s() << endl;
//     cout << "src->to_s()=" << inst->get_arc(1,2)->dst.to_s() << endl;
//     cout << "dist=       " << inst->get_arc(1,2)->dist << endl;
// }

// Recherche d'un arc (linéaire en nbre d'arcs)
//
Arc* Instance::find_arc(const string& s_name, const string& d_name) const {
  for (auto& arc : this->arcs) {
    if (arc->src->name == s_name && arc->dst->name == d_name) {
      return arc;
    }
  }
  cerr << "ERREUR : arc introuvable pour src=" << s_name << " dst=" << d_name
       << endl;
  U::die("ERREUR : abondon !");
  return nullptr;
}

// Recherche d'une station (linéaire en nbre de stations)
//
Station* Instance::find_station(const string& name) const {
  for (const auto& station : this->stations) {
    if (station->name == name) {
      return station;
    }
  }
  cerr << "ERREUR : station \"" << name << "\" introuvable" << endl;
  U::die("ERREUR : abondon !");
  return nullptr;
}

Instance* Instance::new_velib_mini() {
  // U::die("new_velib_mini: non implantée !");

  // On crée l'instance mini du sujet velib (2 remorques pour 8 stations).
  // Les stations ou remorques peuvent être ajoutées dans un ordre arbitraire.
  Instance* inst = new Instance();
  inst->name = "mini_gen";
  inst->add_remorque("r2 75     75 4");
  inst->add_station(" s2    20 90   9 7 3  ");

  inst->add_remorque("r1   25 25 5");
  inst->add_station("s1   10 50   11 8 8");

  inst->add_station("s3   25  60      10     7   10");
  inst->add_station("s4   45  80       6     4    5");
  inst->add_station("s5   60  60       7     5    7");
  inst->add_station("s6   60  20       6     2    6");
  inst->add_station("s7   80  50       7     5    7");
  inst->add_station("s8   95  85       7     7    2");

  inst->build_arcs();

  // Je choisis de trier les tableaux de Remorques ou de Stations sur le champ
  // name. La solution suivante utilise une fonction anonyme pour le tri. Elle
  // nécessite cependant une version récente d'un compilateur c++.
  //   sort(inst->stations->begin(),
  //        inst->stations->end(),
  //        [](const Station*  a, const Station*  b) ->bool{
  //               return a->name < b->name;
  //        }
  //   );
  // ATTENTION : cette fonction sort ne fonctionne qu'avec une collection
  // dispose de random access RandomIt.
  // Si stations était une liste au lieu d'un verctor, il faudrait faire :
  //   stations->sort(Place::compareByName);
  //
  sort(inst->stations.begin(), inst->stations.end(), Place::compareByName);
  sort(inst->remorques.begin(), inst->remorques.end(), Place::compareByName);

  return inst;
}
Instance* Instance::new_velib_mini_mono_remorque() {
  // U::die("new_velib_mini: non implantée !");

  // On crée l'instance mini du sujet velib (2 remorques pour 8 stations).
  // Les stations ou remorques peuvent être ajoutées dans un ordre arbitraire.
  Instance* inst = new Instance();
  inst->name = "mini_gen_mono";
  inst->add_remorque("r1   25  25  5");
  inst->add_station("s1   10  50      11     8    8");
  inst->add_station("s2   20  90       9     7    3");
  inst->add_station("s3   25  60      10     7   10");
  inst->add_station("s4   45  80       6     4    5");
  inst->add_station("s5   60  60       7     5    7");
  inst->add_station("s6   60  20       6     2    6");
  inst->add_station("s7   80  50       7     5    7");
  inst->add_station("s8   95  85       7     7    2");
  inst->build_arcs();
  return inst;
}

//./
