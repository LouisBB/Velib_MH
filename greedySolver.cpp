#include "solver.hpp"

GreedySolver::GreedySolver(const Instance* inst) : Solver::Solver(inst) {
  logn1("GreedySolver: résolution par glouton (intelligent) pour " + inst->name);

  cerr << "\nGreedySolver non implanté : AU BOULOT !" << endl;
  exit(1);
  logn1("GreedySolver END.");
}
GreedySolver::~GreedySolver() {
  // TODO
}
// Méthode principale de ce solver, principe :
//
bool GreedySolver::solve() {
  found = false;
  return found;
}

//./
