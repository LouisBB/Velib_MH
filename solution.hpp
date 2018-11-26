#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "circuit.hpp"
#include "common.hpp"
#include "options.hpp"

// CONVENTION : les objets de cette classe seront généralement appelés
// sol1, sol2, pour ne pas confondre avec solver (de classe Solver !) ou
// s1 et s2 de classe Station.
//
class Solution {
 public:
  const Instance* inst;
  vector<Circuit*> circuits;

  // longueur total de la solution
  int length;

  // désequilibre total de la solution
  int desequilibre;

  Solution(const Instance* inst);

  // Attention : ceci n'est pas le constructeur par copie car il prend un
  // **pointeur** en parametre
  Solution(const Solution*);

  void copy(const Solution* other);

  virtual ~Solution();

  // vide chaque circuit (dont les listes de stations), et réinitialise
  // les attributs dérivés (deseuilibre, ...)
  void clear();

  // Met à jour les circuits de durty_circuits, et calcule les attributs
  // dérivés de la solution (desequilibres, charge_init, ...)
  //
  void update(const set<Circuit*>& durty_circuits);

  void update() {
    // set<Circuit*> durty_circuits = this->circuits;
    set<Circuit*> durty_circuits(this->circuits.begin(), this->circuits.end());
    // set<Circuit*> durty_circuits;
    // std::copy(this->circuits->begin(), this->circuits->end(),
    // durty_circuits.begin());
    this->update(durty_circuits);
  };

  // Met à jour seulement les attributs propres à la solution.
  // Les circuits étant supposés déjà mis à jour par ailleurs
  void update_lite() {
    set<Circuit*> durty_circuits;  // ensemble vide !
    this->update(durty_circuits);
  }

  // true ssi la solution est valide (voir doc dans le .cpp)
  bool is_valid() const;

  // vrai ssi la solution est acceptable (e.g. les remorques desservent au
  // moins une station, ...; toutes les station sont desservies, ...)
  // bool is_valid();

  // Retourne la chaine de sortie de la solutin au format standard
  string to_s() const;
  string to_s_lite() const;
  string to_s_long() const;

  // to_s_abstract : convertit la solution en une chaine unique et
  // minimaliste de façon à ce que deux solutions identiques donne
  // le même résumé.
  // Cette méthode est destinée à servir de base à un hachage.
  //
  // ATTENTION : cette méthode ne normalise pas la solution
  // e.g
  // "r1:s1-s2-s3" et "r1:s3-s2-s1" sont toutes deux possibles.
  string to_s_abstract() const;

  // Calule le hachage de la solution (grand entier unique pour
  // une solution donnée)
  size_t to_hash() const;

  // Nomalise la solution de façon à ce que la première station
  // visitée par chaque remorque soit de numéro inférieur à la
  // dernière station visitée.
  // Au besoin la tournée de certaines remorques est inversée
  // (ce qui ne modifie pas son coût !)
  void normalize(bool do_update=true);

  // Construit un nom de fichier de la forme
  //    velib_n_dd_mmmm.sol
  // avec
  //    d : numéro d'instance
  //    nn : desequilibre global
  //    mmmm : longueur de la solution
  //
  string get_tmp_filename(string dir = "") const;

  // Exploite la solution en fonction des options de l'appli (classe Args)
  // static void main_print_solution(Solution* solution, Options* args);
  static void save(const Solution* solution, Options* args);

  // Retourne un coût pour povoir comparer deux solutions entre elles
  // C'est une combinaison lexicogarfique des critères desequilibre et lengh
  inline int get_cost() const {
    // return 1000*this->desequilibre + this->length;
    return 1000000 * this->desequilibre + this->length;
  }
  inline string get_cost_string() const {
    return U::to_s(this->desequilibre) + "-" + U::to_s(this->length);
  }
  // Crée un solution initiale stupide mais valide
  void solve_stupid();      // version conforme à StupidSolver
  void solve_stupid_bis();  // version ordonnée différemment
  // Crée un solution initiale aléatoire mais valide
  void solve_random();

  void move_random(int nb_random_moves);
};
#endif
//./
