/*
 * From https://github.com/clibs/cmocka/blob/master/example/simple_test.c
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "my_i2c.h"
#include "tempsensor.h"
#include "lightsensor.h"
#include "sensordata.h"

/*static void check_read_reg_temp(void)
{
    expect_value(__wrap_read_reg_temp, reg, (uint8_t)0x00);
}*/

/**
 * @brief mocked function for returning temp register read
 * 
 * @param reg 
 * @return int16_t 
 */
uint16_t __wrap_read_reg_temp(uint8_t reg)
{
    // allows the calling test to check if the supplied parameters are as expected
    //check_expected(reg);
   
    return (uint16_t)mock();
} 

/**
 * @brief Mocked function for returning light sensor read
 * 
 * @param reg 
 * @return uint16_t 
 */
uint16_t __wrap_read_reg_light_word(uint8_t reg)
{
    // allows the calling test to check if the supplied parameters are as expected
    //check_expected(reg);
   
    return (uint16_t)mock();
}

/**
 * @brief Testing negative sensor read,
 * values from TMP102 datasheet
 * 
 * @param state 
 */
static void test_negative_temperature(void **state)
{
    will_return(__wrap_read_reg_temp, (uint16_t)0xE700);

    assert_true(get_temp(0) == -25.0);
}

/**
 * @brief Testing positive sensor read,
 * values from TMP102 datasheet
 * 
 * @param state 
 */
static void test_positive_temperature(void **state)
{

    will_return(__wrap_read_reg_temp, (uint16_t)0x7FF0);

    assert_true(get_temp(0) == 127.9375);
}

/**
 * @brief Testing zero sensor reading,
 * values from TMP102 datasheet 
 * 
 * @param state 
 */
static void test_zero_temperature(void **state)
{

    will_return(__wrap_read_reg_temp, (uint16_t)0x00);
    
    assert_true( get_temp(0) == 0.0);
}

/**
 * @brief Testing light sensor read
 * values for fluroscent low gain
 * from APDS 9301 datasheet 
 * 
 * @param state 
 */
static void test_light_value(void **state)
{
    will_return(__wrap_read_reg_light_word, (uint16_t)0xabcd);
    will_return(__wrap_read_reg_light_word, (uint16_t)0x1234);
    
    float f;
    f=get_light();
    
    assert_true((int)f == (int)1219.308960);
}


//static void 

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_zero_temperature),
        cmocka_unit_test(test_negative_temperature),
        cmocka_unit_test(test_positive_temperature),
        cmocka_unit_test(test_light_value),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
