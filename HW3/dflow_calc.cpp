/* 046267 Computer Architecture - Winter 20/21 - HW #3               */
/* Implementation for the dataflow statistics calculator */

#include "dflow_calc.h"
#include <vector>

using namespace std;

// The structure that defines a node in the graph
typedef struct node {
    int line_num;
    int opcode_weight;
}node, * pNode;

// FUNCTION TO FIND THE LINES OF THE DEPENDENCIES OF THE INPUT LINE
void build_graph(int line_num, const InstInfo progTrace[], pNode dep1, pNode dep2);

// Defines the structure of the entire graph
class depend_graph {
public:
    depend_graph(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts);
    int get_inst_depth(int inst);
    int recursive_depth(int inst);
    int get_dependency(int inst, int* src1, int* src2);
    int get_depth();

private:
    vector <vector<node>> adjList_;
    vector <int> exit_;
    int num_inst_;
};

// Constructs a graph
void build_graph(int line_num, const InstInfo progTrace[], pNode dep1, pNode dep2) {
    int depend1 = progTrace[line_num].src1Idx;
    int depend2 = progTrace[line_num].src2Idx;
    dep1->opcode_weight = -1;
    dep2->opcode_weight = -1;
    dep1->line_num = -1;
    dep2->line_num = -1;
    for (int i = line_num - 1; i >= 0; i--) {
        if ((dep1->line_num == -1) && (progTrace[i].dstIdx == depend1))
            dep1->line_num = i;
        if ((dep2->line_num == -1) && (progTrace[i].dstIdx == depend2))
            dep2->line_num = i;
    }
}

// C'tor
depend_graph::depend_graph(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) :num_inst_(numOfInsts) {//BUILD THE DEPENDENCY GRAPH AND CLASS
    vector <node> vec_helper;
    auto arr = new  int[num_inst_];
    for (int i = 0; i < num_inst_; i++) {
        arr[i] = 1;
    }
    node node_helper, dep1, dep2;
    for (int i = 0; i < num_inst_; i++) {
        node_helper.line_num = i;
        node_helper.opcode_weight = opsLatency[progTrace[i].opcode];
        vec_helper.push_back(node_helper);
        build_graph(i, progTrace, &dep1, &dep2);
        if (dep1.line_num < num_inst_) {
            vec_helper.push_back(dep1);
            if (dep1.line_num != -1)
                arr[dep1.line_num] = 0;
        }
        if (dep2.line_num < num_inst_) {
            vec_helper.push_back(dep2);
            if (dep2.line_num != -1)
                arr[dep2.line_num] = 0;
        }
        adjList_.push_back(vec_helper);
        vec_helper.clear();
    }
    for (int i = 0; i < num_inst_; i++) {
        if (arr[i] == 1)exit_.push_back(i);
    }
    delete[] arr;
}

// Getes the maximum depth of an instruction's dependencies
int depend_graph::get_inst_depth(int inst) {
    if (inst >= num_inst_)return-1;
    int max1 = 0;
    int max2 = 0;
    max1 = recursive_depth((adjList_[inst])[1].line_num);
    max2 = recursive_depth((adjList_[inst])[2].line_num);
    if (max2 == -1 || max1 == -1) return -1;
    return (max1 > max2) ? max1 : max2;

}

int depend_graph::recursive_depth(int inst) {
    if (inst >= num_inst_)return-1;
    if (inst == -1)return 0;
    int helper1 = recursive_depth((adjList_[inst])[1].line_num);
    int helper2 = recursive_depth((adjList_[inst])[2].line_num);
    if ((helper1 == -1) || (helper2 == -1)) return -1;
    int helper = (helper1 > helper2) ? helper1 : helper2;
    return adjList_[inst][0].opcode_weight + helper;

}

int depend_graph::get_dependency(int inst, int* src1, int* src2) {
    if (inst >= num_inst_) return -1;
    *src1 = (adjList_[inst])[1].line_num;
    *src2 = (adjList_[inst])[2].line_num;
    return 0;
}

int depend_graph::get_depth() {
    int max = 0;
    int helper;
    for (unsigned int i = 0; i < exit_.size(); i++) {
        helper = recursive_depth(exit_[i]);
        if (helper == -1)return -1;
        if (max < helper) max = helper;
    }
    return max;
}


ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    auto testGraph = new depend_graph(opsLatency, progTrace, numOfInsts);
    return testGraph;
}

void freeProgCtx(ProgCtx ctx) {
    delete (depend_graph*)ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    depend_graph* graph = (depend_graph*)ctx;
    int depth = graph->get_inst_depth(theInst);
    return depth;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int* src1DepInst, int* src2DepInst) {
    depend_graph* graph = (depend_graph*)ctx;
    int depth = graph->get_dependency(theInst, src1DepInst, src2DepInst);
    return depth;
}

int getProgDepth(ProgCtx ctx) {
    depend_graph* graph = (depend_graph*)ctx;
    int depth = graph->get_depth();
    return depth;
}