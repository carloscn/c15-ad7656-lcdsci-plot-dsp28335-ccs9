################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DSP2833x_headers/source/%.obj: ../DSP2833x_headers/source/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/opt/ti/ccs9/ccs/tools/compiler/ti-cgt-c2000_18.12.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/opt/ti/ccs9/ccs/tools/compiler/ti-cgt-c2000_18.12.1.LTS/include" --include_path="D:/workspace/CCS_V9/C15_AD7656_LCD_PLOT_F28335_CCS9_0/DSP2833x_common/include" --include_path="D:/workspace/CCS_V9/C15_AD7656_LCD_PLOT_F28335_CCS9_0/DSP2833x_headers/include" --include_path="D:/workspace/CCS_V9/C15_AD7656_LCD_PLOT_F28335_CCS9_0/inc" --advice:performance=all -g --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="DSP2833x_headers/source/$(basename $(<F)).d_raw" --obj_directory="DSP2833x_headers/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


