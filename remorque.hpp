#ifndef REMORQUE_H
class Remorque;
#define REMORQUE_H

#include "common.hpp"
#include "place.hpp"

class Remorque : public Place {
 public:
  int capa;

  Remorque(int id, string name, int x, int y, int capa);
  virtual ~Remorque();
  virtual string classname() const;

  virtual string to_s() const;
  virtual string to_s_long() const;
};
#endif
