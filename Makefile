CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Winline -ffast-math -pthread -O2 -g

SRC_DIR = .
BUILD_DIR = build
ODIR = obj

SRCS = main.cc vec.cc color.cc io.cc texture.cc entity.cc camera.cc material.cc renderer.cc
_OBJS = $(subst .cc,.o,$(SRCS))
OBJS = $(addprefix $(BUILD_DIR)/, $(_OBJS))

all: dirs $(BUILD_DIR)/neon

.PHONY: dirs
dirs:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/neon: $(OBJS)
	@echo "linking $@"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc
	@echo "compiling $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -f $(OBJS)
