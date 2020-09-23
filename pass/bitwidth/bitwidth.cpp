//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#include "pass_bitwidth.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

#include "bitwidth.hpp"
#include "bitwidth_range.hpp"
#include "lbench.hpp"
#include "lgedgeiter.hpp"
#include "lgraph.hpp"

// Useful for debug
//#define PRESERVE_ATTR_NODE

Bitwidth::Bitwidth(bool _hier, int _max_iterations) :  max_iterations(_max_iterations), hier(_hier) {}


void Bitwidth::do_trans(LGraph *lg) {
  /* Lbench b("pass.bitwidth"); */
  bw_pass(lg);
}

void Bitwidth::process_const(Node &node) {
  auto dpin = node.get_driver_pin();
  auto it   = bwmap.emplace(dpin.get_compact(), Bitwidth_range(node.get_type_const()));
  forward_adjust_dpin(dpin, it.first->second);
}

void Bitwidth::process_flop(Node &node) {
  I(node.has_sink_pin_connected(1));
  auto d_dpin = node.get_sink_pin(1).get_driver_pin();

  Lconst max_val;
  Lconst min_val;
  auto   it3 = bwmap.find(d_dpin.get_compact());
  if (it3 != bwmap.end()) {
    max_val = it3->second.get_max();
    min_val = it3->second.get_min();
  } else if (d_dpin.get_bits()) {
    Lconst b(1);
    max_val = b.lsh_op(d_dpin.get_bits()) - 1;
  } else if (node.get_driver_pin(0).get_bits()) {
    // At least propagate backward the width
    if (d_dpin.get_bits()==0 || d_dpin.get_bits() > node.get_driver_pin(0).get_bits())
      d_dpin.set_bits(node.get_driver_pin(0).get_bits());
    return;
  } else {
    if (d_dpin.has_name())
      fmt::print("pass.bitwidth flop:{} has input pin:{} unconstrained\n", node.debug_name(), d_dpin.get_name());
    else
      fmt::print("pass.bitwidth flop:{} has some inputs unconstrained\n", node.debug_name());
    not_finished = true;
    return;
  }

  bwmap.emplace(node.get_driver_pin(0).get_compact(), Bitwidth_range(min_val, max_val));
}

void Bitwidth::process_not(Node &node, XEdge_iterator &inp_edges) {
  I(inp_edges.size());  // Dangling???

  Lconst max_val;
  Lconst min_val;
  for (auto e : inp_edges) {
    auto it3 = bwmap.find(e.driver.get_compact());
    if (it3 != bwmap.end()) {
      if (max_val < it3->second.get_max())
        max_val = it3->second.get_max();

      if (min_val == 0 || min_val > it3->second.get_min())
        min_val = it3->second.get_min();
    } else if (e.driver.get_bits()) {
      Lconst b(1);
      b = b.lsh_op(e.driver.get_bits());
      if (b > max_val)
        max_val = b;

      min_val = Lconst(0) - max_val;
    } else {
      if (e.driver.has_name())
        fmt::print("pass.bitwidth not:{} has input pin:{} unconstrained\n", node.debug_name(), e.driver.get_name());
      else
        fmt::print("pass.bitwidth not:{} has some inputs unconstrained\n", node.debug_name());
      not_finished = true;
      return;
    }
  }

  bwmap.emplace(node.get_driver_pin(0).get_compact(), Bitwidth_range(min_val, max_val));
}

