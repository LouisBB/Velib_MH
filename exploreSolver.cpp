#include "solver.hpp"

ExploreSolver::ExploreSolver(const Instance* inst) : Solver::Solver(inst) {
  logn1("ExploreSolver : résolution par exploration pour " + inst->name);

  // this->cursol = new Solution(inst);
  // this->bestsol = new Solution(this->cursol);
}
ExploreSolver::~ExploreSolver() {
  // delete cursol;
  // delete bestsol;
}

// Méthode principale de ce solver, principe :
//
bool ExploreSolver::solve() {
  U::die("ExploreSolver::solve: non implantée");

  // ... (au boulot !) ...

  logn2("ExploreSolver::solve: END");
  this->found = true;
  return found;
}

void ExploreSolver::record_solution(Solution* sol, long iter) {
  U::die("ExploreSolver::record_solution: non implantée");
}
//./
