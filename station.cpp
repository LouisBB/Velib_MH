#include "station.hpp"

string Station::classname() const { return "Station"; }

Station::Station(int id, string name, int x, int y, int capa, int ideal,
                 int nbvp)
    : Place::Place(id, name, x, y) {
  this->capa = capa;
  this->ideal = ideal;
  this->nbvp = nbvp;
}
Station::~Station() {}

string Station::to_s() const {
  ostringstream buf;
  buf << "station " << setw(5) << name << setw(5) << x << setw(4) << y
      << setw(8) << capa << setw(6) << ideal << setw(5) << nbvp;
  return buf.str();
}

string Station::to_s_long() const {
  ostringstream buf;
  buf << Place::to_s_long() << " id=" << id << " capa=" << capa
      << " ideal=" << ideal << " nbvp=" << nbvp;
  return buf.str();
}
int Station::margin() const { return this->capa - this->nbvp; }
int Station::deficit() const { return this->ideal - this->nbvp; }

//./
