#include "solution.hpp"

Solution::Solution(const Instance *inst) : inst(inst) {
  // this->inst = inst;
  this->circuits = vector<Circuit *>(this->inst->remorques.size());
  for (unsigned i = 0; i < inst->remorques.size(); i++) {
    Remorque *remorque = inst->remorques[i];
    Circuit *circ = new Circuit(inst, remorque);
    this->circuits[i] = circ; // marche car circuits est prédimentionné
  }

  // La solution est invalide à la construction : on invente un coût
  // "infini" grâce à une longueur arbitrairement élevée !
  // this->length = 0;
  this->length = 999999999;
  this->desequilibre = 0;
}
Solution::Solution(const Solution *other) : inst(other->inst) {
  // this->inst = other->inst;
  this->length = other->length;
  this->desequilibre = other->desequilibre;

  this->circuits = vector<Circuit *>(this->inst->remorques.size());
  for (unsigned i = 0; i < this->inst->remorques.size(); i++) {
    Circuit *circ = new Circuit(other->circuits[i]);
    this->circuits[i] = circ;
  }
}

Solution::~Solution() {
  // for (unsigned i = 0; i < inst->remorques.size(); i++) {
  //     delete this->circuits[i];
  // }
  for (const auto &circ : this->circuits) {
    delete circ;
  }
}

// On dispatche les stations dans les circuits.
// VERSION CONFORME À STUPIDSOLVER
// e.g: r1: s1 s3 s5 s7
//      r2: s2 s4 s6 s8
//
void Solution::solve_stupid() {
  this->clear();
  // Principe : On prend les stations dans l'ordre de l'instance, puis
  // on les affecte à chaque remorque à tour de rôle.
  //
  int remorque_id = -1;
  for (unsigned sid = 0; sid < inst->stations.size(); sid++) {
    // on extrait la prochaine station à visiter
    Station *station = inst->stations[sid];
    // sélection des remorques à tour de rôle
    remorque_id = (remorque_id + 1) % inst->remorques.size();
    // on extrait le circuit associé à la remorque sélectionnée
    Circuit *circ = this->circuits[remorque_id];
    // on ajoute la station en fin de ce circuit
    circ->stations.push_back(station);
  }
  this->update();
}

// On repartit les stations dans les circuits.
// VERSION ALTERNATIVE À CONSERVER (stations ordonnées différement)
// e.g: r1: s1 s2 s3 s4
//      r2: s5 s6 s7 s8
//
void Solution::solve_stupid_bis() {
  this->clear();
  // prochaine station de l'instance à visiter
  int sid = 0;
  // nombre approximatif de station par remorque
  int st_by_rem = round(inst->stations.size() / inst->remorques.size());
  for (unsigned i = 0; i < inst->remorques.size() - 1; i++) {
    // on rempli tous les circuits sauf le dernier
    Circuit *circ = this->circuits[i];
    int last_sid = sid + st_by_rem;
    while (sid < last_sid) {
      circ->stations.push_back(inst->stations[sid]);
      sid++;
    }
  }
  // les stations restantes sont visitées par la dernière remorque
  Circuit *last_circuit = this->circuits.back();
  while (sid < inst->stations.size()) {
    last_circuit->stations.push_back(inst->stations[sid]);
    sid++;
  }
  this->update();
}

void Solution::solve_random() {
  this->clear();

  // On crée une liste de stations dans un ordre aléatoire
  auto stations = vector<Station *>(inst->stations);
  random_shuffle(stations.begin(), stations.end());

  // Tant que la solution générée n'est pas valide (remorque non utilisée, ...)
  do {
    int remorque_id;
    // for (unsigned j = 0; j < stations->size(); j++) {
    //     Station* station = stations->at(j);
    //     remorque_id = rand() % inst->remorques.size();
    //     Circuit* circuit = sol->circuits->at(remorque_id);
    //     circuit->insert_from_option(station);
    // }
    for (const auto &station : stations) {
      remorque_id = rand() % inst->remorques.size();
      Circuit *circuit = this->circuits[remorque_id];
      circuit->append(station);
    }
    this->update();
  } while (!this->is_valid());
};

