#include "include/log.h"
#include "include/vector.h"

const char* k_file_ext = ".gcode";

int main(int argc, char* argv[])
{
    mvector* pt_a = mvector_from(float,
        0.0f, 1.0f, 2.0f,
        3.0f, 4.0f, 5.0f,
        6.0f, 7.0f, 8.0f);

    mvector_logff("Start:\n%s", pt_a, 3);

    mvector_transpose(pt_a, 3, 3);

    mvector_logff("Result:\n%s", pt_a, 3);

    mvector_destroy(pt_a);

    mvector_check_freed();
    return 0;
}
