#include "instance.hpp"

Instance* Instance::new_velib_r1_s050() {
  // Cette instance correspond à l'instance v6 du projet OROC-RO-MH p2017
  // une seule remorque et limité à 50 stations :
  //    velib velib_6_s200_r1_k30..30_v30 1 200
  //

  Instance* inst = new Instance();
  inst->name = "test_r1_s050";
  inst->add_remorque("r1    25  63     30");

  inst->add_station("s1    80  91      6     5     5");
  inst->add_station("s2    76  33      7     0     7");
  inst->add_station("s3    92  48     10     0     0");
  inst->add_station("s4    45   4      7     4     7");
  inst->add_station("s5     1  74     10     5    10");
  inst->add_station("s6    84  81      8     8     8");
  inst->add_station("s7    17  67      8     2     6");
  inst->add_station("s8    64  79      5     3     2");
  inst->add_station("s9    70  89      8     5     5");
  inst->add_station("s10   14   7      5     0     3");
  inst->add_station("s11   27  95     10     5     6");
  inst->add_station("s12   71  50      8     1     0");
  inst->add_station("s13    3   8      6     4     3");
  inst->add_station("s14   72  32      8     5     8");
  inst->add_station("s15   90  38      8     1     8");
  inst->add_station("s16   43  18     10     0    10");
  inst->add_station("s17   10  47      5     4     3");
  inst->add_station("s18   40   8      9     1     4");
  inst->add_station("s19    1  95      6     4     2");
  inst->add_station("s20   75  34      5     3     2");
  inst->add_station("s21   96  84      7     2     2");
  inst->add_station("s22   59  72      7     7     5");
  inst->add_station("s23   31   4      9     9     4");
  inst->add_station("s24   65  90     10     6     3");
  inst->add_station("s25   20  17      6     6     2");
  inst->add_station("s26    4  74      8     5     3");
  inst->add_station("s27   67  35      6     6     3");
  inst->add_station("s28   58  65      9     8     1");
  inst->add_station("s29    5  37      8     2     7");
  inst->add_station("s30   85  87      7     0     0");
  inst->add_station("s31   49  80     10     7     8");
  inst->add_station("s32   43  12      9     6     2");
  inst->add_station("s33   34   4      9     2     0");
  inst->add_station("s34   28  87      6     1     3");
  inst->add_station("s35   85  44      6     3     1");
  inst->add_station("s36   35  78      7     3     1");
  inst->add_station("s37   19  82      5     1     1");
  inst->add_station("s38   94  51      5     0     2");
  inst->add_station("s39   43  19      7     1     5");
  inst->add_station("s40    7  13      7     4     6");
  inst->add_station("s41   91  57      7     2     4");
  inst->add_station("s42   59  17      6     4     6");
  inst->add_station("s43   75  85      8     1     1");
  inst->add_station("s44   81  91      5     4     5");
  inst->add_station("s45   82   1      5     1     1");
  inst->add_station("s46   90  46      6     5     0");
  inst->add_station("s47   59  99      9     9     5");
  inst->add_station("s48   27   4     10     3     4");
  inst->add_station("s49   65  42      9     9     5");
  inst->add_station("s50   85  89     10     2     1");

  inst->build_arcs();
  return inst;
}
