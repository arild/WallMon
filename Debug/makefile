################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include test/subdir.mk
-include test/Debug/subdir.mk
-include server/subdir.mk
-include monitors/slowlogger/subdir.mk
-include monitors/processall/subdir.mk
-include monitors/process/subdir.mk
-include monitors/fastlogger/subdir.mk
-include monitors/boids2d/subdir.mk
-include monitors/Debug/slowlogger/subdir.mk
-include monitors/Debug/processall/subdir.mk
-include lib/touchmanager/subdir.mk
-include lib/stubs/subdir.mk
-include lib/subdir.mk
-include daemon/subdir.mk
-include daemon/Debug/subdir.mk
-include daemon/Debug/.metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include daemon/.metadata/.plugins/org.eclipse.cdt.make.core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: WallMon

# Tool invocations
WallMon: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++  -o"WallMon" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) WallMon
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
