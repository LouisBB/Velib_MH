#include "mutator.hpp"
#include "solver.hpp"

AnnealingSolver::AnnealingSolver(const Instance* inst) : Solver::Solver(inst) {
  logn1("AnnealingSolver: résolution par recuit simulé pour " + inst->name);
  cursol = nullptr;
  testsol = nullptr;
  bestsol = nullptr;
  // ...
}
AnnealingSolver::~AnnealingSolver() {
  delete cursol;
  delete testsol;
  delete bestsol;
}
bool AnnealingSolver::finished() const {
  return false;
}
string AnnealingSolver::get_stats() const {
  // return "AnnealingSolver::get_stats() NON IMPLEMENTEE (voir DescentSolver) !";
  return "";
}

// Calcule d'une température initiale de manière à avoir un
// taux d'acceptation TAUX en démarrage
//
// arguments :
//   - taux_cible : pourcentage représentant le taux d'acceptation cible(e.g.
//   0.8)
//   - nb_degrad_max : nbre de degradation à accepter pour le calcul de la
//   moyenne
//
// Principe :
//   AU BOULOT !
//
// ATTENTION :
//  Cette fonction n'est **pas** une méthode de AnnealingSolver.
//  Elle a juste besoin d'une solution et du type de mouvement à effectuer.
//  Ici, on suppose que le seul mouvement possible est swap!(sol::Solution)
//  Mais il faudra pouvoir paramétrer cette méthode pour des voisinages
//  différents.
//
double AnnealingSolver::guess_temp_init(const Solution* sol, double taux_cible,
                                        int nb_degrad_max) {

  double t_init = 0.0; // savant calcul !
  return t_init;
}
// Cette méthode est appelé en fin de palier à température constante.
// Elle est principalement utilisée pour mettre à jour la température,
// mais aussi pour modifier le comportement du recuit en fonction de l'état du
// solver (e.g passage de l'état équilibré ou non, ...)
//
void AnnealingSolver::update_temp() {
  if (log2()) {
    cout << "update_temp :"
         << " old temp=" << this->temp << " old temp_cof=" << this->temp_coef
         << endl;
  }
  this->temp = this->temp * this->temp_coef;
  if (log2()) {
    cout << " =>  new temp=" << this->temp << endl;
  }
}
void AnnealingSolver::mutate(Solution* sol) {
  logn4("AnnealingSolver::mutate BEGIN");
  U::die("AnnealingSolver::mutate: MÉTHODE NON IMPLÉMENTÉE");
  logn4("AnnealingSolver::mutate END");
}

void AnnealingSolver::record_solution(Solution* sol) {
  if (sol->is_valid()) {

    // RECORD BATTU
    this->bestsol->copy(sol);

    bool do_write_sol = false;  // do_write_sol pour recording !
    if (bestsol->get_cost() <= inst->best_known_cost()) {
      // RECORD ABSOLU BATTU
      do_write_sol = true;
    }

    this->nb_cons_no_improv = 0;

    // On enregistre cette solution dans un fichier temporaire
    string tmpname = this->bestsol->get_tmp_filename();

    // On peut vouloir enregistrer ce record dans un fichier
    if (do_write_sol) {
      U::write_file(tmpname, this->bestsol->to_s());
    }

  } else {
    // Solution invalide
    if (log2()) {
      cout << "X" << flush;
    }
  }
}
int AnnealingSolver::eval_cost(Solution* sol) {
  // Le cout peut changer en fonction de l'équilibrage
  int cost = 1000 * 1000 * sol->desequilibre + sol->length;
  // int cost = 100*sol->desequilibre + sol->length;
  return cost;
}

// Méthode principale de ce solver.
//
/// ATTENTION :
//    Pour faire un redémarrage à chaud, l'appli cliente doit manipuler
//    elle-même les attributs et solutions internes (e.g. xxxsol, ...). Ceci
//    n'pas été testé
bool AnnealingSolver::solve() {
  if (this->temp_init == -1) {
    this->temp = this->guess_temp_init(this->cursol, this->temp_init_rate, 100);
  } else {
    this->temp = this->temp_init;
  }

  while (!this->finished()) {
    logn4("temp=" + U::to_s(temp));
    this->nb_steps += 1;
    this->iter_in_step = 0;

    // boucle à température constante
    for (int iter = 1; iter < this->step_size; ++iter) {
      this->nb_test += 1;
      this->iter_in_step += 1;
      // sera peut-être remis à 0 lors de cette itér.
      this->nb_cons_no_improv += 1;

      this->mutate(testsol);

      // ON PEUT MODIFIER LA FONCTION DE COUT POUR LIMITER L"IMPORTANCE
      // DU DESEQUILIBRAGE
      // Solution::get_cost() donne le coût exact (lexicographique)
      // AnnealinSolver::eval_cost() donne sun coût aplani
      int degrad = this->testsol->get_cost() - this->cursol->get_cost();
      // int degrad = this->eval_cost(testsol) - this->eval_cost(cursol);

      logn5("\ndegrad=" + U::to_s(degrad));
      if (degrad <= 0) {
        // ce voisin est meilleur que la solution courante...
      } else {
        // ce voisin est plus mauvais que la solution courante ...
      }
    }  // fin boucle à temperature constante

    this->update_temp();

  }
  found = true;

  logn2("n---AnnealingSolver::solve END");
  return found;
}
//./