void Bitwidth::process_mux(Node &node, XEdge_iterator &inp_edges) {
  I(inp_edges.size());  // Dangling???

  Lconst max_val;
  Lconst min_val;
  for (auto e : inp_edges) {
    if (e.sink.get_pid() == 0)
      continue;  // Skip select

    auto it = bwmap.find(e.driver.get_compact());
    if (it != bwmap.end()) {
      if (max_val < it->second.get_max())
        max_val = it->second.get_max();

      if (min_val == 0 || min_val > it->second.get_min())
        min_val = it->second.get_min();
    } else if (e.driver.get_bits()) {
      Lconst b(1);
      b = b.lsh_op(e.driver.get_bits()) - 1; // TODO: sign handling
      if (b > max_val)
        max_val = b;

      min_val = Lconst(0);
    } else {
      if (e.driver.has_name())
        fmt::print("pass.bitwidth mux:{} has input pin:{} unconstrained\n", node.debug_name(), e.driver.get_name());
      else
        fmt::print("pass.bitwidth mux:{} has some inputs unconstrained\n", node.debug_name());
      not_finished = true;
      return;
    }
  }

  Lconst n_options(inp_edges.size() - 1 - 1);  // -1 for log and -1 for the select
  node.get_sink_pin(0).get_driver_pin().set_bits(n_options.get_bits());

  Bitwidth_range bw(min_val, max_val);
  bwmap.emplace(node.get_driver_pin(0).get_compact(), bw);
  node.get_driver_pin(0).set_bits(bw.get_bits());
}

void Bitwidth::process_shr(Node &node, XEdge_iterator &inp_edges) {
  I(inp_edges.size() == 2);

  auto a_dpin = node.get_sink_pin(0).get_driver_pin();
  auto n_dpin = node.get_sink_pin(1).get_driver_pin();

  auto a_it = bwmap.find(a_dpin.get_compact());
  auto n_it = bwmap.find(n_dpin.get_compact());

  Bitwidth_range a_bw(0);
  if (a_it == bwmap.end()) {
    a_bw = Bitwidth_range(a_dpin.get_bits());
  } else {
    a_bw = a_it->second;
  }

  Bitwidth_range n_bw(0);
  if (n_it == bwmap.end()) {
    n_bw = Bitwidth_range(n_dpin.get_bits());
  } else {
    n_bw = n_it->second;
  }

  if (a_bw.get_bits() == 0 || n_bw.get_bits() == 0) {
    if (node.get_driver_pin().has_name())
      fmt::print("pass.bitwidth shr:{} has input pin:{} unconstrained\n", node.debug_name(), node.get_driver_pin().get_name());
    else
      fmt::print("pass.bitwidth shr:{} has some inputs unconstrained\n", node.debug_name());
    return;
  }

  if (n_bw.get_min() > 0 && n_bw.get_min().is_i()) {
    auto max    = a_bw.get_max();
    auto min    = a_bw.get_min();
    auto amount = n_bw.get_min().to_i();
    max         = Lconst(max.get_raw_num() >> amount);
    min         = Lconst(min.get_raw_num() >> amount);

    Bitwidth_range bw(min, max);
    bwmap.emplace(node.get_driver_pin().get_compact(), bw);
    node.get_driver_pin().set_bits(bw.get_bits());
  } else {
    bwmap.emplace(node.get_driver_pin().get_compact(), a_bw);
    node.get_driver_pin().set_bits(a_bw.get_bits());
  }
}

void Bitwidth::process_sum(Node &node, XEdge_iterator &inp_edges) {
  I(inp_edges.size());  // Dangling sum??? (delete)

  Lconst max_val;
  Lconst min_val;
  for (auto e : inp_edges) {
    auto it = bwmap.find(e.driver.get_compact());
    if (it != bwmap.end()) {
      if (e.sink.get_pid() == 0 || e.sink.get_pid() == 1) {
        max_val = max_val + it->second.get_max();
        min_val = min_val + it->second.get_min();
      } else {
        fmt::print("max_val:{}, pmax_val:{}\n", max_val.to_i(), it->second.get_max().to_i());
        fmt::print("min_val:{}, pmin_val:{}\n", min_val.to_i(), it->second.get_min().to_i());
        max_val = max_val - it->second.get_min();
        min_val = min_val - it->second.get_max();
      }
    } else if (e.driver.get_bits()) {
      Lconst b(1);
      b = b.lsh_op(e.driver.get_bits()) - 1;
      //FIXME->sh: Debug on Lconst(0) - Lconst(0xFFFF) = 0 ??
      max_val = max_val + b;
      min_val = min_val + b;
#if 0
      if (e.sink.get_pid() == 0 || e.sink.get_pid() == 1) {
        max_val = max_val + b;
        min_val = min_val + b;
      } else {
        max_val = max_val - b;
        min_val = min_val - b;
      }
#endif
    } else {
      if (e.driver.has_name())
        fmt::print("pass.bitwidth sum:{} has input pin:{} unconstrained\n", node.debug_name(), e.driver.get_name());
      else
        fmt::print("pass.bitwidth sum:{} has some inputs unconstrained\n", node.debug_name());
      not_finished = true;
      return;
    }
  }
  bwmap.emplace(node.get_driver_pin(0).get_compact(), Bitwidth_range(min_val, max_val));
}

