#ifdef _MSC_VER
#   include <intrin.h>

#   define bit_AES  (1 << 25)
#   define bit_BMI2 (1 << 8)
#else
#   include <cpuid.h>
#endif

#include <string.h>


#include "Cpu.h"
#include "CpuImpl.h"


#define VENDOR_ID                  (0)
#define PROCESSOR_INFO             (1)
#define CACHE_TLB_DESCRIPTOR       (2)
#define EXTENDED_FEATURES          (7)
#define PROCESSOR_BRAND_STRING_1   (0x80000002)
#define PROCESSOR_BRAND_STRING_2   (0x80000003)
#define PROCESSOR_BRAND_STRING_3   (0x80000004)

#define EAX_Reg  (0)
#define EBX_Reg  (1)
#define ECX_Reg  (2)
#define EDX_Reg  (3)


#ifdef _MSC_VER
static inline void cpuid(int level, int output[4]) {
    __cpuid(output, level);
}
#else
static inline void cpuid(int level, int output[4]) {
    int a, b, c, d;
    __cpuid_count(level, 0, a, b, c, d);

    output[0] = a;
    output[1] = b;
    output[2] = c;
    output[3] = d;
}
#endif


static inline void cpu_brand_string(char* s) {
    int cpu_info[4] = { 0 };
    cpuid(VENDOR_ID, cpu_info);

    if (cpu_info[EAX_Reg] >= 4) {
        for (int i = 0; i < 4; i++) {
            cpuid(0x80000002 + i, cpu_info);
            memcpy(s, cpu_info, sizeof(cpu_info));
            s += 16;
        }
    }
}


static inline bool has_aes_ni()
{
    int cpu_info[4] = { 0 };
    cpuid(PROCESSOR_INFO, cpu_info);

    return cpu_info[ECX_Reg] & bit_AES;
}


static inline bool has_bmi2() {
    int cpu_info[4] = { 0 };
    cpuid(EXTENDED_FEATURES, cpu_info);

    return cpu_info[EBX_Reg] & bit_BMI2;
}


void CpuImpl::initCommon()
{
    cpu_brand_string(m_brand);

#   if defined(__x86_64__) || defined(_M_AMD64)
    m_flags |= Cpu::X86_64;
#   endif

    if (has_aes_ni()) {
        m_flags |= Cpu::AES;
    }

    if (has_bmi2()) {
        m_flags |= Cpu::BMI2;
    }
}
