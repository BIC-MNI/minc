/*[-  HEADER FILE  -------------------------------------------------------------------------*/
/*
   Name:        ds_functions.h

   Description: The header file defines all functions declared by Data Set Library.

                To generate an actual function list delete the current list and copy the
                output of following command as statements in paragraph 'Sequence:
                define functions'.

                % grep "Name:" ds_*.c | grep "()" | awk '{print "   long " $3 ";"}' | sort


                To generate an actual modul list use the command:

                % ls -l ds_*.c | awk '{print "   " $8}'


                To generate an actual header file list use the command:

                % ls -l ds_*.h | awk '{print "   " $8}'


   Author:      THUMSER, Andreas (TH); Siemens AG UBMed CMS/SCE64; phone: 09131 844797
*/
/*]-----------------------------------------------------------------------------------------*/

/* NOTE: list of data set library source code moduls */
/* The first 15 characters must be unique. The unix "ar" utility works only with the first
   15 characters. */
/*
   ds_456789012345----0----5----0
   ds_adapt.c
   ds_build_acr.c
   ds_build_area.c
   ds_build_s1.c
   ds_build_s2.c
   ds_build_up.c
   ds_collect.c
   ds_convert.c
   ds_date.c
   ds_db.c
   ds_fill_acr.c
   ds_fill_area.c
   ds_fill_s1.c
   ds_fill_s2.c
   ds_fill_sha.c
   ds_format.c
   ds_get1.c
   ds_get2.c
   ds_head.c
   ds_info.c
   ds_interface.c
   ds_mani.c
   ds_mixed.c
   ds_num1.c
   ds_num1_xyz.c
   ds_num2.c
   ds_num2_xyz.c
   ds_parser.c
   ds_post.c
   ds_pre.c
   ds_separate.c
   ds_set2.c
   ds_set_s.c
   ds_som0_b1.c
   ds_som0_b2.c
   ds_som0_b3.c
   ds_som0_b4.c
   ds_som0_b5.c
   ds_som0_s.c
   ds_som1.c
   ds_split.c
   ds_string.c
   ds_text.c
   ds_top.c
   ds_transform.c
   ds_vector.c
   ds_xyz.c
   ds_456789012345----0----5----0
*/


/* NOTE: list of data set library header files */
/*
   ds_date.h
   ds_functions.h
   ds_head_acr_groups_types.h
   ds_head_basic_types.h
   ds_head_constants.h
   ds_head_image_text_type.h
   ds_head_numaris1.h
   ds_head_numaris2.h
   ds_head_shadow_groups_types.h
   ds_head_somaris0.h
   ds_head_somaris1.h
   ds_head_type.h
   ds_include_files.h
   ds_mani.h
   ds_messages.h
   ds_transformation.h
   ds_transformation_control.h
*/


#ifndef DS_FUNCTIONS
#define DS_FUNCTIONS