void Bitwidth::process_pick(Node &node) {
  auto data_dpin = node.get_sink_pin(0).get_driver_pin();
  auto out_dpin  = node.get_driver_pin();

  auto           it3 = bwmap.find(data_dpin.get_compact());
  Bitwidth_range bw(out_dpin.get_bits());
  if (it3 != bwmap.end()) {
    bw = it3->second;
  } else if (data_dpin.get_bits()) {
    bw = Bitwidth_range(data_dpin.get_bits());
  }

  if (bw.get_bits() <= out_dpin.get_bits()) {
    bwmap.emplace(node.get_driver_pin(0).get_compact(), bw);
  } else {
    bwmap.emplace(node.get_driver_pin(0).get_compact(), Bitwidth_range(out_dpin.get_bits()));
  }
}

void Bitwidth::process_comparator(Node &node) { bwmap.emplace(node.get_driver_pin(0).get_compact(), Bitwidth_range(1)); }

void Bitwidth::process_logic(Node &node, XEdge_iterator &inp_edges) {
  bool is_logic_op        = node.has_driver_pin_connected(0);
  bool is_logic_reduction = node.has_driver_pin_connected(1);

  if (is_logic_reduction) {
    bwmap.emplace(node.get_driver_pin(1).get_compact(), Bitwidth_range(1));
  }

  if (is_logic_op && inp_edges.size() >= 1) {
    Bits_t max_bits = 0;

    for (auto e : inp_edges) {
      auto   it   = bwmap.find(e.driver.get_compact());
      Bits_t bits = 0;
      if (it == bwmap.end()) {
        bits = e.driver.get_bits();
      } else {
        bits = it->second.get_bits();
      }
      if (bits == 0) {
        if (e.driver.has_name()) {
          fmt::print("pass.bitwidth gate:{} has input pin:{} unconstrained\n", node.debug_name(), e.driver.get_name());
        } else {
          fmt::print("pass.bitwidth gate:{} has some inputs unconstrained\n", node.debug_name());
        }
        not_finished = true;
        return;
      }
      if (bits > max_bits)
        max_bits = bits;
    }

    bwmap.emplace(node.get_driver_pin(0).get_compact(), Bitwidth_range(max_bits));
  }
}

void Bitwidth::process_logic_and(Node &node, XEdge_iterator &inp_edges) {
  bool logic_op        = node.has_driver_pin_connected(0);
  bool logic_reduction = node.has_driver_pin_connected(1);

  if (logic_reduction) {
    bwmap.emplace(node.get_driver_pin(1).get_compact(), Bitwidth_range(1));
  }

  // determine the min_bits among all inp_edges
  if (logic_op && inp_edges.size() >= 1) {
    Bits_t min_bits = UINT16_MAX;

    for (auto e : inp_edges) {
      auto   pit  = bwmap.find(e.driver.get_compact());
      Bits_t bits = 0;
      if (pit == bwmap.end()) {
        bits = e.driver.get_bits();
      } else {
        bits = pit->second.get_bits();
      }
      if (bits && bits < min_bits)
        min_bits = bits;
    }
    if (min_bits == UINT16_MAX) {
      fmt::print("pass.bitwidth and:{} does not have any constrained input\n", node.debug_name());
      not_finished = true;
      return;
    }

    bwmap.emplace(node.get_driver_pin(0).get_compact(), Bitwidth_range(min_bits));

    for (auto e : inp_edges) {
      auto bits = e.driver.get_bits();
      if (bits)
        continue;  // only handle unconstrained inputs

      if (e.driver.get_num_edges() > 1) {
        must_perform_backward = true;
      } else if (bits==0 || bits > min_bits) {
        // no other output, parent could follow the child
        e.driver.set_bits(min_bits);
      }
    }
  }
}

