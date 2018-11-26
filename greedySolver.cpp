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
  Solution* sol = new Solution(inst);

  // liste triée par déséquilibre des stations
  map<int, Station*> stations_triees;
  for(int i = 0; i < inst->stations.size(); i++){
    stations_triees[i] = inst->stations[i];
  }

  // tri de la liste (tri à bulles)
  for(int i = 0; i < stations_triees.size()-1; i++) {
    for(int j = i; j < stations_triees.size()-1; j++) {
      if(stations_triees[j]->deficit() <= stations_triees[j+1]->deficit()) {
        Station* tmp_station = stations_triees[j];
        stations_triees[j]   = stations_triees[j+1];
        stations_triees[j+1] = tmp_station;
      }
    }
  }

  /*
   * tant que la liste est non-vide, on ajoute les stations une à une
   * aux camions qui conviennent le mieux
   */

  for(int i = 0; i < stations_triees.size(); i++) {
    // on boucle sur les camions pour trouver celui au circuit le plus adapté
    Station* tmp_station = stations_triees[i];
    int diff = tmp_station->deficit();

    // boucle sur les remorques
    pair<int, int> candidat; // <id du circuit ; manque du camion>
    candidat.first = -1;
    candidat.second = -1;
    bool candidate_found = false;
    for(int j = 0; j < inst->remorques.size(); j++) {
      Circuit* tmp_circuit = sol->circuits[j];
      int charge_max = tmp_circuit->remorque->capa;
      Station* latest_station = tmp_circuit->stations[tmp_circuit->stations.size()-1];
      int charge_actuelle = tmp_circuit->charges[latest_station];

      // surplus de vélos dans la station
      if(diff >= 0) {
          // le camion peut fournir les vélos, il est choisi !
        if(charge_max - charge_actuelle < diff) {
          tmp_circuit->stations.push_back(tmp_station);
          tmp_circuit->equilibrate_eleve(); // TODO verifier que ça va bien ici !
          candidate_found = true;
          break;

        } // sinon, on le met comme candidat s'il est meilleur que le précédent
        else {
          if(diff - charge_max + charge_actuelle < candidat.second) {
            candidat.first = j;
            candidat.second = diff - charge_max + charge_actuelle;
          }
        }
      } // déficit de vélos dans la stations
      else {
        // le camion peut donner les vélos : il est choisi !
        if(charge_actuelle > abs(diff)) {
          tmp_circuit->stations.push_back(tmp_station);
          tmp_circuit->equilibrate_eleve(); // TODO : vérifier que ça va bien ici
          candidate_found = true;
          break;
        } // sinon, on l'ajoute en candidat
        else {
          // notre camion a plus à donner que le candidat actuel
          if(candidat.second < charge_actuelle) {
            candidat.first = j;
            candidat.second = charge_actuelle;
          }
        }
      }

    }

    /*
     *  si on a pas trouvé de candidat fonctionnant dirctement, on ajoute
     * la station au candidat qui convient "le mieux"
     */

    if(!candidate_found) {
      sol->circuits[candidat.first]->stations.push_back(tmp_station);
    }
  }

  found = false;
  return found;
}

//./
