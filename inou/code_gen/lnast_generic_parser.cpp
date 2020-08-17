
#include "lnast_generic_parser.hpp"
#include <cstring>
#include <string>

std::string Prp_parser::ref_name(std::string prp_term){
    return prp_term;
}

std::string Prp_parser::ref_name(std::string_view prp_term){
    return std::string(prp_term);
}

std::string_view Prp_parser::stmt_sep(){
  return stmt_separator;
}
std::string_view Cpp_parser::stmt_sep(){
  return stmt_separator;
}
std::string_view Ver_parser::stmt_sep(){
  return stmt_separator;
}

std::string_view Prp_parser::get_lang_type(){
  return lang_type;
}
std::string_view Cpp_parser::get_lang_type(){
  return lang_type;
}
std::string_view Ver_parser::get_lang_type(){
  return lang_type;
}


std::string_view Prp_parser::debug_name_lang(Lnast_ntype node_type){
  return node_type.debug_name_pyrope();
}
std::string_view Cpp_parser::debug_name_lang(Lnast_ntype node_type){
  return node_type.debug_name_cpp();
}
std::string_view Ver_parser::debug_name_lang(Lnast_ntype node_type){
  return node_type.debug_name_verilog();
}


std::string_view Prp_parser::start_else_if(){
  return ("} elif (");
}
std::string_view Cpp_parser::start_else_if(){
  return ("} else if (");
}
std::string_view Ver_parser::start_else_if(){
  return ("end else if (");
}

std::string_view Ver_parser::end_else_if(){
  return ("end");
}

std::string_view Ver_parser::start_else(){
  return ("end else begin\n");
}


std::string_view Ver_parser::end_cond(){
  return (") begin\n");
}

std::string_view Ver_parser::end_if_or_else(){
  return ("end\n");
}

std::string_view Ver_parser::for_stmt_beg() {return "begin\n";}
std::string_view Ver_parser::for_stmt_end() {return "end\n";}

std::string_view Prp_parser::for_cond_mid(){
  return " in ";
}
std::string_view Cpp_parser::for_cond_mid(){
  return ": ";
}
std::string_view Ver_parser::for_cond_mid(){
  return ": ";//TODO
}

std::string_view Prp_parser::for_cond_beg(){
  return " ";
}
std::string_view Cpp_parser::for_cond_beg(){
  return " ( auto ";
}
std::string_view Ver_parser::for_cond_beg(){
  return " ( auto ";//TODO
}

std::string_view Prp_parser::for_cond_end(){
  return " ";
}
std::string_view Cpp_parser::for_cond_end(){
  return ") ";
}
std::string_view Ver_parser::for_cond_end(){
  return ") ";//TODO
}

std::string_view Ver_parser::assign_node_strt() {
  return "assign ";
}

std::string Cpp_parser::starter(std::string_view filename){
  std::string _filename = std::string (filename);
  return absl::StrCat("void ", _filename, "::cycle(");
}

//header related functions:
std::string_view Cpp_parser::supporting_ftype(){
  return supp_ftype;
}
std::string Cpp_parser::supporting_fstart(std::string basename_s){
  return absl::StrCat("file: ", basename_s, "\n#pragma once");
}
std::string Cpp_parser::supporting_fend(std::string basename_s){
  return absl::StrCat("<<EOF ", basename_s);
}

std::string Cpp_parser::main_fstart(std::string basename, std::string basename_s) {
  return absl::StrCat("file: ", basename, "\n#include \"", basename_s, "\"\n");
}