Bitwidth::Attr Bitwidth::get_key_attr(std::string_view key) {
  if (key.substr(0, 6) == "__bits")
    return Attr::Set_bits;

  if (key.substr(0, 5) == "__max")
    return Attr::Set_max;

  if (key.substr(0, 5) == "__min")
    return Attr::Set_min;

  if (key.substr(0, 11) == "__dp_assign")
    return Attr::Set_dp_assign;

  return Attr::Set_other;
}

void Bitwidth::process_attr_get(Node &node) {
  I(node.has_sink_pin_connected(1));
  auto dpin_key = node.get_sink_pin(1).get_driver_pin();
  I(dpin_key.get_node().is_type(TupKey_Op));

  auto key  = dpin_key.get_name();
  auto attr = get_key_attr(key);
  I(attr != Attr::Set_dp_assign);  // Not get attr with __dp_assign
  if (attr == Attr::Set_other) {
    not_finished = true;
    return;
  }

  I(node.has_sink_pin_connected(0));
  auto dpin_val = node.get_sink_pin(0).get_driver_pin();

  auto it = bwmap.find(dpin_val.get_compact());
  if (it == bwmap.end()) {
    not_finished = true;
    return;
  }
  auto &bw = it->second;

  Lconst result;
  if (attr == Attr::Set_bits) {
    result = Lconst(bw.get_bits());
  } else if (attr == Attr::Set_max) {
    result = bw.get_max();
  } else if (attr == Attr::Set_min) {
    result = bw.get_min();
  }

  fmt::print("attr_get key:{} dpin0:{} const:{} node:{}\n", key, dpin_val.debug_name(), result.to_pyrope(), node.debug_name());

  auto new_node = node.get_class_lgraph()->create_node_const(result);
  auto new_dpin = new_node.get_driver_pin();
  for (auto &out : node.out_edges()) {
    new_dpin.connect_sink(out.sink);
  }

  //#ifndef PRESERVE_ATTR_NODE
  if (!hier) // FIXME: once hier del works
    node.del_node();
  //#endif
}

