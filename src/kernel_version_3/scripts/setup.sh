#!/bin/bash
DZ_PROJECT_BASE_PATH="$(pwd)"
export DZ_PROJECT_BASE_PATH

export DZ_PROJECT_PATH="$(pwd)"
export DZ_PROJECT_PATH

DZ_VERSION_1_0="1.0"
DZ_CURRENT_VERSION=${DZ_VERSION_1_0}

DZ_HEADERS_PATH="${DZ_PROJECT_PATH}/headers"

DZ_BASE_ENGINE="base_engine"
DZ_HASH_ENGINE="hash_engine"
DZ_MD_ENGINE="md_engine"
DZ_VOLUME_ENGINE="volume_engine"
DZ_DEDUPE_ENGINE="dedupe_engine"
DZ_FEC_ENGINE="fec_engine"
DZ_ALIGNMENT_ENGINE="alignment_engine"
DZ_BEC_ENGINE="bec_engine"
DZ_INFRA_ENGINE="infra_engine"
DZ_SYSFS_ENGINE="sysfs_engine"
DZ_SYSFS_PARENT="/sys/avm/"
DZ_BUILD_PKG_DIR="build_pkg"
export DZ_BUILD_PKG_DIR


DZ_BASE_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_BASE_ENGINE}"
DZ_HASH_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_HASH_ENGINE}"
DZ_MD_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_MD_ENGINE}"
DZ_VOLUME_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_VOLUME_ENGINE}"
DZ_DEDUPE_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_DEDUPE_ENGINE}"
DZ_FEC_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_FEC_ENGINE}"
DZ_ALIGNMENT_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_ALIGNMENT_ENGINE}"
DZ_BEC_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_BEC_ENGINE}"
DZ_INFRA_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_INFRA_ENGINE}"
DZ_SYSFS_ENGINE_PATH="${DZ_PROJECT_PATH}/${DZ_SYSFS_ENGINE}"

DZ_SYSFS_ENGINE_PATH_RELATIVE="sysfs_engine"
DZ_HASH_PATH="${DZ_PROJECT_PATH}/hash_engine"
DZ_HASH_PATH_RELATIVE="hash_engine"
DZ_SUBDIRS="sysfs"

# Defaults for platform specific variables
#DZ_CC="gcc"
#DZ_D_MACRO=" -D_AISA_"
#DZ_CFLAGS=" -Wall -g "

#echo "include ${DZ_PROJECT_PATH}/include.mk" > include.mk
{
# Current Environment Set with following variable set
echo
echo "# Variables that are same for all platforms"
echo "DZ_PROJECT_BASE_PATH           := ${DZ_PROJECT_BASE_PATH}"
echo "DZ_PROJECT_PATH                := ${DZ_PROJECT_PATH}"
echo "DZ_HEADERS_PATH                := ${DZ_HEADERS_PATH}"
echo "DZ_SYSFS_ENGINE_PATH           := ${DZ_SYSFS_ENGINE_PATH}"
echo "DZ_SYSFS_ENGINE_PATH_RELATIVE  := ${DZ_SYSFS_ENGINE_PATH_RELATIVE}"
echo "DZ_HASH_PATH                   := ${DZ_HASH_PATH}"
echo "DZ_HASH_PATH_RELATIVE          := ${DZ_HASH_PATH_RELATIVE}"
echo "DZ_SUBDIRS                     := ${DZ_SUBDIRS}"
echo "DZ_CURRENT_VERSION             := ${DZ_CURRENT_VERSION}"
echo ""
echo "# Kernel Module Names of their corresponding engines"
echo "# Dependency is represented as number sequence. Higher number value is dependent on lower number"
echo "DZ_LOG_ENGINE_KERNEL_MODULE_NAME       := datumsoft_zaidstor_avm_1_B_log_engine_module"
echo "DZ_INFRA_ENGINE_KERNEL_MODULE_NAME     := datumsoft_zaidstor_avm_2_B_infra_engine_module"
echo "DZ_LAB_ENGINE_KERNEL_MODULE_NAME       := datumsoft_zaidstor_avm_3_B_lab_engine_module"
echo "DZ_BEC_ENGINE_KERNEL_MODULE_NAME       := datumsoft_zaidstor_avm_4_B_bec_engine_module"
echo "DZ_METADATA_ENGINE_KERNEL_MODULE_NAME  := datumsoft_zaidstor_avm_5_B_metadata_engine_module"
echo "DZ_DEDUPE_ENGINE_KERNEL_MODULE_NAME    := datumsoft_zaidstor_avm_6_B_dedupe_engine_module"
echo "DZ_ALIGNMENT_ENGINE_KERNEL_MODULE_NAME := datumsoft_zaidstor_avm_7_B_alignment_engine_module"
echo "DZ_FEC_ENGINE_KERNEL_MODULE_NAME       := datumsoft_zaidstor_avm_8_B_fec_engine_module"
echo "DZ_TARGET_ENGINE_KERNEL_MODULE_NAME    := datumsoft_zaidstor_avm_9_B_target_engine_module"
echo "DZ_VOLUME_ENGINE_KERNEL_MODULE_NAME    := datumsoft_zaidstor_avm_A_B_volume_engine_module"
echo "DZ_SYSFS_ENGINE_KERNEL_MODULE_NAME     := datumsoft_zaidstor_avm_B_B_sysfs_engine_module"

} > include.mk

echo "" >>include.mk
echo "" >>include.mk
echo "#Generated on `date`" >>include.mk
echo "Welcome User !!! ""`date`"
echo "All environment variables are setup. Refer to include.mk file for variables"
