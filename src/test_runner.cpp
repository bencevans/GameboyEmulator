
#include "./test_runner.h"
#include <iostream>

TestRunner::TestRunner(VPU *vpu_inst, CPU *cpu_inst, RAM *ram_inst)
{
    this->cpu_inst = cpu_inst;
    this->vpu_inst = vpu_inst;
    this->ram_inst = ram_inst;
}


void TestRunner::run_tests()
{
    std::cout << "Starting tests" << std::endl;
    this->test_00();
    this->test_01();
    this->test_02();
    this->test_03();
    this->test_04();
    
    this->test_cb_00();
    this->test_cb_01();
    this->test_cb_02();
    this->test_cb_03();

    this->test_cb_10();
    this->test_cb_11();
    this->test_cb_12();

    std::cout << std::endl << "Completed tests" << std::endl;

}

void TestRunner::test_00()
{
    std::cout << "0x00";
    // Ensure that SP is 0
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0000);

    this->ram_inst->memory[0x0000] = 0x00;
    this->cpu_inst->tick();
    
    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0001);
}

void TestRunner::test_01()
{
    std::cout << "0x01";

    this->cpu_inst->r_pc.value = 0x0000;
    this->cpu_inst->r_f.value = 0xff;

    // Setup b and c
    this->cpu_inst->r_b.value = 0xff;
    this->cpu_inst->r_c.value = 0xff;

    // Store load BC, d16 in memory and store 16 bit value
    this->ram_inst->memory[0x0000] = 0x01;
    // Store value in reverse byte
    this->ram_inst->memory[0x0001] = 0xcd;
    this->ram_inst->memory[0x0002] = 0xab;

    this->cpu_inst->tick();
    this->assert_equal(this->cpu_inst->r_b.value, 0xab);
    this->assert_equal(this->cpu_inst->r_c.value, 0xcd);
    this->assert_equal(this->cpu_inst->r_bc.value(), 0xabcd);
    
    // Ensure flags haven't changed
    this->assert_equal(this->cpu_inst->r_f.value, 0xff);
    
    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0003);
}

void TestRunner::test_02()
{
    std::cout << "0x02";
    this->cpu_inst->r_pc.value = 0x0000;
    this->cpu_inst->r_f.value = 0xff;

    // Setup b and c, as destination memory address
    this->cpu_inst->r_a.value = 0xd6;
    this->cpu_inst->r_b.value = 0xfb;
    this->cpu_inst->r_c.value = 0x23;
    this->ram_inst->memory[0xfb23] = 0x00;

    // Setup memory
    this->ram_inst->memory[0x0000] = 0x02;

    this->cpu_inst->tick();

    // Assert that registers were unchanged
    this->assert_equal(this->cpu_inst->r_a.value, 0xd6);
    this->assert_equal(this->cpu_inst->r_b.value, 0xfb);
    this->assert_equal(this->cpu_inst->r_c.value, 0x23);
    this->assert_equal(this->cpu_inst->r_bc.value(), 0xfb23);

    // Ensure that byte has been put into memory
    this->assert_equal(this->ram_inst->memory[0xfb23], 0xd6);

    // Ensure flags haven't changed
    this->assert_equal(this->cpu_inst->r_f.value, 0xff);

    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0001);
}

