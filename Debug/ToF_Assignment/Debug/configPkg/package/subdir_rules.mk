################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1398914479:
	@$(MAKE) --no-print-directory -Onone -f ToF_Assignment/Debug/configPkg/package/subdir_rules.mk build-1398914479-inproc

build-1398914479-inproc: ../ToF_Assignment/Debug/configPkg/package/build.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_32_00_06_core/xs" --xdcpath="C:/ti/tirtos_tivac_2_16_01_14/packages;C:/ti/tirtos_tivac_2_16_01_14/products/tidrivers_tivac_2_16_01_13/packages;C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages;C:/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09/packages;C:/ti/tirtos_tivac_2_16_01_14/products/uia_2_00_05_50/packages;C:/ti/tirtos_tivac_2_16_01_14/products/ns_1_11_00_10/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.tiva:TM4C1294NCPDT -r release -b "C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project/ToF_Assignment/Debug/configPkg/config.bld" -c "C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS" --compileOptions "-mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path=\"C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project/drivers\" --include_path=\"C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project\" --include_path=\"C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project\" --include_path=\"C:/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b\" --include_path=\"C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages/ti/sysbios/posix\" --include_path=\"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include\" --include_path=\"C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project\" --define=ccs --define=PART_TM4C1294NCPDT --define=TIVAWARE -g --c99 --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1398914479 ../ToF_Assignment/Debug/configPkg/package/build.cfg C:/Users/Delluser/Desktop/Projects\ go\ in\ here/Get\ Rotated\ Project/ToF_Assignment/Debug/configPkg/config.bld
configPkg/compiler.opt: build-1398914479 C:/Users/Delluser/Desktop/Projects\ go\ in\ here/Get\ Rotated\ Project/ToF_Assignment/Debug/configPkg/config.bld
configPkg/: build-1398914479 C:/Users/Delluser/Desktop/Projects\ go\ in\ here/Get\ Rotated\ Project/ToF_Assignment/Debug/configPkg/config.bld

ToF_Assignment/Debug/configPkg/package/%.obj: ../ToF_Assignment/Debug/configPkg/package/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project/drivers" --include_path="C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project" --include_path="C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project" --include_path="C:/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages/ti/sysbios/posix" --include_path="C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/Users/Delluser/Desktop/Projects go in here/Get Rotated Project" --define=ccs --define=PART_TM4C1294NCPDT --define=TIVAWARE -g --c99 --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="ToF_Assignment/Debug/configPkg/package/$(basename $(<F)).d_raw" --obj_directory="ToF_Assignment/Debug/configPkg/package" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

