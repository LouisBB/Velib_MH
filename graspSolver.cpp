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
  Solution* best_sol = new Solution(inst);

  for(int i = 0; i < 5; i++) {
  // we make it go through the greedy algorithm first
  
  //
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
  Solution* descent_sol = new Solution(inst);

  // we operate the descent
  // we copy the best solution and run the descent solver
  descent_sol->copy(tmp_sol);
  descent_solver->solve(descent_sol, Options::args->itermax);

  if(logn3()) {
    logn1("\nDescent solver completed\n");
      cout << "Cost : " << descent_sol->get_cost() << endl;
  }
  logn1("\nTODO: investigate in case the descent returns false\n");
    /*
    if(!descent_solver->found) {
      found = false;
      return false;
    }
    */

    // we update the solution if this one is better
  if(descent_sol->get_cost() <= best_sol->get_cost())
    best_sol->copy(descent_sol);
  }
  found = true;

  this->solution->copy(best_sol);
  delete tmp_sol;
  return found;
}

//./
