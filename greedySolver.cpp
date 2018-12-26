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
bool GreedySolver::solve(bool randomized) {
  logn1("\n---GreedySolver::solve START instance\n");

  if(randomized) {
    logn1("\n --- RANDOM Greedy solver\n");
  }

  Solution* sol = new Solution(inst);

  logn4("\nGreedySolver::solve tri des stations\n");
  // liste triée par déséquilibre des stations
  vector<Station*> stations_triees;
  for(int i = 0; i < inst->stations.size(); i++){
    stations_triees.push_back(inst->stations[i]);
  }

  // tri de la liste (tri à bulles)
  for(int i = 0; i < stations_triees.size()-1; i++) {
    for(int j = i; j < stations_triees.size()-1; j++) {
      if(stations_triees[j]->deficit() >= stations_triees[j+1]->deficit()) {
        Station* tmp_station = stations_triees[j];
        stations_triees[j]   = stations_triees[j+1];
        stations_triees[j+1] = tmp_station;
      }
    }
  }

	while(!stations_triees.empty()) {
		logn2("-- Max number of stations to sample in the greedy solver to optionize");
		cout << "number of stations " <<  stations_triees.size() << endl;
		tuple<Station*, Circuit*, int, int> best_couple;

		unsigned int old_score = -1;
		unsigned int new_score = old_score;

    logn1("-- Max number of stations to sample in the greedy solver to optionize");
		int MAX_SAMPLE = 10;
		if(stations_triees.size() < MAX_SAMPLE) {
			MAX_SAMPLE = stations_triees.size();
		}
    logn1("-- Max number of candidates for random greedy to optionize");
    long nb_candidates_greedy = Options::args->nb_candidates_greedy;
    int N_CANDID = randomized ? nb_candidates_greedy : 1;
    // we store the best candidates
    vector<tuple<Station*, Circuit*, int, int> > candidates;

    // looping over the stations
		for(int i = 0; i < MAX_SAMPLE; i++) {

      // looping over the circuits
			for(int j = 0; j < inst->remorques.size(); j++) {


				int positions = sol->circuits[j]->stations.size();
				for(int k = 0; k <= positions; k++) {

					Solution* new_sol = new Solution(inst);
					new_sol->copy(sol);


					new_sol->circuits[j]->insert(stations_triees[i], k);

					new_sol->update();
					new_score = new_sol->get_cost();

          // no need for all the comparisons if we don't have enough candidates
          if(candidates.size() >= N_CANDID) {
            // best score among the candidates
            int highest_index = 0;
            unsigned int highest_score = get<3>(candidates[highest_index]);
            for(int l = 1; l < candidates.size(); l++) {
              if(get<3>(candidates[l]) > highest_score) {
                highest_index = l;
                highest_score = get<3>(candidates[highest_index]);
              }
            }

            // we replace the lowest ranking
            if(new_score < highest_score) 
              candidates[highest_index] = make_tuple(stations_triees[i], sol->circuits[j], k, new_score);


          } else {
            candidates.push_back(make_tuple(stations_triees[i], sol->circuits[j], k, new_score));
          }

					delete new_sol;
				}
			}
		}

    best_couple = candidates[rand() % N_CANDID];

		Station* assigned_station = get<0>(best_couple);
		Circuit* selected_circuit = get<1>(best_couple);
		int insert_pos = get<2>(best_couple);
		selected_circuit->insert(assigned_station, insert_pos);

		sol->update();

		// removing the selected station from the list
		auto it = std::find(stations_triees.begin(), stations_triees.end(),	assigned_station);
		if (it != stations_triees.end())
			stations_triees.erase(it);

	}

  this->found = true;
  this->solution = sol;
  logn3("GreedySolver::solve: END");
  return this->found;
}

//./
