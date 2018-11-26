#include "solver.hpp"

GraspSolver::GraspSolver(const Instance *inst) : Solver::Solver(inst) {
  logn1("GraspSolver: résolution par glouton (intelligent) pour " + inst->name);

  cerr << "\nGraspSolver non implanté : AU BOULOT !" << endl;
  exit(1);
  logn1("GraspSolver END.");
}
GraspSolver::~GraspSolver() {
  // TODO
}
// Méthode principale de ce solver, principe :
//
bool GraspSolver::solve() {
  found = false;
  return found;
}

//./