/* DECLARATION: define data set library functions */

  /* DECLARATION: define integer data set library functions */
  long ds_adapt_double();
  long ds_adapt_int();
  long ds_adapt_string();
  long ds_build_up_char_buf();
  long ds_build_up_cms_nema_data_set();
  long ds_build_up_data_groups();
  long ds_build_up_graphic_groups();
  long ds_build_up_group0008();
  long ds_build_up_group0009();
  long ds_build_up_group0010();
  long ds_build_up_group0011();
  long ds_build_up_group0013();
  long ds_build_up_group0018();
  long ds_build_up_group0019();
  long ds_build_up_group0020();
  long ds_build_up_group0021();
  long ds_build_up_group0028();
  long ds_build_up_group0029();
  long ds_build_up_group0051();
  long ds_build_up_group600x();
  long ds_build_up_group6021();
  long ds_build_up_group7FE0();
  long ds_build_up_group7FE1();
  long ds_build_up_header_groups();
  long ds_build_up_header_groups();
  long ds_build_up_image_text_item_value();
  long ds_build_up_key();
  long ds_build_up_long_buf();
  long ds_build_up_short_buf();
  long ds_build_up_som00008();
  long ds_build_up_som00009();
  long ds_build_up_som00010();
  long ds_build_up_som00011();
  long ds_build_up_som00018();
  long ds_build_up_som00020();
  long ds_build_up_som00021();
  long ds_build_up_som00028();
  long ds_build_up_som0600x();
  long ds_build_up_som07001();
  long ds_build_up_som07003();
  long ds_build_up_som07005();
  long ds_build_up_som0_blocked_data_set();
  long ds_build_up_som0_framed_data_set();
  long ds_collect_calculation_mode();
  long ds_collect_cardiac_code();
  long ds_collect_compression_code();
  long ds_collect_contrast();
  long ds_collect_data_object_subtype();
  long ds_collect_data_object_type();
  long ds_collect_data_set_subtype();
  long ds_collect_data_set_type();
  long ds_collect_date();
  long ds_collect_filter_type();
  long ds_collect_filter_type_image();
  long ds_collect_gate_phase();
  long ds_collect_ident();
  long ds_collect_image_format();
  long ds_collect_image_geometry_type();
  long ds_collect_image_location();
  long ds_collect_imaged_nucleus();
  long ds_collect_integer_number();
  long ds_collect_laterality();
  long ds_collect_measurement_mode();
  long ds_collect_modality();
  long ds_collect_order_of_slices();
  long ds_collect_patient_phase();
  long ds_collect_patient_position();
  long ds_collect_patient_region();
  long ds_collect_pixel_quality_mode();
  long ds_collect_procedure_description();
  long ds_collect_real_number();
  long ds_collect_rest_direction();
  long ds_collect_rotation_direction();
  long ds_collect_save_code();
  long ds_collect_sex();
  long ds_collect_storage_mode();
  long ds_collect_study_type();
  long ds_collect_time();
  long ds_collect_uid();
  long ds_collect_view_direction();
  long ds_collect_window_style();
  long ds_convert_double();
  long ds_convert_int();
  long ds_convert_string();
  long ds_date_check_cms();
  long ds_date_cms_to_display();
  long ds_date_cms_to_ingres();
  long ds_date_cms_to_nema();
  long ds_date_cms_to_rt11();
  long ds_date_cms_to_spi();
  long ds_date_days_of_month();
  long ds_date_days_of_year();
  long ds_date_display_to_cms();
  long ds_date_get();
  long ds_date_get_age();
  long ds_date_get_new();
  long ds_date_ingres_to_cms();
  long ds_date_nema_to_cms();
  long ds_date_pdp_to_cms();
  long ds_date_string_to_cms();
  long ds_date_vax_to_cms();
  long ds_date_vms_to_cms();
  long ds_db_set_comdiainfo();
  long ds_db_set_imainfo();
  long ds_db_set_patinfo();
  long ds_db_set_stuinfo();
  long ds_db_use_comdiainfo();
  long ds_db_use_patinfo();
  long ds_file_control();
  long ds_fill_basic_structs();
  long ds_fill_binary_data();
  long ds_fill_binary_g7FE0();
  long ds_fill_binary_g7FE1();
  long ds_fill_common_black_image_data();
  long ds_fill_data_structs();
  long ds_fill_default_data();
  long ds_fill_fix_data();
  long ds_fill_graphic_structs();
  long ds_fill_image_g7FE0();
  long ds_fill_image_graphic();
  long ds_fill_image_text();
  long ds_fill_patient_data();
  long ds_fill_struct_g08();
  long ds_fill_struct_g09();
  long ds_fill_struct_g09Cms();
  long ds_fill_struct_g09Lab();
  long ds_fill_struct_g09Spi();
  long ds_fill_struct_g10();
  long ds_fill_struct_g11();
  long ds_fill_struct_g11Cms();
  long ds_fill_struct_g11Spi();
  long ds_fill_struct_g13();
  long ds_fill_struct_g13Cms();
  long ds_fill_struct_g18();
  long ds_fill_struct_g19();
  long ds_fill_struct_g19Acqu();
  long ds_fill_struct_g19Cms();
  long ds_fill_struct_g19Ct();
  long ds_fill_struct_g19CtCoAd();
  long ds_fill_struct_g19Mr();
  long ds_fill_struct_g19MrCoAd();
  long ds_fill_struct_g20();
  long ds_fill_struct_g21();
  long ds_fill_struct_g21Cms();
  long ds_fill_struct_g21Ct();
  long ds_fill_struct_g21CtRaw();
  long ds_fill_struct_g21Med();
  long ds_fill_struct_g21Mr();
  long ds_fill_struct_g21MrRaw();
  long ds_fill_struct_g28();
  long ds_fill_struct_g29();
  long ds_fill_struct_g29Cms();
  long ds_fill_struct_g29Spi();
  long ds_fill_struct_g51();
  long ds_fill_struct_g6021();
  long ds_fill_test_data();
  long ds_finally_interface_status_update();
  long ds_generate_image_text();
  long ds_get_data_set_owner();
  long ds_get_data_set_type();
  long ds_get_g09_med_parameter();
  long ds_get_subgroup_owner_code();
  long ds_get_swap_mode();
  long ds_get_vax_char_buf();
  long ds_get_vax_integer2_buf();
  long ds_get_vax_integer4_buf();
  long ds_get_vax_real_buf();
  long ds_get_vax_skip_gap();
  long ds_hea_slice_1_2_single();
  long ds_info_get_xxx_version();
  long ds_info_show_versions();
  long ds_init_error_handling();
  long ds_init_header();
  long ds_initialize_nema_parser();
  long ds_mani_gen_rot_matrix();
  long ds_mani_image_magnify();
  long ds_mani_image_mirror();
  long ds_mani_image_rotate_90();
  long ds_mani_mult_matrix_matrix();
  long ds_mani_mult_matrix_scalar();
  long ds_mani_patient_modify();
  long ds_mani_set_id_matrix();
  long ds_mani_transform_vector();
  long ds_mes_calc_prs();
  long ds_modify_patient_data();
  long ds_nema_check_if_transformation_possible();
  long ds_num1_blocked_fill_basic_structs();
  long ds_num1_blocked_fill_data_structs();
  long ds_num1_framed_fill_basic_structs();
  long ds_num1_framed_fill_data_structs();
  long ds_num1_get_cms_sep_basic_data();
  long ds_num1_get_pixel_quality_code();
  long ds_num1_get_pixel_quality_mode();
  long ds_num1_num1_to_cms();
  long ds_num1_vax_to_sun();
  long ds_num2_fill_basic_structs();
  long ds_num2_fill_data_structs();
  long ds_num2_get_cms_sep_basic_data();
  long ds_num2_get_pixel_quality_code();
  long ds_num2_get_pixel_quality_mode();
  long ds_num2_num2_to_cms();
  long ds_num2_vax_to_sun();
  long ds_parser_error_handling();
  long ds_post_build_up_name();
  long ds_post_build_up_record();
  long ds_post_separate();
  long ds_pre_allocate_data_areas_b1();
  long ds_pre_allocate_data_areas_b2();
  long ds_pre_allocate_data_areas_s();
  long ds_pre_build_up_name();
  long ds_pre_build_up_record();
  long ds_pre_determine_control_status_b1();
  long ds_pre_determine_control_status_b2();
  long ds_pre_determine_control_status_s();
  long ds_pre_determine_data_area_lengths_b1();
  long ds_pre_determine_data_area_lengths_b2();
  long ds_pre_determine_data_area_lengths_s();
  long ds_pre_initialize();
  long ds_pre_separation();
  long ds_reset_nema_parser();
  long ds_return_control();
  long ds_separate_byte_buf();
  long ds_separate_char_buf();
  long ds_separate_cms_blocked_data_set();
  long ds_separate_common_data_set();
  long ds_separate_given_item();
  long ds_separate_group_600x();
  long ds_separate_key();
  long ds_separate_long_buf();
  long ds_separate_nema_data_set();
  long ds_separate_num1_blocked_data_set();
  long ds_separate_num1_framed_data_set();
  long ds_separate_num2_data_set();
  long ds_separate_short_buf();
  long ds_separate_som0_blocked_data_set();
  long ds_separate_som0_framed_data_set();
  long ds_separate_som_blocked_data_set();
  long ds_separate_som_framed_data_set();
  long ds_set_pdp_byte_buf();
  long ds_set_pdp_char_buf();
  long ds_set_pdp_fill_gap();
  long ds_set_pdp_integer2_buf();
  long ds_set_pdp_integer4_buf();
  long ds_set_struct_g09Cms();
  long ds_set_struct_g09Lab();
  long ds_set_struct_g09Spi();
  long ds_set_struct_g11Cms();
  long ds_set_struct_g11Spi();
  long ds_set_struct_g19Acqu();
  long ds_set_struct_g19Cms();
  long ds_set_struct_g19Ct();
  long ds_set_struct_g19CtCoAd();
  long ds_set_struct_g19Mr();
  long ds_set_struct_g19MrCoAd();
  long ds_set_struct_g21Cms();
  long ds_set_struct_g21Ct();
  long ds_set_struct_g21CtRaw();
  long ds_set_struct_g21Med();
  long ds_set_struct_g21Mr();
  long ds_set_struct_g21MrRaw();
  long ds_set_struct_g29Cms();
  long ds_set_struct_g29Spi();
  long ds_show_help_text();
  long ds_simulate_ct_mr_status_file();
  long ds_som0_blocked_build_up_basic_structs();
  long ds_som0_blocked_build_up_data_structs();
  long ds_som0_blocked_fill_basic_structs();
  long ds_som0_blocked_fill_data_structs();
  long ds_som0_check_if_transformation_possible();
  long ds_som0_cms_to_som0();
  long ds_som0_fill_bild_text();
  long ds_som0_fill_block_0();
  long ds_som0_framed_build_up_basic_groups();
  long ds_som0_framed_build_up_data_groups();
  long ds_som0_framed_fill_basic_structs();
  long ds_som0_framed_fill_data_structs();
  long ds_som0_get_cms_sep_basic_data();
  long ds_som0_get_patient_orientation_vector();
  long ds_som0_pdp_to_sun();
  long ds_som0_print_build_up_protocol();
  long ds_som0_som0_to_cms();
  long ds_som0_sun_to_pdp();
  long ds_som_blocked_fill_basic_structs();
  long ds_som_blocked_fill_data_structs();
  long ds_som_framed_fill_basic_structs();
  long ds_som_framed_fill_data_structs();
  long ds_som_get_cms_sep_basic_data();
  long ds_som_get_patient_position();
  long ds_som_get_rest_direction();
  long ds_som_som_to_cms();
  long ds_som_vax_to_sun();
  long ds_split_calculation_mode();
  long ds_split_cardiac_code();
  long ds_split_compression_code();
  long ds_split_contrast();
  long ds_split_data_object_subtype();
  long ds_split_data_set_subtype();
  long ds_split_filter_type();
  long ds_split_filter_type_image();
  long ds_split_gate_phase();
  long ds_split_ident();
  long ds_split_image_format();
  long ds_split_image_geometry_type();
  long ds_split_imaged_nucleus();
  long ds_split_integer_number();
  long ds_split_laterality();
  long ds_split_measurement_mode();
  long ds_split_modality();
  long ds_split_order_of_slices();
  long ds_split_patient_phase();
  long ds_split_patient_position();
  long ds_split_patient_region();
  long ds_split_pixel_quality_mode();
  long ds_split_procedure_description();
  long ds_split_real_number();
  long ds_split_rest_direction();
  long ds_split_rotation_direction();
  long ds_split_save_code();
  long ds_split_sex();
  long ds_split_storage_mode();
  long ds_split_study_type();
  long ds_split_view_direction();
  long ds_split_window_style();
  long ds_stop_dbx();
  long ds_string_delete_leading_characters();
  long ds_string_delete_multiple_characters();
  long ds_string_delete_tailing_characters();
  long ds_string_find_string_in_string();
  long ds_string_reformate();
  long ds_string_replace();
  long ds_text_comment_no_1();
  long ds_text_comment_no_2();
  long ds_text_contrast();
  long ds_text_data_set_id();
  long ds_text_date_of_measurement();
  long ds_text_echo_time();
  long ds_text_field_of_view();
  long ds_text_gantry_tilt();
  long ds_text_gating_and_trigger();
  long ds_text_get_black_image_text();
  long ds_text_get_none_image_text();
  long ds_text_get_normal_image_text();
  long ds_text_get_recon_3d_image_text();
  long ds_text_image_number();
  long ds_text_installation_name();
  long ds_text_label();
  long ds_text_magnification_factor();
  long ds_text_manufacturer_model();
  long ds_text_matrix();
  long ds_text_mip_column();
  long ds_text_mip_head_line();
  long ds_text_mip_line();
  long ds_text_mip_slice();
  long ds_text_number_of_acquisitions();
  long ds_text_patient_birthdate();
  long ds_text_patient_name();
  long ds_text_patient_number();
  long ds_text_patient_orientation_set_1();
  long ds_text_patient_orientation_set_2();
  long ds_text_patient_orientation_sets();
  long ds_text_patient_orientation_string();
  long ds_text_patient_position();
  long ds_text_patient_sex_and_age();
  long ds_text_repetition_time();
  long ds_text_saturation_regions();
  long ds_text_scan_number();
  long ds_text_sequence_information();
  long ds_text_slice_orientation_no();
  long ds_text_slice_position();
  long ds_text_slice_thickness();
  long ds_text_software_version();
  long ds_text_study_number();
  long ds_text_table_position();
  long ds_text_time_of_acquisition();
  long ds_text_time_of_measurement();
  long ds_text_time_of_scanning();
  long ds_text_tube_current();
  long ds_text_tube_voltage();
  long ds_text_type_of_measurement();
  long ds_text_zoom_center();
  long ds_time_check_cms();
  long ds_time_cms_to_display();
  long ds_time_cms_to_nema();
  long ds_time_cms_to_pdp();
  long ds_time_cms_to_spi();
  long ds_time_display_to_cms();
  long ds_time_nema_to_cms();
  long ds_time_pdp_to_cms();
  long ds_time_string_to_cms();
  long ds_top_up_header();
  long ds_trace_control();
  long ds_transform_cms_to_blocked_som0();
  long ds_transform_cms_to_cms_nema();
  long ds_transform_cms_to_framed_som0();
  long ds_transform_common_to_cms();
  long ds_update_nema_parser();
  long ds_vector_add();
  long ds_vector_assign();
  long ds_vector_check_if_equal();
  long ds_vector_check_if_nil();
  long ds_vector_check_if_normalized();
  long ds_vector_check_if_undefined();
  long ds_vector_cut();
  long ds_vector_get();
  long ds_vector_inner_product();
  long ds_vector_multiply();
  long ds_vector_orthogonal_to_polar();
  long ds_vector_polar_to_orthogonal();
  long ds_vector_print();
  long ds_vector_put();
  long ds_vector_set();
  long ds_vector_subtract();
  long ds_vector_transform_l();
  long ds_vector_transform_m();
  long ds_vector_vector_product();
  long ds_xyz_get_cms_black_basic_data();
  long ds_xyz_get_cms_derived_data();
  long ds_xyz_get_cms_field_of_view();
  long ds_xyz_get_cms_sep_basic_data();
  long ds_xyz_get_image_class();
  long ds_xyz_get_nema_image_place();
  long ds_xyz_get_slice_orientation_parameters();
  long ds_xyz_get_transformation_list1();
  long ds_xyz_get_transformation_list2();
  long ds_xyz_get_view_direction();
  long ds_xyz_shift_definition_point();


  /* DECLARATION: define special data set library functions */
  data_area_type_t ds_get_data_area_type();
  item_quality_t ds_get_item_quality();
  image_text_type_t ds_get_image_text_type();
  double ds_get_max_abs_vector_component();
  char *ds_info_get_header_version();
  char *ds_info_get_nema_version();
  char *ds_info_get_software_version();
  char *ds_info_get_spi_version();
  char *ds_strstr();
  long unsigned ds_cal_image_base_vectors();
  long unsigned ds_cal_vec_mode();
  long unsigned ds_mpr_mult_matmat3();
  
#define ds_string_find_string_in_string ds_strstr


  /* DECLARATION: define special system functions */
  char *malloc();

#endif
