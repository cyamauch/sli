
CCFLAGS  =

# DO NOT CHANGE
ALL_CCFLAGS = -O -Wall $(CCFLAGS)
IINC     = -I..
LLIB     = -L../..

.cc.o:   ; sh ../../s++.sh $(ALL_CCFLAGS) $(IINC) -c $*.cc

all::	00_constructor_sample 01_opeEqu_sample    02_opeEqu_sample \
	03_opePlus_sample     04_opeSubst_sample  05_opeMulti_sample \
	06_opeDivid_sample    07_opeComp_sample   071_opeNotComp_sample \
	08_size_type_sample   09_bytes_sample     10_dim_length_sample \
	11_length_sample      12_byte_length_sample 13_col_length_sample \
	14_row_length_sample  15_layer_length_sample 16_assign_default_sample \
	17_default_value_sample 18_init_sample    19_auto_resize_sample \
	20_rounding_sample    21_dvalue_sample    22_lvalue_sample \
	23_assign_sample      24_f_sample         25_f_cs_sample \
	26_put_sample         27_swap_sample      28_move_sample \
	29_cpy_sample         30_insert_sample    31_crop_sample \
	32_erase_sample       33_resize_sample    34_resizeby_sample \
	35_increase_dim_sample 36_decrease_dim_sample 37_swapObj_sample \
	38_ceil_sample        39_floor_sample     40_round_sample \
	41_trunc_sample       42_abs_sample       43_convert_sample \
	44_copy_sample        45_copy_sample      46_cut_sample \
	48_clean_sample       49_fill_sample \
	50_add_sample         51_multiply_sample  52_paste_sample \
	53_add_sample         54_subtract_sample  55_multiply_sample \
	56_divide_sample      57_compare_sample   58_register_extptr_sample \
	59_getdata_sample     60_putdata_sample   61_carray_sample \
	62_reverse_endian_sample 63_scan_cols_sample 64_flip_sample \
	65_reallocate_sample  66_trim_sample      67_flip_cols_sample \
	68_resize_sample      69_transpose_sample 70_rotate_sample \
	71_array_ptr_2d_sample 72_move_to_sample  73_math_sample \
	74_complex_sample     75_complex_cproj_sample 76_statistics_sample \
	77_pastef_sample      78_flipf_sample     79_transposef_sample \
	80_transpose_xyz2zxy_sample 81_transposef_xyz2zxy_sample \
	82_scan_f_sample      83_shallow_copy_sample

test::
	sh ./do_test.sh > test_result.txt
	diff -C 6 test_result_ok.txt test_result.txt

00_constructor_sample:		00_constructor_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

01_opeEqu_sample:		01_opeEqu_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

02_opeEqu_sample:		02_opeEqu_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

03_opePlus_sample:		03_opePlus_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

04_opeSubst_sample:		04_opeSubst_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

05_opeMulti_sample:		05_opeMulti_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

06_opeDivid_sample:		06_opeDivid_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

07_opeComp_sample:		07_opeComp_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

071_opeNotComp_sample:		071_opeNotComp_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

08_size_type_sample:		08_size_type_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

09_bytes_sample:		09_bytes_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

10_dim_length_sample:		10_dim_length_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

11_length_sample:		11_length_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

12_byte_length_sample:		12_byte_length_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

13_col_length_sample:		13_col_length_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

14_row_length_sample:		14_row_length_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

15_layer_length_sample:		15_layer_length_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

16_assign_default_sample:	16_assign_default_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

17_default_value_sample:	17_default_value_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

18_init_sample:			18_init_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

19_auto_resize_sample:		19_auto_resize_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

20_rounding_sample:		20_rounding_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

21_dvalue_sample:		21_dvalue_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

22_lvalue_sample:		22_lvalue_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

23_assign_sample:		23_assign_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

24_f_sample:			24_f_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

25_f_cs_sample:			25_f_cs_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

26_put_sample:			26_put_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

27_swap_sample:			27_swap_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

28_move_sample:			28_move_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

29_cpy_sample:			29_cpy_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

30_insert_sample:		30_insert_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

31_crop_sample:			31_crop_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

32_erase_sample:		32_erase_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

33_resize_sample:		33_resize_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

34_resizeby_sample:		34_resizeby_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

35_increase_dim_sample:		35_increase_dim_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

36_decrease_dim_sample:		36_decrease_dim_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

37_swapObj_sample:		37_swapObj_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