void Solution::copy(const Solution *other) {
  this->inst = other->inst;
  this->length = other->length;
  this->desequilibre = other->desequilibre;
  // ASSERT this->circuits.size() == other->circuits.size()
  for (unsigned i = 0; i < this->circuits.size(); i++) {
    this->circuits[i]->copy(other->circuits[i]);
  }
}
void Solution::clear() {
  length = 999999999;
  this->desequilibre = 0;
  // Boucle version itérateur
  // for (unsigned i = 0; i < circuits.size(); i++) {
  //     circuits[i]->clear();
  // }
  // Boucle version C++11
  for (auto &&circ : this->circuits) {
    circ->clear();
  }
}
// Mise à jour de la solution complète en fonction des circuits modifiés
// - réinitialise ls attribut globaux de la sol (length, ...)
// - recalcule l'équilibrage optimal des seuls circuits en paramètres
//   (supposés être des circuits de cette solution)
// - met à jour les attributs globaux en fonction de ceux de **tous** les
//   circuits de la solution
void Solution::update(const set<Circuit *> &durty_circuits) {
  logn4("Solution::update BEGIN");

  if (log5() && (durty_circuits.size() != this->circuits.size())) {
    cout << "Solution::update durty_circuits : ";
    for (const auto &circ : durty_circuits) {
      cout << circ->remorque->name << " ";
    }
    cout << endl;
  }

  // for (auto it = durty_circuits.begin();
  //           it != durty_circuits.end(); it++) {
  //     Circuit* circuit = *it;
  //     xxxx;
  // }
  for (const auto &circ : durty_circuits) {
    if (log6()) {
      cout << "Solution::update: remorque=" << circ->remorque->name
           << "  cost=" << U::to_s(circ->desequilibre) << "--"
           << U::to_s(circ->length);
    }
    circ->update();
  }

  // Reset des attributs dérivés globaux de la solution
  length = 0;
  desequilibre = 0;

  // for (auto it = this->circuits.begin();
  //           it != this->circuits.end(); it++) {
  //     Circuit* circuit = *it;
  //     this->length += circuit->length;
  //     this->desequilibre += circuit->desequilibre;
  // }
  for (const auto &circ : this->circuits) {
    this->length += circ->length;
    this->desequilibre += circ->desequilibre;
  }
  // cout << "xxxx UPDATE " << this->desequilibre << "-" << this->length <<
  // endl;
  logn4("Solution::update: cost=" + U::to_s(this->desequilibre) + "--" +
        U::to_s(this->length));
  logn4("Solution::update END");
}

// true ssi la solution est valide
// Une solution est non valide si une des remorques ne dessert aucune station
// PRECONDITION : la solution a été mise à jour par update()
bool Solution::is_valid() const {
  bool is_valid = true;
  //
  // AU BOULOT !!!
  // ...
  return is_valid;
}

// Construction d'une chaine correspondant au format standard de la solution
//
// Exemple de format de sortie
//
//     # Fichier velib_0_0_279.sol
//     # generee le ... par ...
//
//     nom velib_0
//     desequilibre 0
//     distance 270
//
//     # id, charge_init, desequ, longueur
//     circuit r1 0 0 58
//     s1 0
//     end
//
//     # id, charge_init, desequ, longueur
//     circuit r2 6 0 212
//     s4 2
//     s2 4
//     s3 -3
//     s5 0
//     s6 -1
//     s7 0
//     s8 -1
//     end
//
string Solution::to_s() const {
  ostringstream buf;
  buf << "# générée le : TODO " << endl;
  buf << endl;
  buf << "nom " << inst->name << endl;
  buf << "desequilibre " << this->desequilibre << endl;
  buf << "distance " << this->length << endl;
  buf << endl;
  for (const auto &circ : this->circuits) {
    buf << circ->to_s() << endl;
  }
  return buf.str();
}

string Solution::to_s_lite() const {
  ostringstream buf;
  for (const auto &circ : this->circuits) {
    buf << "  " << circ->remorque->name << ": " << inst->to_s(circ->stations)
        << endl;
  }
  buf << "  => coût : " << this->get_cost_string() << endl;
  return buf.str();
}

// Fornat libre et verbeux
string Solution::to_s_long() const {
  // U::die("Solution::to_s_long : non implantée");
  ostringstream buf;
  buf << "# générée le : TODO " << endl;
  buf << endl;
  buf << "nom " << this->inst->name << endl;
  buf << "desequilibre " << this->desequilibre << endl;
  buf << "distance " << this->length << endl;
  buf << endl;
  for (const auto &circ : this->circuits) {
    buf << circ->to_s_long() << endl;
  }
  return buf.str();
}