void Bitwidth::process_attr_set_dp_assign(Node &node) {
  auto dpin_variable = node.get_sink_pin(2).get_driver_pin();
  auto dpin_value    = node.get_sink_pin(0).get_driver_pin();
  auto dpin_output   = node.get_driver_pin(0);

  auto it = bwmap.find(dpin_variable.get_compact());
  Bitwidth_range bw_variable(0);
  if (it != bwmap.end()) {
    bw_variable = it->second;
  } else if (dpin_variable.get_bits()) {
    bw_variable = Bitwidth_range(dpin_variable.get_bits());
  } else {
    if (dpin_output.has_name())
      fmt::print("pass.bitwidth {}:= is unconstrained\n", dpin_output.get_name());
    else
      fmt::print("pass.bitwidth := is unconstrained (node:{})\n", node.debug_name());
    return;
  }

  auto it2 = bwmap.find(dpin_value.get_compact());
  Bitwidth_range bw_value(0);
  if (it2 != bwmap.end()) {
    bw_value = it2->second;
  } else if (dpin_variable.get_bits()) {
    bw_value = Bitwidth_range(dpin_value.get_bits());
  } else {
    if (dpin_output.has_name())
      fmt::print("pass.bitwidth {}:= node:{} is unconstrained\n", dpin_output.get_name(), dpin_variable.debug_name());
    else
      fmt::print("pass.bitwidth := node:{} is unconstrained (node:{})\n", dpin_variable.debug_name(), node.debug_name());
    return;
  }

  // fmt::print("attr_set_dp_assign name:{} variable_bits:{} bw_value:{} node:{}\n", dpin_variable.debug_name(),
  // bw_variable.get_bits(), bw_value.get_bits(), node.debug_name());

  if (bw_value.get_bits() == 0 && bw_variable.get_bits() == 0) {  // Can not solve now
    if (dpin_output.has_name())
      fmt::print("pass.bitwidth {}:= is unconstrained\n", dpin_output.get_name());
    else
      fmt::print("pass.bitwidth := is unconstrained (node:{})\n", node.debug_name());
    return;
  } else {
    if (bw_value.get_bits() == 0) {
      fmt::print("pass.bitwidth := propagating bits:{} upwards to node:{}\n", bw_variable.get_bits(), dpin_value.debug_name());
      dpin_value.set_bits(bw_variable.get_bits());
      bwmap.emplace(dpin_value.get_compact(), bw_variable);
      return;
    }
    if (bw_variable.get_bits() == 0) {
      if (dpin_output.has_name())
        fmt::print("pass.bitwidth {}:= is unconstrained\n", dpin_output.get_name());
      else
        fmt::print("pass.bitwidth := is unconstrained (node:{})\n", node.debug_name());
      return;
    }
    if (bw_value.get_bits() <= bw_variable.get_bits()) {  // Already match
      for (auto e : node.out_edges()) {
        dpin_value.connect_sink(e.sink);
      }
    } else {  // rhs.bits > lhs.bits --> drop rhs bits and reconnect
      auto new_node  = node.get_class_lgraph()->create_node(Pick_Op);
      auto zero_node = node.get_class_lgraph()->create_node_const(Lconst(0));
      auto zero_dpin = zero_node.setup_driver_pin();
      auto new_dpin  = new_node.get_driver_pin();
      new_dpin.set_bits(bw_variable.get_bits());

      dpin_value.connect_sink(new_node.setup_sink_pin(0));
      zero_dpin.connect_sink(new_node.setup_sink_pin(1));
      for (auto e : node.out_edges()) {
        new_dpin.connect_sink(e.sink);
      }
    }
  }

  if (!hier) // FIXME: once hier del works
    node.del_node();
  fmt::print("DBG: delete dp_assign\n");
}

void Bitwidth::process_attr_set_new_attr(Node &node) {
  I(node.has_sink_pin_connected(1));
  auto dpin_key = node.get_sink_pin(1).get_driver_pin();
  auto key      = dpin_key.get_name();
  auto attr     = get_key_attr(key);
  if (attr == Attr::Set_other) {
    not_finished = true;
    return;
  }

  if (attr == Attr::Set_dp_assign) {
    process_attr_set_dp_assign(node);
    return;
  }

  I(node.has_sink_pin_connected(2));
  auto dpin_val = node.get_sink_pin(2).get_driver_pin();

  if (!dpin_key.get_node().is_type(TupKey_Op)) {
    not_finished = true;
    return;  // Can not handle now
  }

  I(dpin_key.has_name());

  auto attr_dpin = node.get_driver_pin(0);

  std::string_view dpin_name;
  if (attr_dpin.has_name())
    dpin_name = attr_dpin.get_name();

  // copy parent's bw for some judgement and then update to attr_set value
  Bitwidth_range bw(0);
  bool           parent_pending = false;
  if (node.has_sink_pin_connected(0)) {
    auto through_dpin = node.get_sink_pin(0).get_driver_pin();
    auto it           = bwmap.find(through_dpin.get_compact());
    if (it != bwmap.end()) {
      bw = it->second;
    } else {
      parent_pending = true;
    }
  }

  // fmt::print("attr_set_new name:{} key:{} bw_bits:{} node:{}\n", dpin_name, key, bw.get_bits(), node.debug_name());

  if (attr == Attr::Set_bits) {
    I(dpin_val.get_node().is_type_const());
    auto val = dpin_val.get_node().get_type_const();
    if (bw.get_bits() && bw.get_bits() > val.to_i()) {
      Pass::error("bitwidth missmatch. Variable {} needs {}bits, but constrained to {}bits\n",
                  dpin_name,
                  bw.get_bits(),
                  val.to_i());
    } else {
      if (bw.is_always_positive())
        bw.set_ubits(val.to_i());
      else
        bw.set_sbits(val.to_i());
    }
  } else if (attr == Attr::Set_max) {
    I(false);  // FIXME: todo
  } else if (attr == Attr::Set_min) {
    I(false);  // FIXME: todo
  } else {
    I(false);  // Attr::Set_dp_assign handled in another method
  }

  for (auto out_dpin : node.out_connected_pins()) {
    out_dpin.set_bits(bw.get_bits());
    bwmap.emplace(out_dpin.get_compact(), bw);
  }

  // upwards propagate for one step node
  if (parent_pending) {
    auto through_dpin = node.get_sink_pin(0).get_driver_pin();
    through_dpin.set_bits(bw.get_bits());
    bwmap.emplace(through_dpin.get_compact(), bw);
  }

  // dpin_val.dump_all_prp_vname();
}

