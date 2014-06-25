
__kernel
void test(int mult, __global int* buf)
{
    int idx = get_global_id(0);
    buf[idx] = idx*mult;
}

