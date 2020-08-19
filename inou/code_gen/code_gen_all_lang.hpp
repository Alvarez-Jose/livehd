
#pragma once

#include <string>
#include <string_view>
#include "code_gen.hpp"

class Code_gen_all_lang {
//protected:
//  virtual std::string_view stmt_sep() = 0;
public:
  Code_gen_all_lang() {};

  virtual std::string_view stmt_sep() = 0;
  virtual std::string_view get_lang_type() = 0;
  virtual std::string_view debug_name_lang(Lnast_ntype node_type) = 0;
  virtual std::string_view start_else_if() = 0;

  virtual std::string_view end_else_if() { return ("}"); }
  virtual std::string_view start_else() { return ("} else {\n"); }
  virtual std::string_view end_cond() {return ") {\n";}
  virtual std::string_view end_if_or_else() { return "}\n";}

  std::string_view start_cond() {return "if (";}
  std::string_view tuple_stmt_sep() {return ", ";}
  std::string_view tuple_begin() {return "(";}
  std::string_view tuple_end() {return ")";}

  //TODO: func def related parameters: need to make language specific! currently as per pyrope:
  std::string_view func_begin() {return "";}
  std::string_view func_name(std::string_view func_name) {return func_name;}
  std::string_view param_start(bool param_exist) {if (param_exist) return " = :("; else return "= :";}
  std::string_view func_param_sep() {return ", ";}
  std::string_view param_end(bool param_exist) {if (param_exist) return ")"; else return "";}
  std::string print_cond(std::string cond_val) {
    if(cond_val != "")
      return (absl::StrCat(" when ", cond_val));
    else
      return cond_val;
  }
  std::string_view func_stmt_strt() {return ":{\n";}
  std::string_view func_stmt_end() {return "}\n";}
  std::string_view func_end() {return "";}

  //for related parameters:
  virtual std::string_view for_cond_beg() = 0;
  virtual std::string_view for_cond_mid() = 0;
  virtual std::string_view for_cond_end() = 0;
  virtual std::string_view for_stmt_beg() {return "{\n";}
  virtual std::string_view for_stmt_end() {return "}\n";}

  //TODO: while related parameters: need to make language specific! currently as per pyrope:
  std::string_view while_cond_beg() {return "(";}
  std::string_view while_cond_end() {return ") ";}

  //TODO: select related parameters: need to make language specific! currently as per pyrope:
  std::string_view select_init(std::string select_type) {
    if (select_type=="bit")
      return "[[";
    else
      return "[";
  }
  std::string_view select_end(std::string select_type) {
    if (select_type=="bit")
      return "]]";
else
      return "]";
  }

  //this func is to truncate the %/$/# during cpp or verilog conversion
  bool has_prefix(std::string_view test_string) {
    return (test_string.find("$")==0 || test_string.find("#") ==0 || test_string.find("%") == 0);
  }
  virtual std::string ref_name(std::string_view prp_term) {
    if(Code_gen_all_lang::has_prefix(prp_term)) {
      std::string _prp_term = std::string(prp_term);
      return _prp_term.substr(1);
    } else
      return std::string(prp_term);
  }

  virtual std::string ref_name(std::string prp_term) {
    if(Code_gen_all_lang::has_prefix(prp_term)) {
      return prp_term.substr(1);
    } else
      return prp_term;
  }

  //in verilog, assign stmt starts with assign keyword. thus this function.
  virtual std::string_view assign_node_strt() {return "";}

  virtual std::string starter(std::string_view filename) {return "";} ;

  //for header file:
  virtual std::string supporting_fend(std::string basename_s) {return "";} ;
  virtual std::string supporting_fstart(std::string basename_s) {return "";} ;
  virtual std::string_view supporting_ftype() {return "";};
  virtual std::string_view supp_buffer_to_print() {return "";};

  //for main file (cpp file)
  virtual std::string main_fstart(std::string basename, std::string basename_s) {return absl::StrCat("file: ", basename, "\n");} ;
  virtual bool convert_parameters(std::string key, std::string ref) {return false;};

  //for cpp constructor:
  virtual std::string outline_cpp(std::string modname) {return "";};
  virtual void call_get_maps() {;};//for debugging only
};

