static void initialize_csum( fitsio_csum *ret_suminfo, unsigned long sum )
{
    ret_suminfo->cnt = 0;
    ret_suminfo->sum = sum;
    ret_suminfo->hi = (sum >> 16);
    ret_suminfo->lo = sum & 0xffff;
    return;
}
