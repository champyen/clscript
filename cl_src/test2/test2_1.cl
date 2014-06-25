
int get_idx(void);

__kernel
void test(int mult, __global int* buf)
{
    buf[get_idx()] = get_idx()*mult;
}

