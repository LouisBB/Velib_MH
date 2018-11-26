#ifndef STATION_H
#define STATION_H

#include "common.hpp"
#include "place.hpp"

class Station : public Place {
 public:
  int capa;   // capacité maxi de la station
  int ideal;  // nombre idéla de vélo dans la station
  int nbvp;   // nombre de véo présent

  Station(int id, string name, int x, int y, int capa, int ideal, int nbvp);
  virtual ~Station();
  virtual string classname() const;

  virtual int margin() const;
  virtual int deficit() const;

  virtual string to_s() const;
  virtual string to_s_long() const;
};
#endif
