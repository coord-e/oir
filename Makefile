BUILD_DIR ?= ./build
SRC_DIR ?= ./src

CFLAGS ?= -Wall -std=c11 -pedantic -Isrc
CPPFLAGS ?= -MMD -MP

DEBUG ?= 1
ifeq ($(DEBUG), 1)
  CFLAGS += -O0 -g3 -fsanitize=address -fPIE
  LDFLAGS += -fsanitize=address
  OBJ_SUFFIX = .debug
else
  CFLAGS += -O3
  OBJ_SUFFIX =
endif

TARGET_EXEC ?= oir$(OBJ_SUFFIX)

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%$(OBJ_SUFFIX).o)
DEPS := $(OBJS:.o=.d)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c$(OBJ_SUFFIX).o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: test
test: $(BUILD_DIR)/$(TARGET_EXEC)
	echo "not implemented"

.PHONY: style
style:
	find src -name '*.c' -or -name '*.h' | xargs clang-format -i

.PHONY: watch
watch:
	while true; do \
		make test; \
		inotifywait -q -e close_write -r .; \
	done

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)
