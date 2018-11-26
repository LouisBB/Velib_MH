#ifndef ARC_H
#define ARC_H

#include "common.hpp"
#include "place.hpp"

// La classe Arc représente un couple **ORIENTÉ** de place.
// Les boucles sont autorisées.

class Arc {
  // les "friend" en premier
  friend ostream& operator<<(ostream& os, const Arc& arc);

 public:
  std::string name;
  const Place* src;
  const Place* dst;

  int dist;

  Arc();
  Arc(const Place* src, const Place* dst);
  virtual ~Arc();

  // Retourne true ssi le passage d'un place à un autre est possible
  // compte tenu : (A COMPLETER)
  // Pour l'instant pas de filtrage : on autorise tous les arcs
  // TODO: interdire les arcs suivants :
  //    - d'une place vers la même Place
  //    - d'une remorque vers une autre remorque
  bool is_feasible() const { return true; };

  // vérifie si place est une des extrémités de l'arete
  bool is_adj(const Place* place) const;

  // Retourne l'extrémité de l'arete opposée à place
  const Place* get_opposite(const Place* place) const;

  std::string to_s() const;
  std::string to_s_long() const;
};
#endif
