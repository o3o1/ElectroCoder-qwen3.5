#include <stdio.h>
#include <math.h>

#define EPSILON_DEFAULT 0.001

typedef enum {
    FUZZY_NEGATIVE = -1,
    FUZZY_ZERO = 0,
    FUZZY_POSITIVE = 1
} FuzzyValue;

typedef enum {
    ADJUST_NEGATIVE = -1,
    ADJUST_ZERO = 0,
    ADJUST_POSITIVE = 1
} AdjustValue;

FuzzyValue fuzzify(double value, double epsilon) {
    if (value > epsilon) {
        return FUZZY_POSITIVE;
    } else if (value < -epsilon) {
        return FUZZY_NEGATIVE;
    } else {
        return FUZZY_ZERO;
    }
}

int fuzzy_mppt_delta(double dP, double dV, double epsilon) {
    FuzzyValue fuzzy_dP = fuzzify(dP, epsilon);
    FuzzyValue fuzzy_dV = fuzzify(dV, epsilon);
    
    static const int rule_table[3][3] = {
        {ADJUST_POSITIVE, ADJUST_POSITIVE, ADJUST_ZERO},     // dP = N
        {ADJUST_POSITIVE, ADJUST_ZERO,     ADJUST_NEGATIVE}, // dP = Z
        {ADJUST_ZERO,     ADJUST_NEGATIVE, ADJUST_NEGATIVE}  // dP = P
    };
    
    int dP_index = fuzzy_dP + 1;
    int dV_index = fuzzy_dV + 1;
    
    return rule_table[dP_index][dV_index];
}

const char* fuzzy_value_to_str(FuzzyValue val) {
    switch(val) {
        case FUZZY_NEGATIVE: return "N";
        case FUZZY_ZERO: return "Z";
        case FUZZY_POSITIVE: return "P";
        default: return "Unknown";
    }
}

const char* adjust_value_to_str(int val) {
    switch(val) {
        case -1: return "N";
        case 0: return "Z";
        case 1: return "P";
        default: return "Unknown";
    }
}

const char* get_physical_meaning(double dP, double dV, int delta) {
    if (dP > EPSILON_DEFAULT && dV > EPSILON_DEFAULT) {
        return "Power increase and voltage increase, should decrease voltage reference";
    } else if (dP > EPSILON_DEFAULT && fabs(dV) <= EPSILON_DEFAULT) {
        return "Power increase and voltage stable, should decrease voltage reference";
    } else if (dP > EPSILON_DEFAULT && dV < -EPSILON_DEFAULT) {
        return "Power increase and voltage decrease, should decrease voltage reference";
    } else if (fabs(dP) <= EPSILON_DEFAULT && dV > EPSILON_DEFAULT) {
        return "Power stable and voltage increase, should increase voltage reference";
    } else if (fabs(dP) <= EPSILON_DEFAULT && fabs(dV) <= EPSILON_DEFAULT) {
        return "Power and voltage both stable, keep voltage reference";
    } else if (fabs(dP) <= EPSILON_DEFAULT && dV < -EPSILON_DEFAULT) {
        return "Power stable and voltage decrease, should decrease voltage reference";
    } else if (dP < -EPSILON_DEFAULT && dV > EPSILON_DEFAULT) {
        return "Power decrease and voltage increase, keep voltage reference";
    } else if (dP < -EPSILON_DEFAULT && fabs(dV) <= EPSILON_DEFAULT) {
        return "Power decrease and voltage stable, should increase voltage reference";
    } else if (dP < -EPSILON_DEFAULT && dV < -EPSILON_DEFAULT) {
        return "Power decrease and voltage decrease, should increase voltage reference";
    }
    return "Unknown situation";
}

int main() {
    double epsilon = EPSILON_DEFAULT;
    
    printf("Fuzzy MPPT Algorithm Test\n");
    printf("epsilon = %f\n\n", epsilon);
    
    double test_cases[][2] = {
        {0.005, 0.002},   // Power increase and voltage increase
        {-0.003, -0.001}, // Power decrease and voltage decrease
        {0.0005, -0.002}  // Power stable and voltage decrease
    };
    
    const char* descriptions[] = {
        "Test 1: Power increase and voltage increase",
        "Test 2: Power decrease and voltage decrease", 
        "Test 3: Power stable and voltage decrease"
    };
    
    for (int i = 0; i < 3; i++) {
        double dP = test_cases[i][0];
        double dV = test_cases[i][1];
        int delta = fuzzy_mppt_delta(dP, dV, epsilon);
        
        printf("%s\n", descriptions[i]);
        printf("dP=%.4f, dV=%.4f, epsilon=%.4f -> delta=%d (%s)\n", 
               dP, dV, epsilon, delta, adjust_value_to_str(delta));
        printf("Physical meaning: %s\n\n", get_physical_meaning(dP, dV, delta));
    }
    
    return 0;
}