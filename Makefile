#
# LuxeC (c) 2023 by Jozef Nagy
#
# LuxeC is licensed under a
# Creative Commons Attribution-NoDerivatives 4.0 International License.
#
# You should have received a copy of the license along with this
# work. If not, see <http://creativecommons.org/licenses/by-nd/4.0/>.
#
#

# Valid options: x86_64 arm64
ARCH := x86_64

CC := gcc
LD := $(CC)

BUILD_DIR := build
PROGRAM := $(BUILD_DIR)/$(ARCH)-luxecc

DEST := /usr/local/bin

INCDIR := include

# We use += here in case the user wants to supply their own flags.
CFLAGS += -Wall -Wextra -Werror -std=c99
CPPFLAGS += $(INCDIR:%=-I%) -MMD -MP
LDFLAGS +=

ARCH_C_FILES := $(shell find src/$(ARCH) -name "*.c" -type f)
C_FILES := $(ARCH_C_FILES) $(shell find src -name "*.c" -not -path "src/arm64/*" -not -path "src/x86_64/*" -type f)

OBJ := $(C_FILES:%.c=$(BUILD_DIR)/%.o)


.PHONY: all
all: $(PROGRAM)

.PHONY: format
format:
	@clang-format -i $(shell find src include -name "*.c" -o -name "*.h")

.PHONY: install
install: $(PROGRAM)
	@install $(PROGRAM) $(DEST)

$(PROGRAM): $(OBJ)
	@mkdir -p $(dir $@)
	@printf " LD   $@\n"
	@$(LD) $(OBJ) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@printf " CC   $^\n"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@printf " CLEAN\n"
	@rm -rf $(BUILD_DIR) out.s