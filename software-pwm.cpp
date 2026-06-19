

#pragma once

#include <cstdint>
#include <new>

#include <memalloc.hpp>
#include <digioreg.hpp>
#include <time.hpp>


class SoftwarePWM {
    
public:

    volatile uint32_t* const pinadd;
    uint32_t perc;
    uint32_t count;
    uint32_t status = 0;
    DigIOReg* pwm_reg;
    Time* time_ins;
    

    static SoftwarePWM* create_PWM(volatile uint32_t* const pa, uint32_t pc, uint32_t c, uint32_t st) {
        if (!((pc <= 100))) {
            return nullptr;
        }

        SoftwarePWM* ptr = ::new (MemoryAllocator::alloc(sizeof(SoftwarePWM))) SoftwarePWM(pa, pc, c, st);

        uint32_t pin_num = (reinterpret_cast<uint32_t>(pa) - 0x60009004) / 4;
        uint32_t dynamic_mask = (1 << pin_num);

        DigIOReg* pwm_pin = DigIOReg::digioreg(
        GPIO_ENABLE_REG,
        DigIOReg::Mode::output,  
        pa,          
        GPIO_DATA_REG,           
        dynamic_mask,               
        MemoryAllocator::alloc(sizeof(DigIOReg)));

        Time* time_in = ::new (MemoryAllocator::alloc(sizeof(Time))) Time();

        ptr->pwm_reg = pwm_pin;
        ptr->time_ins = time_in;
        return ptr;

    
    }

    void on() {
        status = 1;

        while (status == 1) {


            uint32_t on_time = count*perc/100;
            uint32_t off_time = count - on_time;

            pwm_reg->SetHigh();

            time_ins->set_ref(0);
            while (!time_ins->is_delay_complete(on_time, 0)) {
                if (status == 0) {
                    return;
                }
            }

            pwm_reg->SetLow();
            time_ins->set_ref(0);
            while (!time_ins->is_delay_complete(off_time, 0)) {
                if (status == 0) {
                    return;
                }
            }

        }

        pwm_reg->SetLow();
    }
    

    void off() {
        status = 0;
    }
    
    void set_count(uint32_t c) {
        count = c;
    } 

    void set_perc(uint32_t p) {
        perc = p;
    } 
    

private:

    SoftwarePWM(volatile uint32_t* const pa, uint32_t pc, uint32_t c, uint32_t st) : pinadd(pa), perc(pc), count(c), status(st) { 
        
    }


};
