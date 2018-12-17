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
  greedy_solver->solve(true); // the argument true enables randomization
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

  logn1("TODO : use argument instead of hard-coded number of descents (5)");

  Solution* descent_sol = new Solution(inst);

  // we operate several descents
  for(int i = 0; i < 5; i++) {
    cout  << endl << i << "th iteration of the descent solver" << endl;
    // we copy the best solution and run the descent solver
    descent_sol->copy(tmp_sol);
    descent_solver->solve(descent_sol, Options::args->itermax);

    logn1("\nDescent solver completed\n");
    cout << "Cost : " << descent_sol->get_cost() << endl;
    logn1("TODO: investigate in case the descent returns false");
    /*
    if(!descent_solver->found) {
      found = false;
      return false;
    }
    */

    // we update the solution if this one is better
    if(descent_sol->get_cost() <= tmp_sol->get_cost())
      tmp_sol->copy(descent_sol);
  }
  found = true;

  delete descent_sol;
  delete descent_solver;

  this->solution->copy(tmp_sol);
  delete tmp_sol;

  return found;
}

//./
