#include "solver.hpp"
#include "mutator.hpp"

DescentSolver::DescentSolver(const Instance *inst) : Solver::Solver(inst) {
  logn1("DescentSolver : résolution par une descente pour " + inst->name);

  this->cursol = new Solution(inst);

  // Méthode1: Initialisation avec la solution d'un solveur externe
  //   StupidSolver* initsolver = new StupidSolver(inst);
  //   initsolver->solve();
  //   testsol->copy(initsolver->get_solution());

  // Méthode2: Initialisation de la solution par méthode interne à classe
  // Solution
  this->cursol->solve_stupid();
  this->bestsol = new Solution(this->cursol);
  this->testsol = new Solution(this->cursol);

  this->iter = 0;

  this->itermax = Options::args->itermax;
  // Par défaut (-1) on ne fait une infinité d'itérations
  if (this->itermax == -1) this->itermax = U::MY_LONG_MAX;

  this->nb_test = 0;
  this->nb_move = 0;

  // Nombre de tests consécutifs non améliorants.
  this->nb_cons_no_improv = 0;

  // nb maxi de tests consécutifs non améliorants
  this->nb_cons_no_improv_max = 1e9;  // infini;
}
DescentSolver::~DescentSolver() {
  delete testsol;
  delete bestsol;
  delete cursol;
}

// Méthode principale de ce solver, principe :
//
bool DescentSolver::solve() {
  return this->solve(this->cursol, Options::args->itermax);
}
// Méthode principale de ce solver, principe :
//
bool DescentSolver::solve(Solution *sol, long nbiter) {
  if (log2()) {
    cout << "\n---DescentSolver::solve START nbiter=" << nbiter << "---\n";
  }
  cursol->copy(sol);
  if (cursol->get_cost() < bestsol->get_cost()) {
    bestsol->copy(sol);
  }
  if (nbiter == 0) {
    nbiter = Options::args->itermax;
  }

  testsol->copy(cursol);

  this->iter = 0;
  while (!finished()) {
    logn3("solve : iter=" + U::to_s(iter));
    this->iter++;
    this->mutate(testsol);
    this->nb_test++;

    if (testsol->get_cost() < this->bestsol->get_cost()) {
      this->record_solution(testsol, iter);
      cursol->copy(testsol);
      this->nb_move++;
      this->nb_cons_no_improv = 0;
    } else {
      this->reject_solution(testsol);
      this->nb_cons_no_improv++;
    }
    // cout << "nb_cons_no_improv=" << nb_cons_no_improv << endl;
  }
  sol->copy(cursol);
  if (log2()) {
    cout << endl << flush;
  }  // on ne veut pas voir le prefix "L2:"

  logn2("DescentSolver::solve: END");

  return this->found = true;
  // return true;
}

bool DescentSolver::finished() const {
  bool too_many_no_improv =
      (this->nb_cons_no_improv >= this->nb_cons_no_improv_max);
  bool iter_reached = (this->iter >= this->itermax);
  if (iter_reached || too_many_no_improv) {
    if (log2()) {
      cout << "\n(finished=true) STATISTIQUES\n" << this->get_stats();
      cout << endl;
      cout << "=> STOP" << endl;
    }
    return true;
  } else {
    return false;
  }
}

// Effectue une mutation sur la solution en paramètre.
//
void DescentSolver::mutate(Solution *sol) {
  logn4("DescentSolver::mutate BEGIN");
  // U::die("DescentSolver::mutate: MÉTHODE NON IMPLÉMENTÉE");

  // On peut éventuellement appliquer plusieurs types de mutation selon
  // une certaine probabilité, ou encore appliquer plusieurs mouvements
  // consécutifs pour élargir le voisinage

  // Choix d'un numéro de circuit source (i.e. remorque) aléatoire
  // Circuit* c1 = sol->circuits[ rand() % sol->circuits.size() ];

  // Choix d'un numéro de circuit destination (i.e. remorque) aléatoire
  // Circuit* c2 = sol->circuits[ rand() % sol->circuits.size() ];

  // et on en fait quoi de c1 et c2 ?
  // ...


  Mutator::mutate_intra_circuit_move_station(sol);

  sol->update();
  logn4("DescentSolver::mutate END");
};

string DescentSolver::get_stats() const {
  ostringstream stm;
  stm << ""
         "Paramètres de l'objet DescentSolver :\n"
         "  iter=                  "
      << this->iter << endl
      << "  itermax=               " << this->itermax << endl
      << "  nb_cons_no_improv=     " << this->nb_cons_no_improv << endl
      << "  nb_cons_no_improv_max= " << this->nb_cons_no_improv_max << endl
      << "  nb_test=               " << this->nb_test << endl
      << "  nb_move=               " << this->nb_move << endl
      <<
      // "  nb_reject=             " << this->nb_reject << endl <<
      "  testsol->get_cost()=   " << this->testsol->get_cost() << endl
      << "  cursol->get_cost()=    " << this->cursol->get_cost() << endl
      << "  bestsol->get_cost()=   " << this->bestsol->get_cost() << endl
      << "";
  return stm.str();
}

void DescentSolver::record_solution(Solution *sol, long iter) {
  if (sol->is_valid()) {
    // RECORD BATTU
    this->bestsol->copy(sol);
    if (log1()) {
      // ATTENTION FAQ : bufférisation et cout.flush() ou << flush
      // L'utilisation de endl au lieu de "\n" est inutile ici car
      // on veut afficher la chaine qui suite ce endl. Il faut donc
      // ajouter faire un flush du flux pour garantir l'affichage
      // immédiat.
      cout << "\n" << iter << ": " << bestsol->get_cost() << flush;
    }
    // On enregistre cette solution dans un fichier temporaire
    string tmpname = this->bestsol->get_tmp_filename();
    // U::write_file(tmpname, this->bestsol->to_s());
  } else {
    // Solution invalide
    // log2("x"); // afficherait "L2x" or on veut seulement "x"
    if (log2()) {
      cout << "x" << flush;
    }
  }
}
// solution rejetée. Deux cas possibles :
// 1. si mode "descente pur" alors on repart de la meilleure solution
// 2. si mode "exploration" alors on accepte systématiquement
//    (hot pour simuler une température infinie)
void DescentSolver::reject_solution(Solution *sol) {
  if (!Options::args->hot) {
    sol->copy(this->bestsol);  // on restaure bestsol dans sol
  } else {
    // rien à faire
  }
  // log2("."); // afficherait "L2." or on veut seulement un "."
  if (log2()) {
    cout << "." << flush;
  }
}
//./
