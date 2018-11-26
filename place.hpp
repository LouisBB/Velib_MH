#ifndef PLACE_H
#define PLACE_H

#include "common.hpp"

class Place {
  // les "friend" en premier
  friend ostream& operator<<(ostream& os, const Place& place);

 public:
  string name;
  int x;
  int y;
  int id;

  // Place();
  Place(int id, const string name, const int x, const int y);
  virtual ~Place();

  virtual string to_s() const;
  virtual string to_s_long() const;

  virtual string classname() const;
  virtual string to_s(const vector<Place*>& places) const;

  static bool compareByName(const Place* p1, const Place* p2) {
    return p1->name < p2->name;
  }
};

#endif
