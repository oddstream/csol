/* lil_settings.c */

#include <stdlib.h>
#include <stdio.h>

#include "lil.h"
#include "settings.h"

#define MAGIC (0x33dead33)
struct MagicHolder {
    unsigned magic;
};

void LoadSettings(int *windowWidth, int *windowHeight)
{
    lil_t lil = lil_new();

    const char tmpcode[] = "set __csol:settings:code__ [read {csol.settings.lil}]\nif [streq $__csol:settings:code__ ''] {print There is no code in the file or the file does not exist} {eval $__csol:settings:code__}\n";
    lil_value_t result = lil_parse(lil, tmpcode, 0, 1);
    lil_free_value(result); // result of lil_parse (value of last variable set, maybe) must be freed

    const char* err_msg;
    size_t pos;
    if (lil_error(lil, &err_msg, &pos)) {
        fprintf(stderr, "lil: error at %i: %s\n", (int)pos, err_msg);
    }

    lil_value_t val;

    val = lil_get_var(lil, "CardScale");
    if (!lil_empty_value(val)) {
        extern float cardScale;
        cardScale = (float)lil_to_double(val);
    }

    val = lil_get_var(lil, "WindowWidth");
    if (!lil_empty_value(val)) {
        *windowWidth = lil_to_integer(val);
    }

    val = lil_get_var(lil, "WindowHeight");
    if (!lil_empty_value(val)) {
        *windowHeight = lil_to_integer(val);
    }

    val = lil_get_var(lil, "DoesNotExist");
    if (lil_empty_value(val)) {
        fprintf(stdout, "DoesNotExist does not exist\n");
    } else {
        fprintf(stdout, "DoesNotExist exists\n");
    }

#if 1
    struct MagicHolder mh = (struct MagicHolder){.magic=MAGIC};
    struct MagicHolder *mh1 = &mh;

    val = lil_alloc_userdata(mh1);
    lil_set_var(lil, "Magic", val, 1);
    lil_free_value(val);    // you alloc it, you free it (except for function results)

    lil_value_t val2 = lil_get_var(lil, "Magic");
    if (!lil_empty_value(val2)) {
        struct MagicHolder *mh2 = lil_to_userdata(val2);
        fprintf(stdout, "Magic is %x\n", mh2->magic);
    }
#endif

    lil_free(lil);
}

void SaveSettings(void)
{

}
