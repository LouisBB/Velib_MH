#include "arc.hpp"
#include "instance.hpp"

// les "friend" en premier
ostream& operator<<(ostream& os, const Arc& arc) { return os << arc.name; }

Arc::Arc(const Place* src, const Place* dst) : src(src), dst(dst) {
  // this->src = src;
  // this->dst = dst;
  this->name = "[" + src->name + "->" + dst->name + "]";

  double x1 = src->x;
  double y1 = src->y;
  double x2 = dst->x;
  double y2 = dst->y;
  this->dist = (int)round(sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
}

// Destructeur
Arc::~Arc() {}

bool Arc::is_adj(const Place* place) const {
  return (this->src->id == place->id || this->dst->id == place->id);
}
const Place* Arc::get_opposite(const Place* place) const {
  if (this->src->id == place->id) {
    return this->dst;
  } else if (this->dst->id == place->id) {
    return this->src;
  } else {
    // Il faudra lever une vraie exception. Pour l'instant on hurle !!
    cerr << "ERREUR : get_opposite  de " << this->to_s() << " pour "
         << place->to_s() << " : \nplace non adjacent !!\n";
    return place;  // histoire de retourner quelque chose !!
  };
}

// Affichage plus détaillé que le simple name
string Arc::to_s() const { return name; }
// Affichage plus détaillé que le simple name
string Arc::to_s_long() const {
  ostringstream stm;
  stm << name << ": "
      << " dist=" << dist;
  if (!this->is_feasible()) {
    stm << " ARC INFAISABLE !";
  }
  return stm.str();
}
//./
