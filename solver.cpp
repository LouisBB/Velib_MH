#include "solver.hpp"

Solver::Solver(const Instance* inst) {
  this->inst = inst;
  found = false;
}
Solver::~Solver() {
  // todo;
}

// Méthode principale de ce solver, principe :
//
bool Solver::solve() {
  found = false;
  return found;
}
// ./
