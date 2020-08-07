#!/bin/bash
rm -rf ./lgdb
rm -rf ./lgdb2

pts='if if2 nested_if ssa_rhs logic adder_stage ff_if' # tuple_if'
#simple_tuple
#tuple
#tuple_if
#tuple_if2
#tuple_if5
#tuple_nested_if

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
    echo ""
    echo ""
    echo ""
    echo "===================================================="
    echo "Compilation to get stable LGraph"
    echo "===================================================="

    echo "----------------------------------------------------"
    echo "Pyrope -> LNAST -> LGraph"
    echo "----------------------------------------------------"

    ${LGSHELL} "inou.pyrope files:inou/cfg/tests/${pt}.prp |> inou.lnast_dfg.tolg"
    if [ $? -eq 0 ]; then
      echo "Successfully create the inital LGraph with tuples: ${pt}.cfg"
    else
      echo "ERROR: Pyrope compiler failed: LNAST -> LGraph, testcase: ${pt}.cfg"
      exit 1
    fi

    ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    mv ${pt}.dot ${pt}.no_bits.tuple.reduced_or.dot

    echo ""
    echo ""
    echo ""
    echo "----------------------------------------------------"
    echo "cprop and bitwidth"
    echo "----------------------------------------------------"

    ${LGSHELL} "lgraph.open name:${pt} |> pass.cprop |> pass.bitwidth |> pass.cprop |> pass.bitwidth"
    if [ $? -eq 0 ]; then
      echo "Successfully optimize design bitwidth: ${pt}.v"
    else
      echo "ERROR: Pyrope compiler failed: bitwidth optimization, testcase: ${pt}.cfg"
      exit 1
    fi

    ${LGSHELL} "lgraph.open name:${pt} |> inou.graphviz.from verbose:false"
    mv ${pt}.dot ${pt}.or.dot

#############################################################

    echo ""
    echo ""
    echo ""
    echo "===================================================="
    echo "LG-LNAST interface verification"
    echo "===================================================="

    echo "----------------------------------------------------"
    echo "LGraph (golden) -> LNAST -> LGraph (new)"
    echo "----------------------------------------------------"

    ${LGSHELL} "lgraph.open name:${pt} |> pass.lgraph_to_lnast |> inou.lnast_dfg.tolg path:lgdb2"
    if [ $? -eq 0 ]; then
      echo "Successfully create the new LG: ${pt}.cfg"
    else
      echo "ERROR: Tester failed: LG -> LNAST -> LGraph, testcase: ${pt}.cfg"
      exit 1
    fi

    ${LGSHELL} "lgraph.open name:${pt} path:lgdb2 |> inou.graphviz.from verbose:false"
    mv ${pt}.dot ${pt}.newlg.no_bits.tuple.or.dot

    echo ""
    echo ""
    echo ""
    echo "----------------------------------------------------"
    echo "cprop and bitwidth (LGraph)"
    echo "----------------------------------------------------"

    ${LGSHELL} "lgraph.open name:${pt} path:lgdb2 |> pass.cprop |> pass.bitwidth |> pass.cprop |> pass.bitwidth"
    if [ $? -eq 0 ]; then
      echo "Successfully optimize design bitwidth on new lg: ${pt}.v"
    else
      echo "ERROR: Pyrope compiler failed on new lg: bitwidth optimization, testcase: ${pt}.cfg"
      exit 1
    fi

    ${LGSHELL} "lgraph.open name:${pt} path:lgdb2 |> inou.graphviz.from verbose:false"
    mv ${pt}.dot ${pt}.newlg.or.dot

    if [[ ${pt} == *_err* ]]; then
        echo "----------------------------------------------------"
        echo "Pass! This is a Compile Error Test, No Need to Generate Verilog Code "
        echo "----------------------------------------------------"
    else
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
          echo "ERROR: Pyrope compiler failed: verilog generation, testcase: ${pt}.cfg"
          exit 1
        fi


        echo ""
        echo ""
        echo ""
        echo "----------------------------------------------------"
        echo "Logic Equivalence Check"
        echo "----------------------------------------------------"

        ${LGCHECK} --implementation=${pt}.v --reference=./inou/cfg/tests/verilog_gld/${pt}.gld.v

        if [ $? -eq 0 ]; then
          echo "Successfully pass logic equivilence check!"
        else
          echo "FAIL: "${pt}".v !== "${pt}".gld.v"
          exit 1
        fi
    fi
    ${LGSHELL} "lgraph.open name:${pt} path:lgdb2 |> inou.graphviz.from verbose:false"


    #rm -f ${pt}.v
    rm -f ${pt}.cfg
    rm -f lnast.dot
    rm -f lnast.dot.gld
    rm -f lnast.nodes
    rm -f lnast.nodes.gld
done #end of for



