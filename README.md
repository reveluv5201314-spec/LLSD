# LLSD
# 项目简介

本项目是一个面向带标签约束的最短路径查询问题的图算法系统，主要用于在复杂图结构中高效处理“满足标签约束条件的最短路径查询”。

该系统通过构建图的结构化索引与层次化表示，将原始图数据转化为可高效查询的索引结构，从而显著提升在复杂约束条件下的路径查询效率。

## 项目目标

传统最短路径算法（如 Dijkstra）在面对标签约束、多条件过滤等场景时效率较低。本项目旨在解决以下问题：

1.如何在带标签约束的图中高效查询最短路径  
2.如何减少复杂约束带来的搜索空间爆炸问题  
3.如何在预处理与查询效率之间取得平衡  

## 项目结构  
├── Graph.cpp / Graph.h      图结构构建与算法逻辑  
├── Tree.cpp / Tree.h        图的层次化结构表示  
├── utility.cpp / utility.h  基础数据结构与工具函数  
├── test.cpp                 实验与测试入口  

## 如何运行

### 编译方式  

```js
g++ -std=c++17 -O2 Graph.cpp Tree.cpp utility.cpp test.cpp -o LLSD  
```

### 运行方式  

```js
  .\LLSD.exe [test set] [LSD index] [LLSD index] [hierarchical parameter] [test mode] [query set]
```

其中:  
[test set]表示输入的无向有权图数据集,格式为{总边数,[起始点,终止点,边权重,边标签],[起始点,终止点,边权重,边标签]...}  
[LSD index]表示将生成或读取的LSD索引名称,包含分解树结构  
[LLSD index]表示将生成或读取的LLSD索引名称  
[hierarchical parameter]表示层次化参数α,取值范围为0-100  
[test mode]表示运行方式,0表示生成索引,1表示测试数据  
[query set]表示将读取的测试集名称  

