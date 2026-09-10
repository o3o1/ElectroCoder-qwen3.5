import ctypes
import os

# 编译C代码
c_code = '''
#include <stdio.h>
#include <math.h>

#define DELTA_V_STEP 0.5f
#define VREF_INIT 15.0f

static float P_prev = 0.0f;
static float Vref_prev = VREF_INIT;
static int first_call = 1;

float mppt_perturb_and_observe(float V, float I, float *Vref) {
    float P_now = V * I;
    
    if (first_call) {
        P_prev = P_now;
        Vref_prev = *Vref;
        first_call = 0;
        return P_now;
    }
    
    float delta_P = P_now - P_prev;
    float delta_V = *Vref - Vref_prev;
    
    float Vref_new;
    
    if (delta_P > 0) {
        Vref_new = *Vref + (delta_V > 0 ? DELTA_V_STEP : -DELTA_V_STEP);
    } else if (delta_P < 0) {
        Vref_new = *Vref - (delta_V > 0 ? DELTA_V_STEP : -DELTA_V_STEP);
    } else {
        Vref_new = *Vref;
    }
    
    P_prev = P_now;
    Vref_prev = *Vref;
    *Vref = Vref_new;
    
    return P_now;
}

float pv_characteristic(float V) {
    return -0.1f * (V - 20.0f) * (V - 20.0f) + 40.0f;
}

int main() {
    float Vref = VREF_INIT;
    
    printf("MPPT Perturb and Observe Algorithm Test\\n");
    printf("=======================================\\n");
    
    for (int i = 0; i <= 20; i++) {
        float V = 10.0f + i * 1.0f;
        float P = pv_characteristic(V);
        float I = P / V;
        
        float P_calc = mppt_perturb_and_observe(V, I, &Vref);
        
        printf("V=%.1f, I=%.2f, P=%.2f, Vref=%.2f\\n", V, I, P_calc, Vref);
    }
    
    return 0;
}
'''

# 写入C文件
with open('./sandbox/task_mppt_po.c', 'w') as f:
    f.write(c_code)

# 尝试编译
print("尝试编译C代码...")
result = os.system('gcc -Wall -std=c99 ./sandbox/task_mppt_po.c -o ./sandbox/task_mppt_po.exe 2>&1')
if result != 0:
    print("gcc编译失败，尝试clang...")
    result = os.system('clang -Wall -std=c99 ./sandbox/task_mppt_po.c -o ./sandbox/task_mppt_po.exe 2>&1')
    if result != 0:
        print("clang编译失败，尝试cc...")
        result = os.system('cc -Wall -std=c99 ./sandbox/task_mppt_po.c -o ./sandbox/task_mppt_po.exe 2>&1')
        if result != 0:
            print("所有C编译器都不可用，使用Python模拟执行...")
            
            # Python模拟C代码逻辑
            DELTA_V_STEP = 0.5
            VREF_INIT = 15.0
            
            P_prev = 0.0
            Vref_prev = VREF_INIT
            first_call = True
            
            def mppt_perturb_and_observe(V, I, Vref):
                global P_prev, Vref_prev, first_call
                P_now = V * I
                
                if first_call:
                    P_prev = P_now
                    Vref_prev = Vref[0]
                    first_call = False
                    return P_now
                
                delta_P = P_now - P_prev
                delta_V = Vref[0] - Vref_prev
                
                if delta_P > 0:
                    Vref_new = Vref[0] + (DELTA_V_STEP if delta_V > 0 else -DELTA_V_STEP)
                elif delta_P < 0:
                    Vref_new = Vref[0] - (DELTA_V_STEP if delta_V > 0 else -DELTA_V_STEP)
                else:
                    Vref_new = Vref[0]
                
                P_prev = P_now
                Vref_prev = Vref[0]
                Vref[0] = Vref_new
                
                return P_now
            
            def pv_characteristic(V):
                return -0.1 * (V - 20.0) * (V - 20.0) + 40.0
            
            print("MPPT Perturb and Observe Algorithm Test")
            print("=======================================")
            
            Vref = [VREF_INIT]
            
            for i in range(21):
                V = 10.0 + i * 1.0
                P = pv_characteristic(V)
                I = P / V
                
                P_calc = mppt_perturb_and_observe(V, I, Vref)
                
                print(f"V={V:.1f}, I={I:.2f}, P={P_calc:.2f}, Vref={Vref[0]:.2f}")
        else:
            print("编译成功，运行程序...")
            os.system('./sandbox/task_mppt_po.exe')
    else:
        print("编译成功，运行程序...")
        os.system('./sandbox/task_mppt_po.exe')
else:
    print("编译成功，运行程序...")
    os.system('./sandbox/task_mppt_po.exe')