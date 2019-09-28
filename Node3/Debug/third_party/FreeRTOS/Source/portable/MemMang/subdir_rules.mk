################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
third_party/FreeRTOS/Source/portable/MemMang/heap_2.obj: E:/merged_partition_content/Programs/ti/third_party/FreeRTOS/Source/portable/MemMang/heap_2.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"E:/merged_partition_content/Programs/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 --include_path="E:/merged_partition_content/Programs/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --include_path="E:/merged_partition_content/MCT/Spring 2019 (FINALLY!!)/Automotive_Embedded/projectss/Node3" --include_path="E:/merged_partition_content/Programs/ti/examples/boards/ek-tm4c123gxl" --include_path="E:/merged_partition_content/Programs/ti" --include_path="E:/merged_partition_content/Programs/ti/third_party" --include_path="E:/merged_partition_content/Programs/ti/third_party/FreeRTOS/Source/include" --include_path="E:/merged_partition_content/Programs/ti/third_party/FreeRTOS" --include_path="E:/merged_partition_content/Programs/ti/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --advice:power=all -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="third_party/FreeRTOS/Source/portable/MemMang/heap_2.d_raw" --obj_directory="third_party/FreeRTOS/Source/portable/MemMang" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


