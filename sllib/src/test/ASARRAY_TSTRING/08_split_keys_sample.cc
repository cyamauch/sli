/**
  example code
  asarray_tstring�N���X:asarray_tstring &split_keys( const char *line, const char *delims, 
 	                			     bool zero_str, const char *quotations, int escape )
			asarray_tstring &split_keys( const char *line, const char *delims,
						     bool zero_str = false )
                        ������𕪊����L�[�ɐݒ肷��

  			asarray_tstring &split_values( const char *line, const char *delims, 
	                    			       bool zero_str, const char *quotations, int escape )
		    	asarray_tstring &split_values( const char *line, const char *delims,
						       bool zero_str = false )
                        ������𕪊����l�ɐݒ肷��

*/
#include <sli/stdstreamio.h>
#include <sli/asarray_tstring.h>
#include <sli/tarray_tstring.h>
#include <sli/ctindex.h>
#include <sli/digeststreamio.h>
#include <sli/tstring.h>

using namespace sli;

int main( int argc, char *argv[] )
{
    stdstreamio sio;

    const char *line = "'Camellia sasanqua' 'Chrysanthemum morifolium' 'Cyclamen persicum'";
    asarray_tstring my_arr;
    //line�̕�����𕪊�����my_arr�̃L�[�ɐݒ�
    my_arr.split_keys(line, " ", false, "'", 0);
    //�L�[�̐ݒ���m�F
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        const char *key = my_arr.key(i);
        sio.printf("%s ... [%s]\n", key, my_arr.cstr(key));
    }

    sio.printf("******************************************\n");

    const char *val = "'Camellia,pink','Chrysanthemum,yellow','Cyclamen,pink'";
    //val�̕�����𕪊�����my_arr�̒l�ɐݒ�
    my_arr.split_values(val, ",", false, "'", 0);
    //�L�[�ƒl�̐ݒ���m�F
    for ( size_t i=0 ; i < my_arr.length() ; i++ ) {
        const char *key = my_arr.key(i);
        sio.printf("%s ... [%s]\n", key, my_arr.cstr(key));
    }

    //�t�@�C������̓ǂݍ��񂾕������ݒ肷���
    stdstreamio fin;
    tstring line_buf;
    asarray_tstring academy_awards_asarr;

    //�t�@�C���̃I�[�v��
    if (fin.open("r","split_key_value.csv") < 0){
      //�G���[
    }

    //�t�@�C���̓ǂݍ���
    if ((line_buf = fin.getline()) == NULL){
      //�G���[
    }

    //�s���̉��s������ǂݍ��ނ悤�Ȃ̂ŁA�폜����
    line_buf.strtrim();
    sio.printf("line_buf ... [%s]\n", line_buf.cstr());
    //1�s�ڂ̕�����𕪊�����my_arr�̃L�[�ɐݒ�
    academy_awards_asarr.split_keys(line_buf.cstr(), ",", false);
    for ( size_t i=0 ; i < academy_awards_asarr.length() ; i++ ) {
        const char *key = academy_awards_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, academy_awards_asarr.cstr(key));
    }

    //�O�ׁ̈A������
    line_buf.init();
    //���̍s�̓ǂݍ���
    if ((line_buf = fin.getline()) == NULL){
      //�G���[
    }
    line_buf.strtrim();
    sio.printf("line_buf ... [%s]\n", line_buf.cstr());
    //2�s�ڂ̕�����𕪊�����my_arr�̒l�ɐݒ�
    academy_awards_asarr.split_values(line_buf.cstr(), ",", false);
    for ( size_t i=0 ; i < academy_awards_asarr.length() ; i++ ) {
        const char *key = academy_awards_asarr.key(i);
        sio.printf("%s ... [%s]\n", key, academy_awards_asarr.cstr(key));
    }

    return 0;
}

