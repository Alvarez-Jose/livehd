// This file is distributed under the BSD 3-Clause License. See LICENSE for details.
#include "main_api.hpp"

#include "cloud_api.hpp"
#include "eprp_utils.hpp"
#include "inou_lef_api.hpp"
#include "meta_api.hpp"
#include "top_api.hpp"

std::string Main_api::main_path;

void setup_inou_code_gen();
void setup_inou_liveparse();
void setup_inou_pyrope();
void setup_inou_verific();
void setup_inou_yosys();
void setup_inou_firrtl();
void setup_pass_fplan();
void setup_pass_lnast_tolg();
void setup_cops_live();

// add new setup function prototypes here

void Main_api::init() {
  for (const auto &it : Pass_plugin::get_registry()) {
    // fmt::print("function:{}\n", it.first);
    it.second();
  }

  //setup_inou_liveparse();
  setup_inou_pyrope();
  setup_inou_yosys();
  //setup_pass_fplan();
  //setup_pass_lnast_tolg();
  /* setup_inou_verific(); */
  /* setup_cops_live(); */

  // FIXME beyond this point (to delete some of them)

  Top_api::setup(Pass::eprp);  // *

  Meta_api::setup(Pass::eprp);   // lgraph.*
  Cloud_api::setup(Pass::eprp);  // cloud.*

  main_path = Eprp_utils::get_exe_path();
}
