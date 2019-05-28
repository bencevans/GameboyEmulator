#pragma once

#include <iostream>
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

    void test_cb_00();
    void test_cb_01();
    void test_cb_02();
    void test_cb_03();
    void test_cb_04();
    void test_cb_05();
    
    void test_cb_10();
    void test_cb_11();
    void test_cb_12();
    void test_cb_13();
    void test_cb_14();
    void test_cb_15();
    
    void test_cb_18();
    void test_cb_19();
    void test_cb_1a();
    void test_cb_1b();
    void test_cb_1c();
    void test_cb_1d();

    void test_cb_30();
    void test_cb_31();
    void test_cb_32();
    void test_cb_33();
    void test_cb_34();
    void test_cb_35();

    void test_RLC(reg8* reg, uint8_t op_code);
    void test_RL(reg8* reg, uint8_t op_code);
    void test_RR(reg8* reg, uint8_t op_code);
    void test_SWAP(reg8* reg, uint8_t op_code);


    bool assert(bool outcome)
    {
        if (! outcome)
            std::cout << "F";
        else
            std::cout << ".";
        return outcome;
    }

    bool assert_equal(unsigned int left, unsigned int right)
    {
        if (! assert(left == right))
        {
            std::cout << std::hex << std::endl << "Equal failure: 0x"
                << left << " != 0x" << right << std::endl;
                return false;
        }
        return true;
    }

    bool assert_equal(uint16_t left, uint16_t right)
    {
        return assert_equal((unsigned int)left, (unsigned int)right);
    }
    bool assert_equal(uint8_t left, uint8_t right)
    {
        return assert_equal((unsigned int)left, (unsigned int)right);
    }

};