// to_s_abstract : convertit la solution en une chaine unique et
// minimaliste de façon à ce que deux solutions identiques **noramalisées**
// donnent le même résumé.
// Cette méthode est destinée à servir de base à un hachage.
//
// Exemple : "r1:s1,s2,s3-r2:s4,s5,s6"
//
// ATTENTION : cette méthode normalise seulement l'abstract de la solution
// e.g  "r1:s3,s2,s1" n'est pas possible.
//
string Solution::to_s_abstract() const {
  ostringstream buf;
  string circuit_sep = ""; // passera à "-" à partir du second circuit
  for (const auto &circ : this->circuits) {
    string station_sep = ""; // "," à partir de la seconde station
    buf << circuit_sep << circ->remorque->name << ":";
    circuit_sep = "-";

    if (circ->stations.front()->id < circ->stations.back()->id) {
      for (const auto &st : circ->stations) {
        buf << station_sep << st->name;
        station_sep = ",";
      }
    } else {
      for (int i = 0; i < circ->stations.size(); i++) {
        auto st = circ->stations[i];
        buf << station_sep << st->name;
        station_sep = ",";
      }
    }

    // Ceci fonctionnait (mais pas de normalisation du circuit)
    // for (const auto &st : circ->stations) {
    //   buf << station_sep << st->name;
    //   station_sep = ",";
    // }
  }
  return buf.str();
}

// Calcule le hachage de la solution (grand entier unique pour
// une solution donnée)
size_t Solution::to_hash() const {
  // - std::hash<std::string> est un type,
  // - {} crée une instance de ce type,
  // - Cette instance est un foncteur (qui contient un operator()),
  // - c'est cette operateur qui calcule le hash sur la
  //   représentation chaine de notre solution !
  // (c'est bo le c++, hein : après on s'étonne que les gamins ne
  // jurent que par le piton ;-)
  size_t hash = std::hash<string>{}(this->to_s_abstract());
  return hash;
}

// Normalise la solution de façon à ce que la première station
// visitée par chaque remorque soit de numéro inférieur à la
// dernière station visitée.
// Au besoin la tournée de certaines remorques est inversée
// (ce qui ne modifie pas son coût !)
//
// Si do_update est à vrai (valeur pas défaut) la solution
// est mise à jour (équilibrage, ...)
// Le coût et la distance étant inchangés, cet update n'est utile pour pour
// mettre à jour les attributs unordered_map charges[] de depots[].
void Solution::normalize(bool do_update) {
  for (auto &circuit : this->circuits) {
    int size = circuit->stations.size();
    if (circuit->stations[0]->id > circuit->stations[size - 1]->id) {
      circuit->reverse_branch(0, size);
      if (do_update) {
        circuit->update();
      }
    }
  }
}

// Construit un nom de fichier de la forme
//    velib_nn_mmmm.sol
// avec
//    nn : valeur du déséquilibre
//    mmmm : longueur de la solution
// @params: dir : nom du répertoire de sauvegarde de la solution
//
// - Si dir est une chaine vide, on utilise le répertoire passé en option
// - Pour imposer le répertoire courant malgré une option différente passée en
// option
//   de la'pllication, il faut passer la chaine "./"
string Solution::get_tmp_filename(string soldir) const {
  if (soldir == "") {
    soldir =
        Options::args->soldir + "/"; // répertoire reçu en option de l'applis
  }
  ostringstream buf;
  buf << soldir << "/";
  buf << inst->get_shortname() << "-" << this->desequilibre << "-"
      << this->length << ".sol";
  return buf.str();
}

// Exploite la solution en fonction des options de l'appli (méthode statique !)
void Solution::save(const Solution *sol, Options *args) {
  if (log5()) {
    cout << "Affichage détaillé de la solution\n";
    cout << sol->to_s_long();
  } else if (log1()) {
    cout << "\nSOLUTION TROUVÉE POUR " << sol->inst->get_shortname()
         << " DE COÛT " << sol->get_cost_string() << " (soit "
         << sol->get_cost() << ")" << endl;
  }
  logn3("Enregistement éventuel de la solution\n");
  if (args->outfilename != "") {
    if (args->outfilename == "_AUTO_") {
      // Deux possibilités
      // - ou bien un fichier d'entrée est spécifié et on l'utilise
      // - ou bien l'instance est générée sans être enregistrée
      //   Dans ce cas on basera le fichier de sortie sur le nom de
      //   l'instance avec le nombre de jobs non déservi et la distance
      //   (e.g gotic_9_t20_j100_c03_s40-0-2357.sol)
      //
      if (args->filename != "") {
        // On contruit le nom du fichier de sortie en fonction du nom de
        // l'instance.
        // On supprime le répertoire prefix du fichier d'entrée pour que
        // l'enregistrement se fasse dans le répertoire courant, puis on
        // supprime le suffixe ".dat" pour le remplacer par ".sol".
        ostringstream buf;
        buf << U::file_basename(args->filename, ".dat") << "-"
            << sol->desequilibre << "-" << sol->length << ".sol";
        // ATTENTION, ICI ON MODIFIE UN CHAMPS DE L'OBJET args !
        args->outfilename = buf.str();
      } else {
        // args->outfilename = sol->inst->name + ".sol"; // pour fainéants !
        args->outfilename = sol->get_tmp_filename();
      }
    }
    U::write_file(args->outfilename, sol->to_s());
  }
}

  //./
