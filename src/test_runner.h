#pragma once

#include "./ram.h"
#include "./cpu.h"
#include "./vpu.h"

class TestRunner
{
public:
    TestRunner(VPU *vpu_inst, CPU *cpu_inst, RAM *ram_inst);
    CPU* cpu_inst;
    VPU* vpu_inst;
    RAM* ram_inst;
    void run_tests();
    void test_00();
    void test_01();
    void test_02();
    void test_03();
    void test_04();

};
