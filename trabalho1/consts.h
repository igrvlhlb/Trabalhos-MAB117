#include "expr.h"
#include <math.h>

#define MAX_NAME 32

struct name_val {
	char name[MAX_NAME];
	double value;
};

/* aqui definimos constantes uteis */
static struct name_val predef_vars[] = {
	{"e", M_E},
	{"pi", M_PI},
	{0, 0}
}; 


static void init_vars(struct expr_var_list *vars) {
	struct name_val atual;
	int i = 0;
	while ((atual = predef_vars[i]).name[0] != '\0') {
		struct expr_var *var_atual = expr_var(vars, atual.name, strlen(atual.name));
		var_atual->value = atual.value;
		i += 1;
	}
}