38_ceil_sample:			38_ceil_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

39_floor_sample:		39_floor_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

40_round_sample:		40_round_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

41_trunc_sample:		41_trunc_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

42_abs_sample:			42_abs_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

43_convert_sample:		43_convert_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

44_copy_sample:			44_copy_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

45_copy_sample:			45_copy_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

46_cut_sample:			46_cut_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

48_clean_sample:		48_clean_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

49_fill_sample:			49_fill_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

50_add_sample:			50_add_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

51_multiply_sample:		51_multiply_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

52_paste_sample:		52_paste_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

53_add_sample:			53_add_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

54_subtract_sample:		54_subtract_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

55_multiply_sample:		55_multiply_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

56_divide_sample:		56_divide_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

57_compare_sample:		57_compare_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

58_register_extptr_sample:	58_register_extptr_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

59_getdata_sample:		59_getdata_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

60_putdata_sample:		60_putdata_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

61_carray_sample:		61_carray_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

62_reverse_endian_sample:	62_reverse_endian_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

63_scan_cols_sample:	63_scan_cols_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

64_flip_sample:		64_flip_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

65_reallocate_sample:	65_reallocate_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

66_trim_sample:	66_trim_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

67_flip_cols_sample:	67_flip_cols_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

68_resize_sample:	68_resize_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

69_transpose_sample:	69_transpose_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

70_rotate_sample:	70_rotate_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

71_array_ptr_2d_sample:	71_array_ptr_2d_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

72_move_to_sample:	72_move_to_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

73_math_sample:		73_math_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

74_complex_sample:	74_complex_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

75_complex_cproj_sample:	75_complex_cproj_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

76_statistics_sample:		76_statistics_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

77_pastef_sample:		77_pastef_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

78_flipf_sample:		78_flipf_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

79_transposef_sample:		79_transposef_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

80_transpose_xyz2zxy_sample:		80_transpose_xyz2zxy_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

81_transposef_xyz2zxy_sample:		81_transposef_xyz2zxy_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

82_scan_f_sample:	82_scan_f_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

83_shallow_copy_sample:	83_shallow_copy_sample.o
	sh ../../s++.sh $(CPPC) $(LLIB) -o $@ $@.o ../../libsllib.a

clean::
	rm -f *.o *.exe \
	00_constructor_sample 01_opeEqu_sample    02_opeEqu_sample \
	03_opePlus_sample     04_opeSubst_sample  05_opeMulti_sample \
	06_opeDivid_sample    07_opeComp_sample   071_opeNotComp_sample \
	08_size_type_sample   09_bytes_sample     10_dim_length_sample \
	11_length_sample      12_byte_length_sample 13_col_length_sample \
	14_row_length_sample  15_layer_length_sample 16_assign_default_sample \
	17_default_value_sample 18_init_sample    19_auto_resize_sample \
	20_rounding_sample    21_dvalue_sample    22_lvalue_sample \
	23_assign_sample      24_f_sample         25_f_cs_sample \
	26_put_sample         27_swap_sample      28_move_sample \
	29_cpy_sample         30_insert_sample    31_crop_sample \
	32_erase_sample       33_resize_sample    34_resizeby_sample \
	35_increase_dim_sample 36_decrease_dim_sample 37_swapObj_sample \
	38_ceil_sample        39_floor_sample     40_round_sample \
	41_trunc_sample       42_abs_sample       43_convert_sample \
	44_copy_sample        45_copy_sample      46_cut_sample \
	48_clean_sample       49_fill_sample \
	50_add_sample         51_multiply_sample  52_paste_sample \
	53_add_sample         54_subtract_sample  55_multiply_sample \
	56_divide_sample      57_compare_sample   58_register_extptr_sample \
	59_getdata_sample     60_putdata_sample   61_carray_sample \
	62_reverse_endian_sample 63_scan_cols_sample 64_flip_sample \
	65_reallocate_sample  66_trim_sample      67_flip_cols_sample \
	68_resize_sample      69_transpose_sample 70_rotate_sample \
	71_array_ptr_2d_sample 72_move_to_sample  73_math_sample \
	74_complex_sample     75_complex_cproj_sample 76_statistics_sample \
	77_pastef_sample      78_flipf_sample     79_transposef_sample \
	80_transpose_xyz2zxy_sample 81_transposef_xyz2zxy_sample \
	82_scan_f_sample      83_shallow_copy_sample

