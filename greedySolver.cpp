#include "solver.hpp"

GreedySolver::GreedySolver(const Instance* inst) : Solver::Solver(inst) {
  logn2("GreedySolver START Résolution par algorithme glouton intelligent.\n"
        " inst: " + inst->name);

  this->solution = new Solution(inst);
  logn2("GreedySolver END construit.");
}
GreedySolver::~GreedySolver() {
  delete this->solution;
}
// Méthode principale de ce solver, principe :
//
bool GreedySolver::solve() {
  found = false;
  return found;
}

//./
