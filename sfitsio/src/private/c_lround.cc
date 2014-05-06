inline static long c_lround( double val )
{
    if ( val < 0 ) return (long)(val-0.5);
    else return (long)(val+0.5);
}

