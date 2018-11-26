#ifndef INSTANCE_H
#define INSTANCE_H

#include "arc.hpp"
#include "common.hpp"
#include "options.hpp"
#include "place.hpp"
#include "remorque.hpp"
#include "station.hpp"
// #include "solver.hpp"

// 17/02/2016 ATTENTION : IMPOSSIBLE DE CREER PLUSIEURS INSTANCE
// En effet, pour l'instant, les id des remorques ou des stations sont créés
// automatiquement via les variables statiques Station::last_id et
// Remorque::last_id.
// Or ces id doivent commencer à 0 car il sont exploités pour les calculs
// d'indices de tableaux pour les arcs.
// TODO : il faudra donc que la classe Instance soit devienne des id de
// Station ou Remorque qu'elle crée.

// Classe responsable de la manipulation des l'instance (dont lecture du
// fichier d'instance, ...)
//
// Exemple de fichier à reconnaitre :
//
//      velib velib_0 2 8
//      version 1.0
//
//      #remorque   id  x   y   K
//      remorque    r1  25  25  6
//      remorque    r2  75  75  6
//
//      #station  id     x   y  capa  ideal   nbvp
//      station   s1    10  50    11      8      8
//      station   s2    20  90     9      7      3
//      station   s3    25  60    10      7     10
//      station   s4    45  80     6      4      2
//      station   s5    40  60     7      5      5
//      station   s6    60  40     6      5      6
//      station   s7    80  50     7      5      5
//      station   s8    95  85     7      6      7
//
//
class Instance {
 public:
  // nom de l'instance (e.g. velib_1_s010_r2_k5..6_v11)
  string name;

  vector<Station*> stations;
  vector<Remorque*> remorques;
  vector<Place*> places;
  // int       nb_places;

  // le vecteur des arcs valides
  vector<Arc*> arcs;

  // arcs_grid est un vector représentant une matrice dont les coordonnées sont
  // les id (interne) des places et dont la valeur est l'arc entre ces deux
  // places Sera utilisé pour accéder à un arc correspondant à partir de deux
  // places connus vector<Arc> arcs_grid;
  vector<Arc*> arcs_grid;

  // Matrice des distances précalculées.
  // Redondant avec l'attribut dist de la classe arcs_grid car chaque arc
  // possède un attribut dist. Mais la notion d'arc n'est probablement pas
  // utile pour le projet velib. L'attribut dists_grid évitera la création des
  // arcs rien que pour récupérer les distances.
  vector<int> dists_grid;

  // Construteur par défaut
  Instance();
  // Constructeur acceptant un nom de fichier
  Instance(string);

  // Le destructeur
  virtual ~Instance();

  string filename;

  // Nombre totale de places (stations + remorques)
  int get_nb_places() const {
    // return Place::nb_places(); // FAUX SI PLUSIEURS INSTANCES SIMULTANEES !
    return this->stations.size() + this->remorques.size();
  }
  // retourne le meme pointeur de Edge pour i1 < i2 ou i1 > i2
  Arc* get_arc(int i1, int i2) const;
  Arc* get_arc(const Place* p1, const Place* p2) const;

  // Quelques méthodes de recherche d'objet à partir de son nom.
  // Utilisées seulement pour faciliter les tests.
  Arc* find_arc(const string& s_name, const string& d_name) const;
  Station* find_station(const string& name) const;

  int get_dist(const Place* s1, const Place* s2) const;

  // construit les arcs associés aux stations et à la remorque en paramètre,
  // et remplir le vecteur d'arcs fourni.
  void fill_arcs(vector<Arc*>& arcs, const vector<Station*>& stations,
                 const Remorque* remorque);

  // // int get_dist(const vector<Station>& stations,
  // //              const Remorque& remorque);

  // int get_dist(const list<int>& station_ids,
  //              const Remorque& remorque);

  int get_dist(const vector<Station*>& stations,
               const Remorque* remorque = nullptr) const;

