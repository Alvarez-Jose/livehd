//  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

#include "edge.hpp"
#include "lgraph.hpp"
#include "node.hpp"
#include "node_pin.hpp"

static_assert(sizeof(XEdge::Compact) == 8);
static_assert(sizeof(Node_pin::Compact) == 12);
static_assert(sizeof(Node_pin::Compact_class) == 4);
static_assert(sizeof(Node::Compact) == 12);
static_assert(sizeof(Node::Compact_class) == 4);

XEdge::XEdge(LGraph *g, const Compact &c)
    : driver(g, Node_pin::Compact_class(c.driver_idx, false)), sink(g, Node_pin::Compact_class(c.sink_idx, true)) {
  I(sink.is_sink());
  I(driver.is_driver());
}

XEdge::XEdge(const Node_pin &src_, const Node_pin &dst_) : driver(src_), sink(dst_) {
  I(sink.is_sink());
  I(driver.is_driver());

  I(driver.get_top_lgraph() == sink.get_top_lgraph());
}

void XEdge::del_edge() {
  I(driver.get_class_lgraph() == sink.get_class_lgraph());

  bool deleted = driver.get_class_lgraph()->del_edge(driver, sink);
  I(deleted);
}

void XEdge::add_edge() { driver.get_class_lgraph()->add_edge(driver, sink); }

void XEdge::add_edge(uint32_t bits) { driver.get_class_lgraph()->add_edge(driver, sink, bits); }
