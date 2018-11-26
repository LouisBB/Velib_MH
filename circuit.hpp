#ifndef CIRCUIT_HPP
#define CIRCUIT_HPP

#include "common.hpp"
#include "instance.hpp"

// Un circuit représente le résultat d'une tournée associée à une remorque
// donnée. Les principaux attributs sont :
//   - inst : l'instance propriétaire
// Attributs primaires (indispensables pour construire une solution)
//   - remorque : la remorque correspondant à ce circuit,
//   - charge_init : charge initiale de la remorque
//   - tournees_data : tableau de paires [station, depot]
//     (n'est PAS mémorisé en attribut)
// Attributs dérivés (calculés à partir des attributs primaires)
//   - stations : la liste des stations gérées par cette remorque
//   - depots[Station] : la Hash des dépots des stations visitées
//   - arcs : liste des arcs représentant le chemin (plus pratique à exploiter
//     que la liste des stations visitées)
//
// Pour l'instant, un Circuit n'est pas intelligent : tout est piloté par la
// classe Solution (qui doit donc avoir un accès complet à tous les attributs)
//
// idée : créer une classe Service pour représenter un couple
//  <Station*, int depos>
class Circuit {
 public:
  // L'instance du problème
  const Instance* inst;

  // La remorque associée à ce circuit
  Remorque* remorque;

  // charge initiale de la remorque
  int charge_init;

  // les stations visitées
  vector<Station*> stations;

  // Une map (Hash en Ruby ou Dict en python) contenant le dépos associé
  // à chaque station servie
  // map<Station*,int> depots;
  unordered_map<Station*, int> depots;

  // charge de la remorque **après** passage à une station
  // (peut-être utilisé pour vérifier la remorque ou optimiser une insertion)
  // map<Station*,int> charges;
  unordered_map<Station*, int> charges;

  // Désequilibre total (toujours positif ou nul !)
  int desequilibre;

  // longueur de ce circuit
  int length;

  // Construction d'un circuit sans stations
  //
  Circuit(const Instance* inst, Remorque* remorque);

  // Construction d'un circuit à partir d'une liste de stations
  //
  Circuit(const Instance* inst, Remorque* remorque,
          const vector<Station*>& stations);

  // Attention : ceci n'est pas le constructeur par copie car il prend un
  // **pointeur** en parametre
  Circuit(const Circuit* circ);

  virtual ~Circuit();

  void copy(const Circuit* other);

  // vide la liste des stations
  void clear();

  // Ne vide que les attributs dérivés à partir de la liste des stations
  // desservies (en vue de regénérer des attributs dérivés valides)
  void partial_clear();

  // Calul le dépos optimum en chaque staion, la charge initiale de la remorque
  // et les autres attributs dérivés
  // Pourrait s'appeler equilibate equilibrate, mais update est plus générique
  void update();

  // retourne le coût mélangeant déséquilibre et distance totale (mesure
  // permettant la comparaison simple de deux solutions)
  inline int get_cost() const {
    // return 1000*this->desequilibre + this->length;
    return 1000000 * this->desequilibre + this->length;
  }
  // retourne une chaine de la forme  "1-1234"  séparant la valeur du
  // déséquilibre et la distance totale (pour affichage des messages ou dans le
  // nom des fichiers de solution)
  inline string get_cost_string() const {
    return U::to_s(this->desequilibre) + "-" + U::to_s(this->length);
  }

  // procède à la mise à jour des attributs charge_init, charges et depots
  // de façon à equilibrer au mieux les stations gérées par la remorque de ce
  // circuit.
  void equilibrate_dummy();
  void equilibrate();
  void equilibrate_eleve();

  void insert(Station* s, int pos = 0);
  void insert_best(Station* s);
  void insert_rand(Station* s);
  void append(Station* s);  // idem insert avec pos=-1

  // Insère la station dans le circuit en fonction de l'option
  // Options::args->station_inserter
  void insert_from_option(Station* s);

  // Supprime et retourne le  sid de la station en position pos.
  Station* erase(int pos);

  void move(int pos1, int pos2);
  void move_to(int pos1, Circuit* other, int pos2);

  void move_best(int pos1);
  void move_best_to(int pos1, Circuit* other);

  void reverse_branch(int pos1, int pos2);
  void move_branch_to(int pos1, int pos2, Circuit* other, int pos3,
                      bool reverse = false);
  void move_branch(int pos1, int pos2, int pos3, bool reverse = false);


  // Effectue une mutation de type 2opt aléatoire.
  // Si le boolean do_update est true, alors le circuit est mis à jour
  // (desequilibre, length...)
  //
  void mutate_2opt(bool do_update = true);

  string to_s() const;
  string to_s_lite() const;
  string to_s_long() const;

  void raise(string str = "ERREUR : exception non précisée !");
};
#endif
//./
