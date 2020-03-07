BUILD_DIR ?= ./build
SRC_DIR ?= ./src
DEV_DIR ?= ./dev

DOCKER ?= docker
IMAGE_NAME ?= oir-dev

CFLAGS ?= -Wall -std=c11 -pedantic -Isrc -D_POSIX_C_SOURCE=2
CPPFLAGS ?= -MMD -MP

DEBUG ?= 1
SANITIZER ?= 0
OBJ_SUFFIX ?=
TARGET_EXEC ?= oir$(OBJ_SUFFIX)


ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -g3
	OBJ_SUFFIX := $(OBJ_SUFFIX).debug
else
	CFLAGS += -O3
endif

ifeq ($(SANITIZER), 1)
	CFLAGS += -fsanitize=address -fPIE -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=address
	OBJ_SUFFIX := $(OBJ_SUFFIX).sanitized
endif


SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%$(OBJ_SUFFIX).o)
DEPS := $(OBJS:.o=.d)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c$(OBJ_SUFFIX).o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

-include $(DEPS)


########################################
# helper phony targets
########################################

.PHONY: build
build: $(BUILD_DIR)/$(TARGET_EXEC)

.PHONY: test
test: $(BUILD_DIR)/$(TARGET_EXEC)
	OIR_BIN=$(BUILD_DIR)/$(TARGET_EXEC) $(DEV_DIR)/test.sh

.PHONY: style
style:
	find src -name '*.c' -or -name '*.h' | xargs clang-format -i

.PHONY: watch
watch:
	while true; do \
		$(MAKE) test; \
		inotifywait -q -e close_write --exclude '\.git' -r .; \
	done

.PHONY: clean
clean:
	-$(RM) -r $(BUILD_DIR)
	-$(RM) .image-built


########################################
# targets to build in docker environment
########################################

DOCKERFILE := $(DEV_DIR)/Dockerfile
DOCKER_VARS := ACCEPT BUILD_DIR SRC_DIR DEV_DIR CC DEBUG SANITIZER ASAN_OPTIONS TARGET_EXEC
DOCKER_RUN := \
	$(DOCKER) run --rm \
		$(shell [ -t 0 ] && echo " -ti") \
		-v $(CURDIR):/work \
		$(foreach v,$(DOCKER_VARS),-e $(v)="$($(v))") \
		$(IMAGE_NAME)

.PHONY: image
image: .image-built

.image-built: $(DOCKERFILE)
	$(DOCKER) build $(dir $(DOCKERFILE)) -f $(DOCKERFILE) -t $(IMAGE_NAME)
	@touch .image-built

.PHONY: build.docker
build.docker: .image-built
	$(DOCKER_RUN) make build

.PHONY: style.docker
style.docker: .image-built
	$(DOCKER_RUN) make style

.PHONY: test.docker
test.docker: .image-built
	$(DOCKER_RUN) make test

.PHONY: watch.docker
watch.docker: .image-built
	$(DOCKER_RUN) make watch
