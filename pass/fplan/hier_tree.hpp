/*
  This class holds a hierarchy hypertree (coming from json_inou or elsewhere) and two algorithms that operate on the tree:
  1. Hierarchy discovery, where we walk the tree and subdivide it if the number of components in the tree grows beyond a certain
  number.
  2. Tree collapsing, where nodes with small areas get combined into supernodes with larger areas.
*/
/*
#pragma once

#include <memory>  // for shared_ptr
#include <string>  // for strings

#include "graph_info.hpp"

// a struct representing a node in a hier_tree
struct Hier_node {
  std::string name;

  double area = 0.0;  // area of the leaf (if node is a leaf)

  std::shared_ptr<Hier_node> parent      = {nullptr};
  std::shared_ptr<Hier_node> children[2] = {nullptr, nullptr};

  int graph_subset;  // which part of the graph the leaf is responsible for (if node is a leaf)

  bool is_leaf() const { return children[0] == nullptr && children[1] == nullptr; }
};

typedef std::shared_ptr<Hier_node> phier;

class Hier_tree {
public:
  // take in a vector of all nodes in the netlist, and convert it to a tree.
  // min_num_components sets the minimum number of components required to trigger analysis of the hierarchy
  // any node with a smaller area than min_area gets folded into a new supernode with area >= min_area
  Hier_tree(Graph_info&& g, unsigned int num_components);

  // copies require copying the entire tree and are very expensive.
  Hier_tree(const Hier_tree& other) = delete;
  Hier_tree& operator=(const Hier_tree& other) = delete;

  // moves defined since copies are deleted
  Hier_tree(Hier_tree&& other) noexcept : ginfo(std::move(other.ginfo)), root(other.root) {  // TODO: delete stuff here?
  }

  Hier_tree& operator=(Hier_tree&& other) noexcept {
    // TODO: write this
    I(false);
    return *this;
  }

  // print the hierarchy
  void print() const;

  // returns a new tree with small leaf nodes collapsed together (Algorithm 2 in HiReg)
  void collapse(double threshold_area);

  // discover similar subgraphs in the collapsed hierarchy
  void discover_regularity();

private:
  // graph containing the divided netlist
  Graph_info&& ginfo;

  // data used by min_cut
  struct Min_cut_data {
    int  d_cost;  // difference between the external and internal cost of the node
    bool active;  // whether the node is being considered for a swap or not
  };

  typedef decltype(graph::Bi_adjacency_list().vert_map<Min_cut_data>()) Min_cut_map;

  // make a partition of the graph minimizing the number of edges crossing the cut and keeping in mind area (modified kernighan-lin
  // algorithm)
  std::pair<int, int> min_wire_cut(Graph_info& info, int cut_set);

  // make a node for insertion into the hierarchy
  phier make_hier_node(const int set);

  // create a hierarchy tree out of existing hierarchies
  phier make_hier_tree(phier t1, phier t2);

  // perform hierarchy discovery
  phier discover_hierarchy(Graph_info& g, int start_set, unsigned int num_components);

  double find_area(phier node) const;

  void print_node(const phier& node) const;

  phier collapse(phier node, double threshold_area);

  // generator used to make unique node names
  unsigned int node_number = 0;

  // root node of hierarchy tree
  phier root;

  // vector of altered hierarchy trees with nodes collapsed
  std::vector<phier> collapsed_hiers;

  // find patterns in the collapsed hierarchy
  void find_most_freq_pattern();
};
*/