#!/bin/bash
rm -rf ./lgdb
rm -rf ./lgdb2

pts='trivial trivial_and assigns compare trivial1 mux simple_flop' # latch add'
#TO ADD LIST, but have bugs:
#picker -- pick op not yet implemented in lnast2lg
#simple_add -- output 'h' has 1 extra bit, happens in pass.bitwidth
#simple_flop, shift, cse_basic -- problems arise with flops somewhere??
#add -- encountering problems with minus
#arith -- same problem with minus, can't do %
#compare2 -- lnast2lg doesn't yet support range/bit_sel/etc. for pick nodes
#trivial2 -- subgraphs (try to test this)
#consts -- don't have join->concat implemented yet

LGSHELL=./bazel-bin/main/lgshell
LGCHECK=./inou/yosys/lgcheck

if [ ! -f $LGSHELL ]; then
    if [ -f ./main/lgshell ]; then
        LGSHELL=./main/lgshell
        echo "lgshell is in $(pwd)"
    else
        echo "ERROR: could not find lgshell binary in $(pwd)";
    fi
fi

for pt in $pts
do
    echo "----------------------------------------------------"
    echo "Verilog -> LGraph"
    echo "----------------------------------------------------"

    ${LGSHELL} "inou.yosys.tolg files:inou/yosys/tests/${pt}.v"
    if [ $? -eq 0 ]; then
      echo "Successfully created the inital LGraph using Yosys: ${pt}.v"
    else
      echo "ERROR: Verilog -> LGraph failed... testcase: ${pt}.v"
      exit 1
    fi


    ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    mv ${pt}.dot ${pt}.origlg.dot


    echo "----------------------------------------------------"
    echo "LGraph -> LNAST -> LGraph"
    echo "----------------------------------------------------"

    ${LGSHELL} "lgraph.open name:${pt} |> pass.lgraph_to_lnast |> lnast.dump |> inou.lnast_dfg.tolg path:lgdb2"
    if [ $? -eq 0 ]; then
      echo "Successfully went from LG -> LN -> LG: ${pt}.v"
    else
      echo "ERROR: LGraph -> LNAST -> LGraph failed... testcase: ${pt}.v"
      exit 1
    fi

    #echo ""
    #echo ""
    #echo ""
    #echo "----------------------------------------------------"
    #echo "Compiler Optimizations"
    #echo "----------------------------------------------------"

    #${LGSHELL} "lgraph.open name:${pt} path:lgdb2 |> pass.cprop |> pass.bitwidth |> pass.cprop"
    #if [ $? -eq 0 ]; then
    #  echo "Successfully resolve the tuple chain in new lg: ${pt}.v"
    #else
    #  echo "ERROR: Pyrope compiler failed on new lg: resolve tuples, testcase: ${pt}.v"
    #  exit 1
    #fi
    #${LGSHELL} "lgraph.open name:${pt} path:lgdb2 |> inou.graphviz.from verbose:false"
    #mv ${pt}.dot ${pt}.newlg.dot

    echo ""
    echo ""
    echo ""
    echo "----------------------------------------------------"
    echo "LGraph Optimization"
    echo "----------------------------------------------------"

    ${LGSHELL} "lgraph.open name:${pt} |> pass.cprop |> pass.bitwidth |> pass.cprop |> pass.bitwidth |> pass.cprop |> pass.bitwidth |> pass.bitwidth"
    if [ $? -eq 0 ]; then
      echo "Successfully optimize design on new lg: ${pt}.v"
    else
      echo "ERROR: Failed to optimize design on new lg, testcase: ${pt}.v"
      exit 1
    fi
    ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"

    echo ""
    echo ""
    echo ""
    echo "----------------------------------------------------"
    echo "LGraph -> Verilog"
    echo "----------------------------------------------------"

    ${LGSHELL} "lgraph.open name:${pt} path:lgdb2 |> inou.yosys.fromlg"
    if [ $? -eq 0 ] && [ -f ${pt}.v ]; then
      echo "Successfully generate Verilog: ${pt}.v"
      rm -f  yosys_script.*
    else
      echo "ERROR: Yosys failed: verilog generation, testcase: ${pt}.v"
      exit 1
    fi

    echo ""
    echo ""
    echo ""
    echo "----------------------------------------------------"
    echo "Logic Equivalence Check"
    echo "----------------------------------------------------"

    ${LGCHECK} --implementation=${pt}.v --reference=./inou/yosys/tests/${pt}.v

    if [ $? -eq 0 ]; then
      echo "Successfully pass logic equivilence check!"
    else
      echo "FAIL: "${pt}".v !== "${pt}".gld.v"
      exit 1
    fi
    ${LGSHELL} "lgraph.open name:${pt} path:lgdb2 |> inou.graphviz.from verbose:false"

done #end of for
