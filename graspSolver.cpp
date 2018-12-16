#include "solver.hpp"

GraspSolver::GraspSolver(const Instance *inst) : Solver::Solver(inst) {
  logn1("GraspSolver: résolution par GRASP pour " + inst->name);

  this->solution = new Solution(inst);

  logn1("GraspSolver END.");
}
GraspSolver::~GraspSolver() {
  // TODO
}
// Méthode principale de ce solver, principe :
//
bool GraspSolver::solve() {
  found = false;
  logn1("\n---Grasp solver strating ---\n");
  Solution* tmp_sol = new Solution(inst);

  // we make it go through the greedy algorithm first
  GreedySolver* greedy_solver = new GreedySolver(inst);
  greedy_solver->solve();
  if(!greedy_solver->found) {
    logn1("\n   greedy solver didn't find a solution \n");
    delete greedy_solver;
    delete tmp_sol;

    found = false;
    return found;
  }

  tmp_sol->copy(greedy_solver->get_solution());
  delete greedy_solver;

  // now we have an updated solution, let's have it go through the descent Solver
  DescentSolver* descent_solver = new DescentSolver(inst);

  logn1("TODO :? use argument instead of hard-coded number of iterations (100)");
  cout << endl << Options::args->itermax << endl;
  descent_solver->solve(tmp_sol, Options::args->itermax);

  logn1("Descent solver completed\n");

  if(!descent_solver->found) {
    found = false;
    return false;
  }

  found = true;

  delete descent_solver;

  this->solution->copy(tmp_sol);
  delete tmp_sol;

  return found;
}

//./
