#include "data_flow/data_flow.h"
#include "ir/ir.h"
#include "optimization/optimization.h"
#include "util/io.h"

int main(int argc, char** argv) {
  char* input = read_file(argv[1]);
  OIR* ir     = parse_OIR(input);
  data_flow_reaching_definition(ir);
  optimization_propagation(ir);
  data_flow_available_expression(ir);
  optimization_common_subexpression_elimination(ir);
  data_flow_liveness(ir);
  optimization_dead_code_elimination(ir);
  print_OIR(stdout, ir);
  release_OIR(ir);
  free(input);
  return 0;
}
