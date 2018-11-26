#ifndef SOLVER_H
#define SOLVER_H

#include "common.hpp"
#include "instance.hpp"
#include "solution.hpp"

class Solver {
public:
  // nom court du solver e.g "S_1_glouton"
  string name;

  // pointeur sur l'instance du problème
  const Instance *inst;

  // un boolean parmettent de connaitre l'état de la résolution en fin de calcul
  bool found;

  Solution *solution;

  Solver(const Instance *Instance);
  virtual ~Solver();

  // La principal méthode à redéfinir par les classes filles
  //
  virtual bool solve();

  virtual Solution *get_solution() const { return solution; };
};

////////////////////////////////////////////////////////////////////////
// Déclaration des solveur spécialisés
//
class StupidSolver : public Solver {
public:
  StupidSolver(const Instance *Instance);
  virtual ~StupidSolver();
  virtual bool solve();
  Solution *solution;
  virtual Solution *get_solution() const { return this->solution; };
};

class CarloSolver : public Solver {
public:
  CarloSolver(const Instance *Instance);
  virtual ~CarloSolver();

  virtual bool solve();
  virtual Solution *apply_one_greedy(Solution *currentsol);
  virtual Solution *get_solution() const { return this->solution; };
};

class GreedySolver : public Solver {
public:
  GreedySolver(const Instance *Instance);
  virtual ~GreedySolver();
  virtual bool solve();
  Solution *solution;
  virtual Solution *get_solution() const { return this->solution; };
};

class GraspSolver : public Solver {
public:
  GraspSolver(const Instance *Instance);
  virtual ~GraspSolver();
  virtual bool solve();
  Solution *solution;
  virtual Solution *get_solution() const { return this->solution; };
};

class ExploreSolver : public Solver {
public:
  Solution *cursol;
  Solution *bestsol;

  // ensemble des circuits modifiés qui devront être mis à jour
  set<Circuit *> durty_circuits;

  ExploreSolver(const Instance *Instance);
  virtual ~ExploreSolver();
  virtual bool solve();
  virtual void record_solution(Solution *testsol, long iter);

  virtual Solution *get_solution() const { return this->bestsol; };
};

class DescentSolver : public Solver {
public:
  Solution *cursol;
  Solution *testsol;
  Solution *bestsol;

  // ensemble des circuits modifiés qui devront être mis à jour
  set<Circuit *> durty_circuits;

  long iter;
  long itermax;

  // Cumul du nombre de mouvements testées
  long nb_test;

  // Cumul du nombre de mouvements acceptées
  long nb_move;

  // Nombre de tests consécutifs non améliorants.
  long nb_cons_no_improv;

  // Nombre de tests consécutifs non améliorants.
  long nb_cons_no_improv_max;

  DescentSolver(const Instance *Instance);
  virtual ~DescentSolver();
  virtual bool solve();
  virtual bool solve(Solution *sol, long nbiter=0);

  virtual void mutate(Solution *sol);

  virtual void record_solution(Solution *testsol, long iter);
  virtual void reject_solution(Solution *testsol);

  string get_stats() const;
  bool finished() const;

  virtual Solution *get_solution() const { return this->bestsol; };
};


class AnnealingSolver : public Solver {
public:
  Solution *cursol;
  Solution *testsol;
  Solution *bestsol;

  // ensemble des stations modifiées qui devront être mises à jour
  set<Circuit *> durty_circuits;

  //
  // Attributs d'état du solveur
  //

  // Température courante
  double temp;

  // Cumul du nombre de palliers iso température
  int nb_steps;

  // Numéro de l'itération effectué dans le palier courant
  int iter_in_step;

  // Cumul du nombre de mouvements testées
  int nb_test;

  // Cumul du nombre de mouvements acceptées
  int nb_move;

  // Cumul du nombre de mouvements refusés
  int nb_reject;

  // Nombre de refus consécutif
  int nb_cons_reject;

  // Nombre de tests consécutifs non améliorants.
  int nb_cons_no_improv;

  // On peut éventuellement mémoriser l'évolution taux d'acceptation en
  // fonction du nombre d'itération ou autre, ... vector<double> moving_rates =
  // Array.new(step_size, 1.0)

  //
  // Paramètres de configuration du solveur
  //

  // Largeur du palier à température constante
  int step_size;

  // température initiale (-1 pour automatique)
  double temp_init = -1.0;
  double temp_init_2 = -1.0; // phase 2

  // // température initiale pour la phase 2 (équilibrée)
  // double temp_init_stage2;

  // taux d'acceptation cible pour un calcul auto de temp_init
  double temp_init_rate = 0.8;

  // température minimale (critère d'arêt)
  double temp_mini = 1.0e-6;

  // coeff de décroissance géométrique
  double temp_coef = 0.9;
  double temp_coef_2 = 0.9; // phase 2

  // // coeff de décroissance géométrique pour la phase 2 (équilibrée)
  // double temp_coef_stage2 = 0.9999;

  // nb maxi de refus consécutif de mouvement (améliorant ou pas)
  int nb_cons_reject_max; // infini

  // nb maxi de tests consécutifs non améliorants
  int nb_cons_no_improv_max;

  enum State { NON_EQUILIBRATE_STAGE, IN_EQUILIBRATE_STAGE };
  State state;

  //
  // Constructeurs et méthodes
  //

  AnnealingSolver(const Instance *inst);
  virtual ~AnnealingSolver();
  virtual int eval_cost(Solution *sol);
  virtual bool solve();
  void update_temp();
  double guess_temp_init(const Solution *sol, double taux_cible = 0.8,
                         int nb_degrad_max = 1000);
  void mutate(Solution *sol);
  string get_stats() const;
  bool finished() const;

  void record_solution(Solution *sol);
  void _solution(Solution *sol);

  virtual Solution *get_solution() const { return this->bestsol; };
};
#endif
