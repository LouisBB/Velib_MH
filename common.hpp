#ifndef COMMON_H
#define COMMON_H

////////////////////////////////////////////////////////////////////////////////
// Quelques includes ou déclarations pour tout le projet
//
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// #include <getopt.h>
#include <time.h>

#include <sys/types.h>
#include <unistd.h>  // pour getpid()
#include <cassert>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <algorithm>
// #include <deque>
// #include <functional>
// #include <iterator>
#include <list>
#include <map>
#include <unordered_map>
#include <vector>
// #include <memory>
// #include <numeric>
#include <set>
// #include <stack>
// #include <utility>

using namespace std;

// extern "C" {
// #include "glpk.h"
// }

////////////////////////////////////////////////////////////////////////////////
// Fichiers spécifique à l'application

// Quelques defines qui pourront être transformés en options
#define EPS 1.0e-10

#include "options.hpp"

// Quelques classes génériques du projet
#include "logger.hpp"
#include "util.hpp"

// Quelques prédéclaration de classe (parfois nécessaire)
class Instance;
class Place;
class Remorque;
class Station;
// class Arc;
class Circuit;
class Solution;


// TODO : NE PAS INCLURE ICI LES CLASSES SPECIFIQUES.
// IL FAUT LES INCLURE DANS LES FICHIERS QUI LES UTILISENT VRAIMENT.

#endif /* COMMON_H */
//./
