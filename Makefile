CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Winline -pthread -ffast-math -O2 -g

SRC_DIR = .
BUILD_DIR = build

SRCS = $(wildcard *.cc)

_OBJS = $(subst .cc,.o,$(SRCS))
OBJS = $(addprefix $(BUILD_DIR)/, $(_OBJS))

_DEPS = $(subst .cc,.d,$(SRCS))
DEPS = $(addprefix $(BUILD_DIR)/, $(_DEPS))

.PHONY: all
all: dirs $(BUILD_DIR)/neon

.PHONY: dirs
dirs:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/neon: $(OBJS)
	@echo "linking $@"
	@$(CXX) $(CXXFLAGS) $^ -o $@

-include $(DEPS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc Makefile
	@echo "compiling $@"
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

.PHONY: clean
clean:
	@rm -f $(OBJS) $(DEPS)
