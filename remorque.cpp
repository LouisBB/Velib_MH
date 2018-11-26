#include "remorque.hpp"

string Remorque::classname() const { return "Remorque"; }

// // les "friend" en premier (si nécessaire)
// ostream& operator<<(ostream& os, const Remorque  &remorque) {
//     return os << remorque.name;
// }

Remorque::Remorque(int id, const string name, const int x, const int y,
                   const int capa)
    : Place::Place(id, name, x, y) {
  this->capa = capa;
}
Remorque::~Remorque() {}

string Remorque::to_s() const {
  // TODO : passer par super
  stringstream buf;
  buf << "remorque " << setw(4) << name << setw(5) << x << setw(4) << y
      << setw(5) << capa;
  return buf.str();
}
string Remorque::to_s_long() const {
  stringstream buf;
  buf << Place::to_s_long() << " id=" << id << " capa=" << capa;
  return buf.str();
}

//./
