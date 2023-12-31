declare unique index pg_aggregate_fnoid_index 2650 on pg_aggregate using btree(aggfnoid oid_ops)
declare unique index pg_am_name_index 2651 on pg_am using btree(amname name_ops)
declare unique index pg_am_oid_index 2652 on pg_am using btree(oid oid_ops)
declare unique index pg_amop_fam_strat_index 2653 on pg_amop using btree(amopfamily oid_ops, amoplefttype oid_ops, amoprighttype oid_ops, amopstrategy int2_ops)
declare unique index pg_amop_opr_fam_index 2654 on pg_amop using btree(amopopr oid_ops, amoppurpose char_ops, amopfamily oid_ops)
declare unique index pg_amop_oid_index 2756 on pg_amop using btree(oid oid_ops)
declare unique index pg_amproc_fam_proc_index 2655 on pg_amproc using btree(amprocfamily oid_ops, amproclefttype oid_ops, amprocrighttype oid_ops, amprocnum int2_ops)
declare unique index pg_amproc_oid_index 2757 on pg_amproc using btree(oid oid_ops)
declare unique index pg_attrdef_adrelid_adnum_index 2656 on pg_attrdef using btree(adrelid oid_ops, adnum int2_ops)
declare unique index pg_attrdef_oid_index 2657 on pg_attrdef using btree(oid oid_ops)
declare unique index pg_attribute_relid_attnam_index 2658 on pg_attribute using btree(attrelid oid_ops, attname name_ops)
declare unique index pg_attribute_relid_attnum_index 2659 on pg_attribute using btree(attrelid oid_ops, attnum int2_ops)
declare unique index pg_authid_rolname_index 2676 on pg_authid using btree(rolname name_ops)
declare unique index pg_authid_oid_index 2677 on pg_authid using btree(oid oid_ops)
declare index pg_authid_rolresqueue_index 6029 on pg_authid using btree(rolresqueue oid_ops)
declare index pg_authid_rolresgroup_index 6440 on pg_authid using btree(rolresgroup oid_ops)
declare unique index pg_auth_members_role_member_index 2694 on pg_auth_members using btree(roleid oid_ops, member oid_ops)
declare unique index pg_auth_members_member_role_index 2695 on pg_auth_members using btree(member oid_ops, roleid oid_ops)
declare index pg_auth_time_constraint_authid_index 6449 on pg_auth_time_constraint using btree(authid oid_ops)
declare unique index pg_cast_oid_index 2660 on pg_cast using btree(oid oid_ops)
declare unique index pg_cast_source_target_index 2661 on pg_cast using btree(castsource oid_ops, casttarget oid_ops)
declare unique index pg_class_oid_index 2662 on pg_class using btree(oid oid_ops)
declare unique index pg_class_relname_nsp_index 2663 on pg_class using btree(relname name_ops, relnamespace oid_ops)
declare index pg_class_tblspc_relfilenode_index 3455 on pg_class using btree(reltablespace oid_ops, relfilenode oid_ops)
declare unique index pg_collation_name_enc_nsp_index 3164 on pg_collation using btree(collname name_ops, collencoding int4_ops, collnamespace oid_ops)
declare unique index pg_collation_oid_index 3085 on pg_collation using btree(oid oid_ops)
declare index pg_constraint_conname_nsp_index 2664 on pg_constraint using btree(conname name_ops, connamespace oid_ops)
declare unique index pg_constraint_conrelid_contypid_conname_index 2665 on pg_constraint using btree(conrelid oid_ops, contypid oid_ops, conname name_ops)
declare index pg_constraint_contypid_index 2666 on pg_constraint using btree(contypid oid_ops)
declare unique index pg_constraint_oid_index 2667 on pg_constraint using btree(oid oid_ops)
declare index pg_constraint_conparentid_index 2579 on pg_constraint using btree(conparentid oid_ops)
declare unique index pg_conversion_default_index 2668 on pg_conversion using btree(connamespace oid_ops, conforencoding int4_ops, contoencoding int4_ops, oid oid_ops)
declare unique index pg_conversion_name_nsp_index 2669 on pg_conversion using btree(conname name_ops, connamespace oid_ops)
declare unique index pg_conversion_oid_index 2670 on pg_conversion using btree(oid oid_ops)
declare unique index pg_database_datname_index 2671 on pg_database using btree(datname name_ops)
declare unique index pg_database_oid_index 2672 on pg_database using btree(oid oid_ops)
declare index pg_depend_depender_index 2673 on pg_depend using btree(classid oid_ops, objid oid_ops, objsubid int4_ops)
declare index pg_depend_reference_index 2674 on pg_depend using btree(refclassid oid_ops, refobjid oid_ops, refobjsubid int4_ops)
declare unique index pg_description_o_c_o_index 2675 on pg_description using btree(objoid oid_ops, classoid oid_ops, objsubid int4_ops)
declare unique index pg_shdescription_o_c_index 2397 on pg_shdescription using btree(objoid oid_ops, classoid oid_ops)
declare unique index pg_enum_oid_index 3502 on pg_enum using btree(oid oid_ops)
declare unique index pg_enum_typid_label_index 3503 on pg_enum using btree(enumtypid oid_ops, enumlabel name_ops)
declare unique index pg_enum_typid_sortorder_index 3534 on pg_enum using btree(enumtypid oid_ops, enumsortorder float4_ops)
declare index pg_index_indrelid_index 2678 on pg_index using btree(indrelid oid_ops)
declare unique index pg_index_indexrelid_index 2679 on pg_index using btree(indexrelid oid_ops)
declare unique index pg_inherits_relid_seqno_index 2680 on pg_inherits using btree(inhrelid oid_ops, inhseqno int4_ops)
declare index pg_inherits_parent_index 2187 on pg_inherits using btree(inhparent oid_ops)
declare unique index pg_init_privs_o_c_o_index 3395 on pg_init_privs using btree(objoid oid_ops, classoid oid_ops, objsubid int4_ops)
declare unique index pg_language_lanname_index 2681 on pg_language using btree(lanname name_ops)
declare unique index pg_language_oid_index 2682 on pg_language using btree(oid oid_ops)
declare unique index pg_largeobject_loid_pn_index 2683 on pg_largeobject using btree(loid oid_ops, pageno int4_ops)
declare unique index pg_largeobject_metadata_oid_index 2996 on pg_largeobject_metadata using btree(oid oid_ops)
declare unique index pg_namespace_nspname_index 2684 on pg_namespace using btree(nspname name_ops)
declare unique index pg_namespace_oid_index 2685 on pg_namespace using btree(oid oid_ops)
declare unique index pg_opclass_am_name_nsp_index 2686 on pg_opclass using btree(opcmethod oid_ops, opcname name_ops, opcnamespace oid_ops)
declare unique index pg_opclass_oid_index 2687 on pg_opclass using btree(oid oid_ops)
declare unique index pg_operator_oid_index 2688 on pg_operator using btree(oid oid_ops)
declare unique index pg_operator_oprname_l_r_n_index 2689 on pg_operator using btree(oprname name_ops, oprleft oid_ops, oprright oid_ops, oprnamespace oid_ops)
declare unique index pg_opfamily_am_name_nsp_index 2754 on pg_opfamily using btree(opfmethod oid_ops, opfname name_ops, opfnamespace oid_ops)
declare unique index pg_opfamily_oid_index 2755 on pg_opfamily using btree(oid oid_ops)
declare unique index pg_pltemplate_name_index 1137 on pg_pltemplate using btree(tmplname name_ops)
declare unique index pg_proc_oid_index 2690 on pg_proc using btree(oid oid_ops)
declare unique index pg_proc_proname_args_nsp_index 2691 on pg_proc using btree(proname name_ops, proargtypes oidvector_ops, pronamespace oid_ops)
declare unique index pg_rewrite_oid_index 2692 on pg_rewrite using btree(oid oid_ops)
declare unique index pg_rewrite_rel_rulename_index 2693 on pg_rewrite using btree(ev_class oid_ops, rulename name_ops)
declare unique index pg_sequence_seqrelid_index 5002 on pg_sequence using btree(seqrelid oid_ops)
declare index pg_shdepend_depender_index 1232 on pg_shdepend using btree(dbid oid_ops, classid oid_ops, objid oid_ops, objsubid int4_ops)
declare index pg_shdepend_reference_index 1233 on pg_shdepend using btree(refclassid oid_ops, refobjid oid_ops)
declare unique index pg_statistic_relid_att_inh_index 2696 on pg_statistic using btree(starelid oid_ops, staattnum int2_ops, stainherit bool_ops)
declare unique index pg_statistic_ext_oid_index 3380 on pg_statistic_ext using btree(oid oid_ops)
declare unique index pg_statistic_ext_name_index 3997 on pg_statistic_ext using btree(stxname name_ops, stxnamespace oid_ops)
declare index pg_statistic_ext_relid_index 3379 on pg_statistic_ext using btree(stxrelid oid_ops)
declare unique index pg_statistic_ext_data_stxoid_index 3433 on pg_statistic_ext_data using btree(stxoid oid_ops)
declare unique index pg_tablespace_oid_index 2697 on pg_tablespace using btree(oid oid_ops)
declare unique index pg_tablespace_spcname_index 2698 on pg_tablespace using btree(spcname name_ops)
declare unique index pg_transform_oid_index 3574 on pg_transform using btree(oid oid_ops)
declare unique index pg_transform_type_lang_index 3575 on pg_transform using btree(trftype oid_ops, trflang oid_ops)
declare index pg_trigger_tgconstraint_index 2699 on pg_trigger using btree(tgconstraint oid_ops)
declare unique index pg_trigger_tgrelid_tgname_index 2701 on pg_trigger using btree(tgrelid oid_ops, tgname name_ops)
declare unique index pg_trigger_oid_index 2702 on pg_trigger using btree(oid oid_ops)
declare unique index pg_event_trigger_evtname_index 3467 on pg_event_trigger using btree(evtname name_ops)
declare unique index pg_event_trigger_oid_index 3468 on pg_event_trigger using btree(oid oid_ops)
declare unique index pg_ts_config_cfgname_index 3608 on pg_ts_config using btree(cfgname name_ops, cfgnamespace oid_ops)
declare unique index pg_ts_config_oid_index 3712 on pg_ts_config using btree(oid oid_ops)
declare unique index pg_ts_config_map_index 3609 on pg_ts_config_map using btree(mapcfg oid_ops, maptokentype int4_ops, mapseqno int4_ops)
declare unique index pg_ts_dict_dictname_index 3604 on pg_ts_dict using btree(dictname name_ops, dictnamespace oid_ops)
declare unique index pg_ts_dict_oid_index 3605 on pg_ts_dict using btree(oid oid_ops)
declare unique index pg_ts_parser_prsname_index 3606 on pg_ts_parser using btree(prsname name_ops, prsnamespace oid_ops)
declare unique index pg_ts_parser_oid_index 3607 on pg_ts_parser using btree(oid oid_ops)
declare unique index pg_ts_template_tmplname_index 3766 on pg_ts_template using btree(tmplname name_ops, tmplnamespace oid_ops)
declare unique index pg_ts_template_oid_index 3767 on pg_ts_template using btree(oid oid_ops)
declare unique index pg_type_oid_index 2703 on pg_type using btree(oid oid_ops)
declare unique index pg_type_typname_nsp_index 2704 on pg_type using btree(typname name_ops, typnamespace oid_ops)
declare unique index pg_foreign_data_wrapper_oid_index 112 on pg_foreign_data_wrapper using btree(oid oid_ops)
declare unique index pg_foreign_data_wrapper_name_index 548 on pg_foreign_data_wrapper using btree(fdwname name_ops)
declare unique index pg_foreign_server_oid_index 113 on pg_foreign_server using btree(oid oid_ops)
declare unique index pg_foreign_server_name_index 549 on pg_foreign_server using btree(srvname name_ops)
declare unique index pg_user_mapping_oid_index 174 on pg_user_mapping using btree(oid oid_ops)
declare unique index pg_user_mapping_user_server_index 175 on pg_user_mapping using btree(umuser oid_ops, umserver oid_ops)
declare unique index pg_foreign_table_relid_index 3119 on pg_foreign_table using btree(ftrelid oid_ops)
declare unique index pg_default_acl_role_nsp_obj_index 827 on pg_default_acl using btree(defaclrole oid_ops, defaclnamespace oid_ops, defaclobjtype char_ops)
declare unique index pg_default_acl_oid_index 828 on pg_default_acl using btree(oid oid_ops)
declare unique index pg_db_role_setting_databaseid_rol_index 2965 on pg_db_role_setting using btree(setdatabase oid_ops, setrole oid_ops)
declare unique index pg_seclabel_object_index 3597 on pg_seclabel using btree(objoid oid_ops, classoid oid_ops, objsubid int4_ops, provider text_ops)
declare unique index pg_shseclabel_object_index 3593 on pg_shseclabel using btree(objoid oid_ops, classoid oid_ops, provider text_ops)
declare unique index pg_extension_oid_index 3080 on pg_extension using btree(oid oid_ops)
declare unique index pg_extension_name_index 3081 on pg_extension using btree(extname name_ops)
declare unique index gp_distribution_policy_localoid_index 6103 on gp_distribution_policy using btree(localoid oid_ops)
declare unique index pg_appendonly_relid_index 7141 on pg_appendonly using btree(relid oid_ops)
declare unique index gp_fastsequence_objid_objmod_index 6067 on gp_fastsequence using btree(objid oid_ops, objmod int8_ops)
declare unique index pg_statlastop_classid_objid_staactionname_index 6054 on pg_stat_last_operation using btree(classid oid_ops, objid oid_ops, staactionname name_ops)
declare index pg_statlastshop_classid_objid_index 6057 on pg_stat_last_shoperation using btree(classid oid_ops, objid oid_ops)
declare unique index pg_statlastshop_classid_objid_staactionname_index 6058 on pg_stat_last_shoperation using btree(classid oid_ops, objid oid_ops, staactionname name_ops)
declare unique index pg_resqueue_oid_index 6027 on pg_resqueue using btree(oid oid_ops)
declare unique index pg_resqueue_rsqname_index 6028 on pg_resqueue using btree(rsqname name_ops)
declare unique index pg_resourcetype_oid_index 6061 on pg_resourcetype using btree(oid oid_ops)
declare unique index pg_resourcetype_restypid_index 6062 on pg_resourcetype using btree(restypid int2_ops)
declare unique index pg_resourcetype_resname_index 6063 on pg_resourcetype using btree(resname name_ops)
declare index pg_resqueuecapability_resqueueid_index 6442 on pg_resqueuecapability using btree(resqueueid oid_ops)
declare index pg_resqueuecapability_restypid_index 6443 on pg_resqueuecapability using btree(restypid int2_ops)
declare unique index pg_resgroup_oid_index 6447 on pg_resgroup using btree(oid oid_ops)
declare unique index pg_resgroup_rsgname_index 6444 on pg_resgroup using btree(rsgname name_ops)
declare unique index pg_resgroupcapability_resgroupid_reslimittype_index 6445 on pg_resgroupcapability using btree(resgroupid oid_ops, reslimittype int2_ops)
declare index pg_resgroupcapability_resgroupid_index 6446 on pg_resgroupcapability using btree(resgroupid oid_ops)
declare unique index gp_segment_config_content_preferred_role_index 7139 on gp_segment_configuration using btree(content int2_ops, preferred_role char_ops)
declare unique index gp_segment_config_dbid_index 7140 on gp_segment_configuration using btree(dbid int2_ops)
declare unique index pg_extprotocol_oid_index 7156 on pg_extprotocol using btree(oid oid_ops)
declare unique index pg_extprotocol_ptcname_index 7177 on pg_extprotocol using btree(ptcname name_ops)
declare index pg_attribute_encoding_attrelid_index 6236 on pg_attribute_encoding using btree(attrelid oid_ops)
declare unique index pg_attribute_encoding_attrelid_attnum_index 6237 on pg_attribute_encoding using btree(attrelid oid_ops, attnum int2_ops)
declare unique index pg_attribute_encoding_attrelid_filenum_index 6238 on pg_attribute_encoding using btree(attrelid oid_ops, filenum int2_ops)
declare unique index pg_type_encoding_typid_index 6207 on pg_type_encoding using btree(typid oid_ops)
declare unique index pg_proc_callback_profnoid_promethod_index 9926 on pg_proc_callback using btree(profnoid oid_ops, promethod char_ops)
declare unique index pg_compression_compname_index 7059 on pg_compression using btree(compname name_ops)
declare unique index pg_range_rngtypid_index 3542 on pg_range using btree(rngtypid oid_ops)
declare unique index pg_policy_oid_index 3257 on pg_policy using btree(oid oid_ops)
declare unique index pg_policy_polrelid_polname_index 3258 on pg_policy using btree(polrelid oid_ops, polname name_ops)
declare unique index pg_replication_origin_roiident_index 6001 on pg_replication_origin using btree(roident oid_ops)
declare unique index pg_replication_origin_roname_index 6002 on pg_replication_origin using btree(roname text_ops)
declare unique index pg_partitioned_table_partrelid_index 3351 on pg_partitioned_table using btree(partrelid oid_ops)
declare unique index pg_publication_oid_index 6110 on pg_publication using btree(oid oid_ops)
declare unique index pg_publication_pubname_index 6111 on pg_publication using btree(pubname name_ops)
declare unique index pg_publication_rel_oid_index 6112 on pg_publication_rel using btree(oid oid_ops)
declare unique index pg_publication_rel_prrelid_prpubid_index 6113 on pg_publication_rel using btree(prrelid oid_ops, prpubid oid_ops)
declare unique index pg_subscription_oid_index 6114 on pg_subscription using btree(oid oid_ops)
declare unique index pg_subscription_subname_index 6115 on pg_subscription using btree(subdbid oid_ops, subname name_ops)
declare unique index pg_subscription_rel_srrelid_srsubid_index 6117 on pg_subscription_rel using btree(srrelid oid_ops, srsubid oid_ops)
declare unique index gp_partition_template_relid_level_index 5023 on gp_partition_template using btree(relid oid_ops, level int2_ops)
build indices
