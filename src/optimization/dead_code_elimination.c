#include "optimization/dead_code_elimination.h"

void optimization_dead_code_elimination(OIR* ir) {
  FOR_EACH (Inst*, inst, InstList, ir->instructions) {
    if (inst->rd == NULL) {
      continue;
    }

    if (get_BitSet(inst->live_out, inst->rd->virtual)) {
      continue;
    }

    InstListIterator* next_it = remove_InstListIterator(ir->instructions, it_inst);
    it_inst                   = prev_InstListIterator(next_it);
  }
}
