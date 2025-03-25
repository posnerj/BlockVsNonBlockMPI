# ----------------------------------------
# Compiler and Flags
# ----------------------------------------
CC       := mpicc
SP       := scorep
CFLAGS   := -O3 -march=native -flto -Isrc
LDFLAGS  :=

# ----------------------------------------
# Directories and Paths
# ----------------------------------------
SRC_DIR  := src
OUT_DIR  := $(CURDIR)/out
SCOREP_OUT_DIR := $(OUT_DIR)/scorep


# ----------------------------------------
# Discover main source files and targets
# ----------------------------------------
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
EXECUTABLES := $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%,$(SRC_FILES))
SCOREP_EXECUTABLES := $(patsubst $(SRC_DIR)/%.c,$(SCOREP_OUT_DIR)/%,$(SRC_FILES))

# ----------------------------------------
# Default Target
# ----------------------------------------
all: build

build: $(OUT_DIR) $(EXECUTABLES)

# Score-P instrumented build
build-scorep: $(SCOREP_OUT_DIR) $(SCOREP_EXECUTABLES)

# ----------------------------------------
# Build Rule for Executables
# ----------------------------------------
$(OUT_DIR)/%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(SCOREP_OUT_DIR)/%: $(SRC_DIR)/%.c
	$(SP) $(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# ----------------------------------------
# Create output directory if needed
# ----------------------------------------
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(SCOREP_OUT_DIR):
	mkdir -p $(SCOREP_OUT_DIR)

# ----------------------------------------
# Clean Target
# ----------------------------------------
clean:
	rm -rf $(OUT_DIR)
	rm -rf $(SCOREP_OUT_DIR)
