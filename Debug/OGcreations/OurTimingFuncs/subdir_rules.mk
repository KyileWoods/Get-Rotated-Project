################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
OGcreations/OurTimingFuncs/%.obj: ../OGcreations/OurTimingFuncs/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project/drivers" --include_path="C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project" --include_path="C:/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project" --include_path="C:/Users/ti/tivaware_c_series_2_1_4_178/examples/boards/ek-tm4c1294xl-boostxl-kentec-s1" --define=ccs --define=PART_TM4C1294NCPDT --define=TIVAWARE --define=ccs="ccs" -g --c99 --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="OGcreations/OurTimingFuncs/$(basename $(<F)).d_raw" --obj_directory="OGcreations/OurTimingFuncs" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