void Bitwidth::process_attr_set_propagate(Node &node) {
  auto             attr_dpin = node.get_driver_pin(0);
  std::string_view dpin_name;
  if (attr_dpin.has_name())
    dpin_name = attr_dpin.get_name();

  I(node.has_sink_pin_connected(0));
  bool parent_data_pending = false;
  auto data_dpin = node.get_sink_pin(0).get_driver_pin();

  I(node.has_sink_pin_connected(3));
  auto parent_attr_dpin = node.get_sink_pin(3).get_driver_pin();

  Bitwidth_range data_bw(0);
  auto data_it = bwmap.find(data_dpin.get_compact());
  if (data_it != bwmap.end()) {
    data_bw = data_it->second;
  } else {
    parent_data_pending = true;
  }

  /* if (data_it == bwmap.end()) { */
  /*   fmt::print("attr_set propagate bwmap to AttrSet name:{}\n", dpin_name); */
  /*   not_finished = true; */
  /*   return; */
  /* } */
  /* auto &data_bw = data_it->second; */

  auto parent_attr_it = bwmap.find(parent_attr_dpin.get_compact());
  if (parent_attr_it == bwmap.end()) {
    fmt::print("attr_set propagate bwmap to AttrSet name:{}\n", dpin_name);
    not_finished = true;
    return;
  }
  const auto parent_attr_bw = parent_attr_it->second;

  fmt::print("attr_set_prop name:{} parent_attr.bits:{} data_bw.bits:{}\n",
             dpin_name,
             parent_attr_bw.get_bits(),
             data_bw.get_bits());

  if (parent_attr_bw.get_bits() && data_bw.get_bits()) {
    if (parent_attr_bw.get_bits() < data_bw.get_bits()) {
      Pass::error("bitwidth missmatch. Variable {} needs {}bits, but constrained to {}bits\n",
                  dpin_name,
                  data_bw.get_bits(),
                  parent_attr_bw.get_bits());
    } else if (parent_attr_bw.get_max() < data_bw.get_max()) {
      Pass::error("bitwidth missmatch. Variable {} needs {}max, but constrained to {}max\n",
                  dpin_name,
                  data_bw.get_max().to_pyrope(),
                  parent_attr_bw.get_max().to_pyrope());
    } else if (parent_attr_bw.get_min() > data_bw.get_min()) {
      Pass::error("bitwidth missmatch. Variable {} needs {}min, but constrained to {}min\n",
                  dpin_name,
                  data_bw.get_min().to_pyrope(),
                  parent_attr_bw.get_min().to_pyrope());
    }
  }

  for (auto out_dpin : node.out_connected_pins()) {
    out_dpin.set_bits(parent_attr_bw.get_bits());
    bwmap.emplace(out_dpin.get_compact(), parent_attr_bw);
  }

  if (parent_data_pending) {
    data_dpin.set_bits(parent_attr_bw.get_bits());
    fmt::print("data_dpin:{}\n", data_dpin.debug_name());
    fmt::print("data_dpin bits:{}\n", data_dpin.get_bits());
    bwmap.emplace(data_dpin.get_compact(), parent_attr_bw);
  }

}

