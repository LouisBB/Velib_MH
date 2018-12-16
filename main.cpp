#include "common.hpp"

#include "instance.hpp"
#include "solver.hpp"

#include "options.hpp"

#include "test.hpp"

// je dois définir cette variable statique en dehors de la classe Options
Options *Options::args = nullptr;

void test_action(Options *args) {
  // logn1("s=0 => Test construction d'instance new_velib_mini\n");
  // Instance* inst = Instance::new_velib_mini();
  // cout << inst->to_s();
  // return 0;

  test_circuit_reverse_branch();
  test_util_rand();
  test_circuit_move();
  test_circuit_move_to();

  test_proto();

}

void instance_action(Options *args) {
  logn1("instance_action BEGIN : lecture, affichage d'une instance");
  Instance *inst = new Instance(args->filename);
  inst->print_instance();
  inst->print_instance("mini_rand.dat");

  cout << "Affichage détaillé de l'instance (avec les id)" << endl;
  cout << inst->to_s_long();
  delete inst;
  logn1("instance_action END");
}

void stupid_solver_action(Options *args) {
  logn1("stupid_solver_action BEGIN");
  Instance *inst = new Instance(args->filename);
  StupidSolver *solver = new StupidSolver(inst);
  solver->solve();
  if (solver->found) {
    Solution::save(solver->get_solution(), args);
  } else {
    cout << "StupidSolver : pas de solution" << endl;
  }
  delete solver;
  delete inst;
  logn1("stupid_solver_action END");
}

void carlo_solver_action(Options *args) {
  logn1("carlo_solver_action BEGIN");
  Instance *inst = new Instance(args->filename);
  CarloSolver *solver = new CarloSolver(inst);
  solver->solve();
  if (solver->found) {
    Solution::save(solver->get_solution(), args);
  } else {
    cout << "Solver : pas de solution " << endl;
  }
  delete solver;
  delete inst;
  logn1("carlo_solver_action END");
}

void explore_solver_action(Options *args) {
  logn1("explore_solver_action BEGIN");
  Instance *inst = new Instance(args->filename);
  ExploreSolver *solver = new ExploreSolver(inst);
  solver->solve();
  if (solver->found) {
    Solution::save(solver->get_solution(), args);
  } else {
    cout << "Solver : pas de solution " << endl;
  }
  delete solver;
  delete inst;
  logn1("explore_solver_action END");
}

void greedy_solver_action(Options *args) {
  logn1("greedy_solver_action BEGIN");
  Instance *inst = new Instance(args->filename);
  GreedySolver *solver = new GreedySolver(inst);
  solver->solve();
  if (solver->found) {
    Solution::save(solver->get_solution(), args);
  } else {
    cout << "Solver : pas de solution " << endl;
  }
  delete solver;
  delete inst;
  logn1("greedy_solver_action END");
}

void grasp_solver_action(Options *args) {
  logn1("grasp_solver_action BEGIN");
  Instance *inst = new Instance(args->filename);
  GraspSolver *solver = new GraspSolver(inst);
  solver->solve();
  if (solver->found) {
    Solution::save(solver->get_solution(), args);
  } else {
    cout << "Solver : pas de solution " << endl;
  }
  delete solver;
  delete inst;
  logn1("gasp_solver_action END");
}

void descent_solver_action(Options *args) {
  logn1("descent_solver_action BEGIN");
  Instance *inst = new Instance(args->filename);
  DescentSolver *solver = new DescentSolver(inst);
  // if (Options::args->alt) {
  //     solver->solve_diam();
  // } else {
  //     solver->solve();
  // }
  solver->solve();
  if (solver->found) {
    Solution::save(solver->get_solution(), args);
  } else {
    cout << "Solver : pas de solution " << endl;
  }
  delete solver;
  delete inst;
  logn1("descent_solver_action END");
}

void annealing_solver_action(Options *args) {
  logn1("annealing_solver_action BEGIN");
  Instance *inst = new Instance(args->filename);
  AnnealingSolver *solver = new AnnealingSolver(inst);

  // solver->step_size = 10; // ORI
  solver->step_size = inst->stations.size() * inst->remorques.size();
  // solver->temp_init = -1;//  -1 pour auto
  solver->temp_init = 1 * 1000 * 1000;  // phase initiale ORI -1 pour auto

  solver->temp_init_rate = 0.8;
  // solver->temp_mini = 1.0e-10;

  solver->temp_coef = 0.99;
  solver->nb_cons_reject_max = 1e9;
  solver->nb_cons_no_improv_max = 10000;

  cout << "solver->temp_init=" << solver->temp_init << endl;
  solver->solve();
  if (solver->found) {
    Solution::save(solver->get_solution(), args);
  } else {
    cout << "Solver : pas de solution " << endl;
  }
  delete solver;
  delete inst;
  logn1("annealing_solver_action END");
}

int main(int argc, char *argv[]) {
  // Analyse et extraction des arguments de la ligne de commande
  // La variable statique Options::args sera disponible dans toutes les classes
  Options::args = new Options(argc, argv);
  Options *args = Options::args;  // une variable locale

  // Exemple d'affichage de quelques options
  cout << "args->action=" << args->action << endl;
  cout << "args->seed=" << args->seed << endl;
  cout << "args->filename=" << args->filename << endl;
  cout << "args->outfilename=" << args->outfilename << endl;
  cout << "==== U::MY_INT_MAX=" << U::MY_INT_MAX << endl;
  cout << "==== U::MY_LONG_MAX=" << U::MY_LONG_MAX << endl;
  // exit(0);

  // Démarrage d'un chronomètre
  clock_t time_start = clock();

  // Dans la suite, le comportement de la résolution dépend du choix du
  // solveur passé en paramètre.
  if (args->action == "test") {
    ;
    test_action(args);
  } else if (args->action == "instance") {
    instance_action(args);
  } else if (args->action == "stupid") {
    stupid_solver_action(args);
  } else if (args->action == "carlo") {
    carlo_solver_action(args);
  } else if (args->action == "greedy") {
    greedy_solver_action(args);
  } else if (args->action == "grasp") {
    grasp_solver_action(args);
  } else if (args->action == "explore") {
    explore_solver_action(args);
  } else if (args->action == "descent") {
    descent_solver_action(args);
  } else if (args->action == "annealing") {
    annealing_solver_action(args);
  } else {
    cerr << "Action non reconnue : " << args->action << "\n";
    exit(1);
  }

  // Arrêt et exploitation du chronomètre
  clock_t time_stop = clock();
  float diff_sec = ((float)time_stop - (float)time_start) / CLOCKS_PER_SEC;
  if (log1()) {
    printf("Temps de calcul :\t %.2fs\n", diff_sec);
  }

  delete args;
  return 0;
}
//./
