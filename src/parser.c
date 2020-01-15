#include "parser.h"

#include <ctype.h>
#include <string.h>

#include "container/map.h"
#include "util.h"

static BasicBlock* dummy_copy(BasicBlock* block) {
  return block;
}
DECLARE_MAP(BasicBlock*, BBMap)
DEFINE_MAP(dummy_copy, release_void, BasicBlock*, BBMap)

typedef struct {
  const char* cursor;
  OIR* ir;
  BasicBlock* current_block;  // ref
  BBMap* block_map;
} Env;

static void skip_space(Env* env) {
  while (isspace(*env->cursor)) {
    env->cursor++;
  }
}

static void skip_comment(Env* env) {
  if (*env->cursor != '#') {
    return;
  }
  while (*env->cursor != '\n') {
    env->cursor++;
  }
  env->cursor++;
}

static void parse_char(Env* env, char c) {
  if (*env->cursor != c) {
    error("%c is expected, but %c was found", c, *env->cursor);
  }
  env->cursor++;

  skip_space(env);
}

static long parse_number(Env* env) {
  unsigned id = strtol(env->cursor, (char**)&(env->cursor), 10);
  skip_space(env);
  return id;
}

static char* parse_string(Env* env) {
  const char* init = env->cursor;
  while (isalpha(*env->cursor)) {
    env->cursor++;
  }
  char* string = strndup(init, env->cursor - init);
  skip_space(env);
  return string;
}

static Reg* parse_Reg(Env* env) {
  parse_char(env, 'v');
  unsigned id = parse_number(env);

  if (id >= env->ir->reg_count) {
    env->ir->reg_count = id + 1;
  }

  return new_virtual_Reg(id);
}

static Reg* parse_destination(Env* env) {
  Reg* r = parse_Reg(env);
  parse_char(env, '=');
  return r;
}

static Inst* add_inst(Env* env, InstKind kind) {
  Inst* inst = new_Inst(env->ir->inst_count++, kind);
  push_back_InstList(env->ir->instructions, inst);
  return inst;
}

static BasicBlock* add_block(Env* env, const char* name) {
  BasicBlock* block = new_BasicBlock(env->ir->block_count++);
  push_back_BBList(env->ir->blocks, block);
  insert_BBMap(env->block_map, name, block);
  return block;
}

static Inst* parse_mov(Env* env, Reg* rd) {
  Reg* rs = parse_Reg(env);

  if (rd == NULL) {
    error("rd is required for 'MOV' instruction");
  }

  Inst* inst = add_inst(env, IR_MOV);
  inst->rd   = rd;
  push_RegVec(inst->rs, rs);
  return inst;
}

static Inst* parse_imm(Env* env, Reg* rd) {
  long imm = parse_number(env);

  if (rd == NULL) {
    error("rd is required for 'IMM' instruction");
  }

  Inst* inst = add_inst(env, IR_IMM);
  inst->rd   = rd;
  inst->imm  = imm;
  return inst;
}

static Inst* parse_add(Env* env, Reg* rd) {
  Reg* rs1 = parse_Reg(env);
  Reg* rs2 = parse_Reg(env);

  if (rd == NULL) {
    error("rd is required for 'ADD' instruction");
  }

  Inst* inst = add_inst(env, IR_ADD);
  inst->rd   = rd;
  push_RegVec(inst->rs, rs1);
  push_RegVec(inst->rs, rs2);
  return inst;
}

static Inst* parse_equal(Env* env, Reg* rd) {
  Reg* rs1 = parse_Reg(env);
  Reg* rs2 = parse_Reg(env);

  if (rd == NULL) {
    error("rd is required for 'EQUAL' instruction");
  }

  Inst* inst = add_inst(env, IR_EQUAL);
  inst->rd   = rd;
  push_RegVec(inst->rs, rs1);
  push_RegVec(inst->rs, rs2);
  return inst;
}

static Inst* parse_label(Env* env, Reg* rd) {
  char* name = parse_string(env);

  if (rd != NULL) {
    error("rd is unnecessary for 'LABEL' instruction");
  }

  BasicBlock* block = add_block(env, name);

  Inst* inst       = add_inst(env, IR_LABEL);
  inst->label_name = name;
  inst->label      = block;

  if (env->current_block == NULL) {
    env->ir->entry = block;
  } else {
    env->current_block->instructions->to =
        prev_InstListIterator(back_InstList(env->ir->instructions));
  }
  env->current_block                     = block;
  env->current_block->instructions->from = back_InstList(env->ir->instructions);

  return inst;
}

