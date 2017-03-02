.PHONY: cppcheck
.DEFAULT_GOAL := help
ALPINE := $(shell [ -f /etc/apk/world ] && echo True )

CFLAGS := \
	-std=gnu99 \
	-fPIC \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-ffunction-sections \
	-fdata-sections \
	-Wno-unused-function \
	-O0 \
	-ggdb3 \
	-I"$(BASE)/include" \
	-I"$(BUILD)" \
	$(CFLAGS)

LDFLAGS := \
	-L"$(BUILD)" \
	-luv \
	-lssl \
	-lm \
	-lpthread \
	-lcrypto

ifeq ($(ALPINE),True)
ifeq ($(CC), clang)
test: all cppcheck todo  ## Test everything
else
CFLAGS += --coverage
LDFLAGS += --coverage

test: coverage cppcheck todo
endif
else
CFLAGS += --coverage
LDFLAGS += --coverage

test: coverage cppcheck todo
endif

help:  ## Display this help
	@cat $(MAKEFILE_LIST) | grep -E '^[0-9a-zA-Z_-]+:.*?## .*$$' | sort -k1,1 | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

etests:
	$(BUILD)/src/chirp_etest & \
	PID=$$!; \
	sleep 1; \
	kill -2 $$PID
	$(BUILD)/src/quickcheck_etest

cppcheck:  ## Static analysis
	cppcheck -v \
		--error-exitcode=1 \
		--std=c99 \
		--inline-suppr \
		-I"$(BASE)/include" \
		-DCH_ACCEPT_STRANGE_PLATFORM \
		"$(BASE)/src"

todo:  ## Show todos
	@grep -Inrs ".. todo" $(BASE)/src; true
	@grep -Inrs TODO $(BASE)/src; true

include $(BASE)/mk/rules.mk

$(BUILD)/%_etest: $(BUILD)/%_etest.o libchirp.a
ifeq ($(VERBOSE),True)
	$(CC) -o $@ $< $(BUILD)/libchirp.a $(LDFLAGS)
ifeq ($(STRIP),True)
	$(STRPCMD) $@
endif
else
	@echo LD $@
	@$(CC) -o $@ $< $(BUILD)/libchirp.a $(LDFLAGS)
ifeq ($(STRIP),True)
	@echo STRIP $@
	@$(STRPCMD) $@
endif
endif

$(BUILD)/%.c.gcov: $(BUILD)/%.o
ifeq ($(CC),clang)
ifeq ($(UNAME_S),Darwin)
	xcrun llvm-cov gcov $<
else
	llvm-cov gcov $<
endif
else
	gcov $<
endif
ifneq ($(IGNORE_COV),True)
	[ -f "$(notdir $@)" ]
endif
