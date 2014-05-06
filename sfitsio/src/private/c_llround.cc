inline static long long c_llround( double val )
{
    if ( val < 0 ) return (long long)(val-0.5);
    else return (long long)(val+0.5);
}