void TestRunner::test_03()
{
    std::cout << "0x03";

    // Test standard increment
    this->cpu_inst->r_pc.value = 0x0000;
    this->cpu_inst->r_f.value = 0xff;
    this->cpu_inst->r_b.value = 0xaf;
    this->cpu_inst->r_c.value = 0xfe;
    this->ram_inst->memory[0x0000] = 0x03;
    this->cpu_inst->tick();

    // Assert that registers were unchanged
    this->assert_equal(this->cpu_inst->r_b.value, 0xaf);
    this->assert_equal(this->cpu_inst->r_c.value, 0xff);
    this->assert_equal(this->cpu_inst->r_bc.value(), 0xafff);

    // Ensure flags haven't changed
    this->assert_equal(this->cpu_inst->r_f.value, 0xff);

    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0001);

    // Test half-carry
    
    // Setup memory
    this->cpu_inst->r_f.value = 0x00;
    this->ram_inst->memory[0x0001] = 0x03;
    this->cpu_inst->tick();

    // Assert that registers were unchanged
    this->assert_equal(this->cpu_inst->r_b.value, 0xb0);
    this->assert_equal(this->cpu_inst->r_c.value, 0x00);
    this->assert_equal(this->cpu_inst->r_bc.value(), 0xb000);

    // Ensure flags haven't changed
    this->assert_equal(this->cpu_inst->r_f.value, 0x00);
    

    // Test Carry
    // Setup memory and run instruction
    this->cpu_inst->r_f.value = 0x00;
    this->cpu_inst->r_bc.set_value(0xffff);
    this->ram_inst->memory[0x0002] = 0x03;
    this->cpu_inst->tick();

    // Assert that registers were unchanged
    this->assert_equal(this->cpu_inst->r_b.value, 0x00);
    this->assert_equal(this->cpu_inst->r_c.value, 0x00);
    this->assert_equal(this->cpu_inst->r_bc.value(), 0x0000);

    // Ensure flags haven't changed
    this->assert_equal(this->cpu_inst->r_f.value, 0x00);
}

void TestRunner::test_04()
{
    std::cout << "0x04";
    // Test standard increment
    this->cpu_inst->r_pc.value = 0x0000;
    this->cpu_inst->r_f.value = 0xf0;
    this->cpu_inst->r_b.value = 0x58;
    this->ram_inst->memory[0x0000] = 0x04;
    this->cpu_inst->tick();

    // Assert that registers were unchanged
    assert_equal(this->cpu_inst->r_b.value, 0x59);

    // Ensure zero, subtract and half-carry flags have been unset
    // Carry has been left
    assert_equal(this->cpu_inst->r_f.value, 0x10);

    // Ensure that SP has moved on
    assert_equal(this->cpu_inst->r_pc.value, 0x0001);

    // Test half-carry
    
    // Setup memory
    this->cpu_inst->r_f.value = 0x00;
    this->cpu_inst->r_b.value = 0x5f;
    this->ram_inst->memory[0x0001] = 0x04;
    this->cpu_inst->tick();

    // Assert that registers were unchanged
    assert_equal(this->cpu_inst->r_b.value, 0x60);

    // Ensure half carry has been set
    assert_equal(this->cpu_inst->r_f.value, 0x20);
    

    // Test Carry
    // Setup memory and run instruction
    this->cpu_inst->r_f.value = 0x00;
    this->cpu_inst->r_b.value = 0xff;
    this->ram_inst->memory[0x0002] = 0x04;
    this->cpu_inst->tick();

    // Assert that registers were unchanged
    assert_equal(this->cpu_inst->r_b.value, 0x00);

    // Ensure flags haven't changed
    assert_equal(this->cpu_inst->r_f.value, 0xa0);
}





// CB TESTS
void TestRunner::test_cb_00()
{
    std::cout << "0x100";

    this->test_RLC(&this->cpu_inst->r_b, 0x00);
}
void TestRunner::test_cb_01()
{
    std::cout << "0x101";

    this->test_RLC(&this->cpu_inst->r_c, 0x01);
}
void TestRunner::test_cb_02()
{
    std::cout << "0x102";

    this->test_RLC(&this->cpu_inst->r_d, 0x02);
}
void TestRunner::test_cb_03()
{
    std::cout << "0x103";

    this->test_RLC(&this->cpu_inst->r_e, 0x03);
}

