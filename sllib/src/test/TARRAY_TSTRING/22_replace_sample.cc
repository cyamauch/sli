/**
  example code
  tarray_tstringƒNƒ‰ƒX:tarray_tstring &replace( size_t idx1, size_t n1,
						const char *el0, const char *el1, ... )
		       tarray_tstring &vreplace( size_t idx1, size_t n1,
                             			 const char *el0, const char *el1, va_list ap )
		       tarray_tstring &replace( size_t idx1, size_t n1,
                            			const char *const *elements )
		       tarray_tstring &replace( size_t idx1, size_t n1,
                            			const char *const *elements, size_t n2 )
    		       tarray_tstring &replace( size_t idx1, size_t n1,
                            			const tarray_tstring &src, size_t idx2 )
		       tarray_tstring &replace( size_t idx1, size_t n1,
                            			const tarray_tstring &src, size_t idx2, size_t n2 )
                       —v‘f‚Ì’uŠ·
*/
#include <sli/stdstreamio.h>
#include <sli/tarray_tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *tree[] = {"willow", "pine", "fir", NULL};
    const char *addTree[] = {"linden", "beech", "holly", NULL};
    tarray_tstring my_tree = tree;
    tarray_tstring my_addTree = addTree;

    //my_tree‚Ì‚P”Ô–Ú‚©‚ç1ŒÂ‚Ì—v‘f‚ðmy_addTree‚Ì1”Ô–Ú‚©‚ç2ŒÂ‚Ì—v‘f‚Å’uŠ·
    my_tree.replace(1, 1, my_addTree, 1, 2);
    for (size_t i = 0; i < my_tree.length(); i++){
        sio.printf("my_tree[%zu] = %s\n", i, my_tree.cstr(i));
    }

    my_tree.replace(10, 1, my_addTree, 1, 2);
    for (size_t i = 0; i < my_tree.length(); i++){
        sio.printf("my_tree[%zu] = %s\n", i, my_tree.cstr(i));
    }

    //old

    tarray_tstring tarr; 

    const char *hogeElement[] = {"HOGE", "hoge", "is", "Hoge",NULL};
    const char *ponyoElement[] = {"Ponyo", "ponyo", "poNyo", "sakana", "noko",NULL};

    const tarray_tstring hogeArr(hogeElement);
    const tarray_tstring ponyoArr(ponyoElement);
    tarr.assign(hogeArr);

    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

  
    //#4 &replace( size_t idx1, size_t n1, const char *const *elements, size_t n2 ),idx1”Ô–Ú‚©‚çn1ŒÂ‚Ì—v‘f‚ðelements n2ˆÈ~‚Æ’uŠ·‚·‚éB
    //n1ŒÂ < n2ŒÂ‚Ìê‡AƒTƒCƒYŠg’£
    tarr.replace(1,2,ponyoElement,2);
    sio.printf("tarr length = %zu\n", tarr.length());
    for (size_t i = 0; i < tarr.length(); i++){
        sio.printf("tarr[%zu] = %s\n",i, tarr.cstr(i));
    }

    return 0;


}