void Bitwidth::process_attr_set(Node &node) {
  if (node.has_sink_pin_connected(1)) {
    process_attr_set_new_attr(node);
  } else {
    process_attr_set_propagate(node);
  }
}

// focusing on judging the parent is garbage or not
void Bitwidth::garbage_collect_support_structures(XEdge_iterator &inp_edges) {
  for (auto e : inp_edges) {
    auto pit = outcountmap.find(e.driver.get_node().get_compact());  // pit = parent iterator
    if (pit == outcountmap.end()) {
      continue;  // parent node not yet visited
    }

    auto n = pit->second;
    if (n <= 1) {  // I'm the only child, the parent bw_range object could be recycled
      outcountmap.erase(pit);
      for (auto parent_dpin : e.driver.get_node().out_connected_pins()) {
        auto it2 = bwmap.find(parent_dpin.get_compact());
        if (it2 != bwmap.end()) {  // Not all the nodes create bwmap (impossible to infer do not)
          forward_adjust_dpin(parent_dpin, it2->second);
          bwmap.erase(it2);
        }
      }
    } else {
      pit->second = n - 1;
    }
  }
}

void Bitwidth::forward_adjust_dpin(Node_pin &dpin, Bitwidth_range &bw) {
  auto bw_bits = bw.get_bits();
  if (bw_bits && bw_bits < dpin.get_bits()) {
    // fmt::print("bitwidth: bits:{}->{} for dpin:{}\n", dpin.get_bits(), bw_bits, dpin.debug_name());
    dpin.set_bits(bw_bits);
  }
}

void Bitwidth::set_graph_boundary(Node_pin &dpin, Node_pin &spin) {
  I(hier); // do not call unless hierarchy is set

  if (dpin.get_class_lgraph() == spin.get_class_lgraph())
    return;

  I(dpin.get_hidx() != spin.get_hidx());

  auto same_level_spin = spin.get_non_hierarchical();
  for (auto dpin_sub:same_level_spin.inp_driver()) {
    if (!dpin_sub.get_node().is_type(SubGraph_Op))
      continue;
    if (dpin_sub.get_bits()==0 || dpin_sub.get_bits() > dpin.get_bits())
      dpin_sub.set_bits(dpin.get_bits());
  }
}