void TestRunner::test_cb_10()
{
    std::cout << "0x110";

    this->test_RL(&this->cpu_inst->r_b, 0x10);
}
void TestRunner::test_cb_11()
{
    std::cout << "0x111";

    this->test_RL(&this->cpu_inst->r_c, 0x11);
}
void TestRunner::test_cb_12()
{
    std::cout << "0x112";

    this->test_RL(&this->cpu_inst->r_d, 0x12);
}

void TestRunner::test_RLC(reg8 *reg, uint8_t op_code)
{
    // Test RLC
    this->cpu_inst->r_pc.value = 0x0000;
    this->cpu_inst->r_f.value = 0xf0;
    reg->value = 0xaf;
    this->ram_inst->memory[0x0000] = 0xcb;
    this->ram_inst->memory[0x0001] = op_code;
    this->cpu_inst->tick(); // Enable cb-mode
    this->cpu_inst->tick(); // Perform RLC

    // Assert value at b
    this->assert_equal(reg->value, 0x5f);

    // Ensure carry flag is set to 1 (the value that was moved
    // left
    this->assert_equal(this->cpu_inst->r_f.value, 0x10);

    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0002);
    
    // Test Moving 0 into carry flag
    reg->value = 0x02;
    this->ram_inst->memory[0x0002] = 0xcb;
    this->ram_inst->memory[0x0003] = op_code;
    this->cpu_inst->tick(); // Enable cb-mode
    this->cpu_inst->tick(); // Perform RLC

    this->assert_equal(reg->value, 0x04);

    // Ensure carry flag is set to 0 (the value that was moved
    // left
    this->assert_equal(this->cpu_inst->r_f.value, 0x00);

    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0004);
    
    // Test where outcome is 0
    reg->value = 0x80;
    this->ram_inst->memory[0x0004] = 0xcb;
    this->ram_inst->memory[0x0005] = op_code;
    this->cpu_inst->tick(); // Enable cb-mode
    this->cpu_inst->tick(); // Perform RLC

    this->assert_equal(reg->value, 0x01);

    // Ensure carry flag is set to 0 (the value that was moved
    // left
    this->assert_equal(this->cpu_inst->r_f.value, 0x10);

    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0006);
}

void TestRunner::test_RL(reg8 *reg, uint8_t op_code)
{
    // Test RL
    this->cpu_inst->r_pc.value = 0x0000;
    this->cpu_inst->r_f.value = 0xf0;
    reg->value = 0xad;
    this->ram_inst->memory[0x0000] = 0xcb;
    this->ram_inst->memory[0x0001] = op_code;
    this->cpu_inst->tick(); // Enable cb-mode
    this->cpu_inst->tick(); // Perform RLC

    // Assert value at b
    this->assert_equal(reg->value, 0x5b);

    // Ensure carry flag is set to 1 (the value that was moved
    // left
    this->assert_equal(this->cpu_inst->r_f.value, 0x10);

    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0002);

    // Test Moving 0 into carry flag
    this->ram_inst->memory[0x0002] = 0xcb;
    this->ram_inst->memory[0x0003] = op_code;
    this->cpu_inst->tick(); // Enable cb-mode
    this->cpu_inst->tick(); // Perform RL

    this->assert_equal(reg->value, 0xb7);

    // Ensure carry flag is set to 0 (the value that was moved
    // left
    this->assert_equal(this->cpu_inst->r_f.value, 0x00);

    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0004);
    
    // Test where outcome is 0
    reg->value = 0x80;
    this->ram_inst->memory[0x0004] = 0xcb;
    this->ram_inst->memory[0x0005] = op_code;
    this->cpu_inst->tick(); // Enable cb-mode
    this->cpu_inst->tick(); // Perform RL

    this->assert_equal(reg->value, 0x00);

    // Ensure carry flag is set to 0 (the value that was moved
    // left
    this->assert_equal(this->cpu_inst->r_f.value, 0x90);

    // Ensure that SP has moved on
    this->assert_equal(this->cpu_inst->r_pc.value, 0x0006);
}