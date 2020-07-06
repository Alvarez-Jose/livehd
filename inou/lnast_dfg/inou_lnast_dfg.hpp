//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#pragma once
#include <string>
#include <vector>

#include "lgedgeiter.hpp"
#include "lgraph.hpp"
#include "lbench.hpp"
#include "lnast.hpp"
#include "pass.hpp"
#include "likely.hpp"
#include "cfg_lnast.hpp"
#include "pass_bitwidth.hpp"
#include "pass_lgraph_to_lnast.hpp"


class Inou_lnast_dfg : public Pass {
private:
  std::shared_ptr<Lnast> lnast;

  absl::flat_hash_map<std::string_view, Node_pin>                       vname2attr_dpin; //for dummy attribute node construction, vn = variable non-ssa name, dpin = last attr dpin within "any" attributes
  absl::flat_hash_map<Lnast_ntype::Lnast_ntype_int, Node_Type_Op>       primitive_type_lnast2lg;
  absl::flat_hash_map<std::string, Node_pin>                            name2dpin;       // for scalar variable
  absl::flat_hash_map<std::string_view, Node_pin>                       vname2bits_dpin; // variable name (no ssa) to bitwidth //FIXME->sh: to be deprecated after new BW
  absl::flat_hash_map<std::pair<std::string, std::string>, std::string> tup_keyname2pos; // FIXME->sh: should be able to be deprecated, check after the BW is working
  absl::flat_hash_map<std::string, std::pair<Node_pin, uint16_t>>       tn2head_maxlen;  // FIXME->sh: should be able to be deprecated, check after the BW is working


protected:
  std::vector<LGraph *> do_tolg(std::shared_ptr<Lnast> l);
  void                  do_resolve_tuples(LGraph *dfg);
  static void           do_assignment_or_elimination(LGraph *dfg);
  static void           do_dead_code_elimination(LGraph *dfg);

  void lnast2lgraph                           (LGraph *dfg);
  void setup_lgraph_outputs_and_final_var_name(LGraph *dfg);
  void process_ast_stmts            (LGraph *dfg, const Lnast_nid &lnidx_stmts);
  Node process_ast_assign_op        (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_dp_assign_op     (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_nary_op          (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_logical_op       (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_as_op            (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_label_op         (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_if_op            (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_phi_op           (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_uif_op           (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_func_call_op     (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_func_def_op      (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_sub_op           (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_for_op           (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_while_op         (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_tuple_struct     (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_concat_op        (LGraph *dfg, const Lnast_nid &lnidx);
  void process_ast_tuple_add_op     (LGraph *dfg, const Lnast_nid &lnidx_ta);
  void process_ast_tuple_get_op     (LGraph *dfg, const Lnast_nid &lnidx_tg);
  void process_ast_attr_set_op      (LGraph *dfg, const Lnast_nid &lnidx_aset);
  void process_ast_attr_get_op      (LGraph *dfg, const Lnast_nid &lnidx_aget);
  void process_ast_tuple_phi_add_op (LGraph *dfg, const Lnast_nid &lnidx_tpa);


  Node         setup_node_opr_and_lhs         (LGraph *dfg, const Lnast_nid &lnidx_opr);
  Node_pin     setup_node_assign_and_lhs      (LGraph *dfg, const Lnast_nid &lnidx_opr);
  Node_pin     setup_ref_node_dpin            (LGraph *dfg, const Lnast_nid &lnidx, bool from_phi = false, bool from_concat = false);
  Node_Type_Op decode_lnast_op                (const Lnast_nid &lnidx_opr);
  void         setup_dpin_ssa                 (Node_pin &dpin, std::string_view var_name, uint16_t subs);
  void         setup_lnast_to_lgraph_primitive_type_mapping();
  void         nary_node_rhs_connections      (LGraph *dfg, Node &opr_node, const std::vector<Node_pin> &opds, bool is_subt);
  void         setup_clk                      (LGraph *dfg, Node &reg_node);


  static bool is_register          (std::string_view name) {return name.substr(0, 1) == "#" ; }
  static bool is_input             (std::string_view name) {return name.substr(0, 1) == "$" ; }
  static bool is_output            (std::string_view name) {return name.substr(0, 1) == "%" ; }
  static bool is_const             (std::string_view name) {return (std::isdigit(name[0]) || name.at(0) == '-'); }
  static bool is_default_const     (std::string_view name) {return name.substr(0,13) == "default_const"; }
  static bool is_err_var_undefined (std::string_view name) {return name.substr(0,17) == "err_var_undefined"; }
  static bool is_scalar            (Node_pin dpin) {return dpin.get_node().get_type().op != TupAdd_Op; }


  // tuple related
  Node_pin     setup_tuple_ref               (LGraph *dfg, std::string_view tup_name);
  Node_pin     setup_key_dpin                (LGraph *dfg, std::string_view key_name);
  void         reconnect_to_ff_qpin          (LGraph *dfg, const Node &tg_node);
  static bool  tuple_get_has_key_name        (const Node &tup_get);
  static bool  tuple_get_has_key_pos         (const Node &tup_get);
  static bool  is_tup_get_target             (const Node &tup_add, std::string_view tup_get_target);
  static bool  is_tup_get_target             (const Node &tup_add, uint32_t         tup_get_target);
  static void  collect_node_for_deleting     (const Node &node, absl::flat_hash_set<Node> &to_be_deleted);


  // attribute related
  bool check_new_var_chain (const Lnast_nid &lnidx_opr);


  // eprp callbacks
  static void tolg                  (Eprp_var &var);
  static void resolve_tuples        (Eprp_var &var);
  static void assignment_or_elimination(Eprp_var &var);
  static void dce                   (Eprp_var &var);
  static void dbg_lnast_ssa         (Eprp_var &var);

public:
  explicit Inou_lnast_dfg(const Eprp_var &var);

  static void setup();
};

