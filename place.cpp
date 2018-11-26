#include "place.hpp"

string Place::classname() const { return "Place"; }

// les "friend" en premier
ostream& operator<<(ostream& os, const Place& place) {
  return os << place.name;
}

Place::Place(int id, string name, int x, int y) {
  this->id = id;
  this->name = name;
  this->x = x;
  this->y = y;
  // this->sid = Place::last_sid++;
}
Place::~Place() {}

string Place::to_s() const {
  ostringstream buf;
  buf << name << ": (" << x << " " << y << ")";
  return buf.str();
}
string Place::to_s_long() const {
  // pourra être plus verbeux à l'avenir
  ostringstream stm;
  stm << "place " << name << ": (" << x << "," << y << ")";
  return stm.str();
}
string Place::to_s(const vector<Place*>& places) const {
  stringstream buf;
  string sep = "";
  buf << "[";
  for (const auto& place : places) {
    buf << sep << place->to_s();
    sep = ",";
  }
  buf << "]";
  return buf.str();
}

//./
