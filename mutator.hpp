#ifndef MUTATOR_H
#define MUTATOR_H

#include "common.hpp"
#include "instance.hpp"
#include "solution.hpp"

class Mutator {
 private:
  Mutator(Solution* sol);

 public:
  virtual ~Mutator();

  static void mutate_intra_circuit_2opt(Solution* sol);

  static void mutate_intra_circuit_move_station(Solution* sol);

  static void mutate_inter_circuit_move_station(Solution* sol);

  static void mutate_inter_circuit_move_branch(Solution* sol);
};

#endif