static Inst* parse_branch(Env* env, Reg* rd) {
  Reg* rs  = parse_Reg(env);
  char* l1 = parse_string(env);
  char* l2 = parse_string(env);

  if (rd != NULL) {
    error("rd is unnecessary for 'BRANCH' instruction");
  }

  Inst* inst = add_inst(env, IR_BRANCH);
  push_RegVec(inst->rs, rs);
  inst->then_name = l1;
  inst->else_name = l2;

  return inst;
}

static Inst* parse_jump(Env* env, Reg* rd) {
  char* label = parse_string(env);

  if (rd != NULL) {
    error("rd is unnecessary for 'JUMP' instruction");
  }

  Inst* inst      = add_inst(env, IR_JUMP);
  inst->jump_name = label;

  return inst;
}

static Inst* parse_return(Env* env, Reg* rd) {
  Reg* rs = parse_Reg(env);

  if (rd != NULL) {
    error("rd is unnecessary for 'RETURN' instruction");
  }

  Inst* inst = add_inst(env, IR_RETURN);
  push_RegVec(inst->rs, rs);

  env->ir->exit = env->current_block;

  return inst;
}

static Inst* parse_Inst(Env* env) {
  Reg* rd = NULL;
  if (*env->cursor == 'v') {
    rd = parse_destination(env);
  }

  char* name = parse_string(env);
  Inst* inst;

#define IS_SAME(sv, sc) (strlen(sv) == sizeof(sc) - 1 && memcmp(sv, sc, sizeof(sc) - 1) == 0)
  if (IS_SAME(name, "MOV")) {
    inst = parse_mov(env, rd);
  } else if (IS_SAME(name, "IMM")) {
    inst = parse_imm(env, rd);
  } else if (IS_SAME(name, "ADD")) {
    inst = parse_add(env, rd);
  } else if (IS_SAME(name, "EQUAL")) {
    inst = parse_equal(env, rd);
  } else if (IS_SAME(name, "LABEL")) {
    inst = parse_label(env, rd);
  } else if (IS_SAME(name, "BRANCH")) {
    inst = parse_branch(env, rd);
  } else if (IS_SAME(name, "JUMP")) {
    inst = parse_jump(env, rd);
  } else if (IS_SAME(name, "RETURN")) {
    inst = parse_return(env, rd);
#undef IS_SAME
  } else {
    error("unexpected instruction %s", name);
  }

  free(name);
  return inst;
}

static void resolve_labels(Env* env) {
  BasicBlock* cur = env->ir->entry;
  FOR_EACH (Inst*, inst, InstList, env->ir->instructions) {
    switch (inst->kind) {
      case IR_LABEL:
        cur = inst->label;
        break;
      case IR_BRANCH:
        if (!lookup_BBMap(env->block_map, inst->then_name, &inst->then_)) {
          error("could not find label %s", inst->then_name);
        }
        if (!lookup_BBMap(env->block_map, inst->else_name, &inst->else_)) {
          error("could not find label %s", inst->else_name);
        }
        connect_BasicBlock(cur, inst->then_);
        connect_BasicBlock(cur, inst->else_);
        break;
      case IR_JUMP:
        if (!lookup_BBMap(env->block_map, inst->jump_name, &inst->jump)) {
          error("could not find label %s", inst->jump_name);
        }
        connect_BasicBlock(cur, inst->jump);
        break;
      default:
        break;
    }
  }
}

static Env* init_Env(const char* p) {
  Env* env       = calloc(1, sizeof(Env));
  env->cursor    = p;
  env->ir        = new_OIR();
  env->block_map = new_BBMap(16);
  return env;
}

static OIR* finalize_Env(Env* env) {
  release_BBMap(env->block_map);
  OIR* ir = env->ir;
  free(env);
  return ir;
}

OIR* parse(const char* p) {
  Env* env = init_Env(p);
  while (*env->cursor) {
    skip_space(env);
    skip_comment(env);
    parse_Inst(env);
    skip_comment(env);
  }
  env->current_block->instructions->to = back_InstList(env->ir->instructions);
  resolve_labels(env);
  return finalize_Env(env);
}
