BUILD_DIR ?= ./build
SRC_DIR ?= ./src
DEV_DIR ?= ./dev

DOCKER ?= docker
IMAGE_NAME ?= oir-dev

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

$(BUILD_DIR)/%.c$(OBJ_SUFFIX).o: %.c Makefile
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEPS)


########################################
# helper phony targets
########################################

.PHONY: build
build: $(BUILD_DIR)/$(TARGET_EXEC)

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
	-$(RM) -r $(BUILD_DIR)
	-$(RM) .image-built


########################################
# targets to build in docker environment
########################################

DOCKER_RUN := $(DOCKER) run --rm -it -v $(CURDIR):/work $(IMAGE_NAME)

.PHONY: image
image: .image-built

.image-built: $(DEV_DIR)/Dockerfile
	$(DOCKER) build $(DEV_DIR) -t $(IMAGE_NAME)
	touch .image-built

.PHONY: build.image
build.image: .image-built
	$(DOCKER_RUN) make build

.PHONY: test.image
test.image: .image-built
	$(DOCKER_RUN) make test

.PHONY: watch.image
watch.image: .image-built
	$(DOCKER_RUN) make watch