  // nombre de vol de velib (peut-être négatif s'il y a trop de vélos)
  int get_nb_vols() const;

  // Capacité de la plus grosse remorque
  int get_max_remorque_capa() const;

  // Total des capacité des remorques
  int get_total_remorque_capa() const;

  // Valeur maxi du déséquilibre sur l'ensemble des stations
  int get_max_station_desequilibre() const;

  // Cumul des excès des déséquilibre des stations
  // L'excès d'un déséquilibre est la valeur qui ne pourra jamais être
  // réséquilibrée compte tenu de la capacité de la plus grossse remorque.
  int get_total_excess_station_desequilibre() const;

  // Cumul des déséquilibres dus au vols.
  // Dépend de la capacité totale des remorques diponibles
  int get_total_vols_desequilibre() const;

  // Borne inférieurs de la distance totale
  // (version triviale, sinon nécessiterait uns classe solveur spécifique)
  int get_lowerbound_dist() const;

  // // Fonction de tri... TODO passer de Station à Place par template
  // // tri les remorque en fonction de leur proximité à une station donnée
  // void sort_stations_by_nearest_from(vector<Remorque*>* remorques, Place*
  // ref); void sort_stations_by_nearest_from(list<Remorque*>* remorques, Place*
  // ref);

  string get_shortname() const;

  // Retourne le meilleur cout connu de l'instance
  int best_known_cost() const;

  // Convertit l'instance au format standard velib
  string to_s() const;

  // Convertit l'instance au format détaillé (avec affichage des id)
  string to_s_long() const;

  // Convertit un vecteur de Place (donc de stations ou de remorques) en une
  // chaine lisible sous la forme "[s1, s7, s2, r1, s4]"
  //
  template <typename PLACE>
  string to_s(const vector<PLACE*>& places) const {
    string sep = ", ";
    string the_sep = "";
    ostringstream buf;
    buf << "[";
    for (const auto& place : places) {
      buf << the_sep << place->name;
      the_sep = sep;
    }
    buf << "]";
    return buf.str();
  }
  // Convertit une **liste** de places (donc de stations ou de remorques) en une
  // chaine lisible sous la forme "[s1, s7, s2, r1, s4]"
  //
  // TODO:
  //   - Fusionner ces deux méthodes en une seule en paramétrant le type
  //     du conteneur
  template <typename PLACE>
  string to_s(const list<PLACE*>& places) const {
    string sep = ", ";
    string the_sep = "";
    ostringstream buf;
    buf << "[";
    for (const auto& place : places) {
      buf << the_sep << place->name;
      the_sep = sep;
    }
    buf << "]";
    return buf.str();
  }

  // enregistre l'instance dans un fichier (pour le gérérateur d'instance)
  void print_instance(const string& filename) const;

  // affiche l'instance sur un flux ("cout"par défaut)
  void print_instance(ostream& = cout) const;

  // génère l'instance mini donnée en exemple dans le sujet (pour tests)
  static Instance* new_velib_mini();
  static Instance* new_velib_mini_mono_remorque();
  static Instance* new_velib_r1_s050();
  static Instance* new_velib_r1_s100();
  static Instance* new_velib_r1_s200();

  void add_remorque(const string& line);
  void add_station(const string& line);

  static void test_get_arc(const Instance* inst) {
    cout << "Test acces arc (1,2) : to_s, src, dst et dist\n";
    // Edge* edge = b->get_edge(1,2);
    cout << "pp\n";
    cout << "to_s()=     " << inst->get_arc(1, 2)->to_s() << endl;
    cout << "src->to_s()=" << inst->get_arc(1, 2)->src->to_s() << endl;
    cout << "src->to_s()=" << inst->get_arc(1, 2)->dst->to_s() << endl;
    cout << "dist=       " << inst->get_arc(1, 2)->dist << endl;
  };

 private:
  void init();

  void read_velib_instance();

  // Création des matrices d'arcs et des distances
  void build_arcs();
};

#endif
//./