void Bitwidth::bw_pass(LGraph *lg) {
  must_perform_backward = false;
  not_finished          = false;

  for(auto dpin:lg->get_graph_input_node(hier).out_setup_pins()) {
    if (dpin.get_bits())
      bwmap.emplace(dpin.get_compact(), Bitwidth_range(dpin.get_bits()));
  }

  outcountmap[lg->get_graph_input_node(hier).get_compact()] = lg->get_graph_input_node(hier).get_num_out_edges();

  for (auto node : lg->forward(hier)) {
    auto inp_edges = node.inp_edges();
    auto op        = node.get_type_op();

    //fmt::print("bitwidth node:{} lg:{}\n", node.debug_name(), node.get_class_lgraph()->get_name());

    if (inp_edges.empty() && (op != Const_Op && op != SubGraph_Op && op != LUT_Op && op != TupKey_Op)) {
      fmt::print("pass.bitwidth: removing dangling node:{}\n", node.debug_name());
      fmt::print("node:{}\n", node.debug_name());
      if (!hier) // FIXME: once hier del works
        node.del_node();
      continue;
    }

    outcountmap[node.get_compact()] = node.get_num_out_edges();

    if (op == Const_Op) {
      process_const(node);
    } else if (op == TupKey_Op || op == TupGet_Op || op == TupAdd_Op) {
      // Nothing to do for this
    } else if (op == Or_Op || op == Xor_Op) {
      process_logic(node, inp_edges);
    } else if (op == And_Op) {
      process_logic_and(node, inp_edges);
    } else if (op == AttrSet_Op) {
      process_attr_set(node);
      if (node.is_invalid())
        continue;
    } else if (op == AttrGet_Op) {
      process_attr_get(node);
      if (node.is_invalid())
        continue;
    } else if (op == Sum_Op) {
      process_sum(node, inp_edges);
    } else if (op == ShiftRight_Op) {
      process_shr(node, inp_edges);
    } else if (op == Not_Op) {
      process_not(node, inp_edges);
    } else if (op == SFlop_Op || op == AFlop_Op || op == FFlop_Op) {
      process_flop(node);
    } else if (op == Mux_Op) {
      process_mux(node, inp_edges);
    } else if (op == GreaterThan_Op || op == LessThan_Op || op == LessEqualThan_Op || op == Equals_Op
               || op == GreaterEqualThan_Op) {
      process_comparator(node);
    } else if (op == Pick_Op) {
      process_pick(node);
    } else {
      fmt::print("FIXME: node:{} still not handled by bitwidth\n", node.debug_name());
    }

    if (hier) {
      for (auto e:inp_edges) {
        set_graph_boundary(e.driver, e.sink);
      }
    }

    for (auto dpin : node.out_connected_pins()) {
      auto it = bwmap.find(dpin.get_compact());
      if (it == bwmap.end())
        continue;

      auto bw_bits = it->second.get_bits();

      if (bw_bits == 0 && it->second.is_overflow()) {
        fmt::print("bitwidth: dpin:{} has over {}bits (simplify first!)\n", dpin.debug_name(), it->second.get_raw_max());
        continue;
      }

      if (dpin.get_bits() && dpin.get_bits() >= bw_bits)
        continue;

      dpin.set_bits(bw_bits);
    }

    garbage_collect_support_structures(inp_edges);
  }

  for(auto dpin:lg->get_graph_output_node(hier).out_setup_pins()) {
    auto spin = dpin.get_sink_from_output();
    if (!spin.has_inputs())
      continue;

    auto out_driver = spin.get_driver_pin();

    I(!out_driver.is_invalid());
    auto it = bwmap.find(out_driver.get_compact());
    if (it != bwmap.end()) {
      forward_adjust_dpin(out_driver, it->second);

      bwmap.erase(it);
    }

    if (out_driver.get_bits()) {
      dpin.set_bits(out_driver.get_bits());
      if (hier)
        set_graph_boundary(out_driver, spin);
    }

    /* if (out_driver.get_bits()) { */
    /*   if (dpin.get_bits()) {  // output has been attr set bits */
    /*     if (dpin.get_bits() > out_driver.get_bits()) { */
    /*       return; */ 
    /*     } */ 
    /*   } */
    /*   dpin.set_bits(out_driver.get_bits()); */
    /* } */
  }

#ifndef PRESERVE_ATTR_NODE
  if (not_finished) {
    fmt::print("pass_bitwidth: could not converge\n");
  } else {
    // FIXME: this code may need to move to cprop if we have several types of
    // attributes. Delete only if all the attributes are finished
    //
    // Delete all the attr_set/get for bitwidth
    for (auto node : lg->fast()) {
      auto op = node.get_type_op();

      if (op == AttrSet_Op) {
        if (node.has_sink_pin_connected(1)) {
          auto key_dpin = node.get_sink_pin(1).get_driver_pin();
          auto attr     = get_key_attr(key_dpin.get_name());
          if (attr == Attr::Set_other)
            continue;
        }

        if (node.has_sink_pin_connected(0)) {
          auto data_dpin = node.get_sink_pin(0).get_driver_pin();

          for (auto e : node.out_edges()) {
            if (e.driver.get_pid() == 0) {
              e.sink.connect_driver(data_dpin);
            }
          }
        }
        if (!hier) // FIXME: once hier del works
          node.del_node();
      } else if (op == AttrGet_Op) {
        I(false);  // should be deleted by now if solved
      }
    }
  }
#endif

  if (must_perform_backward) {
    fmt::print("pass_bitwidth: some nodes need to back propagate width\n");
  }
}
