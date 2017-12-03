CC     = gcc
CFLAGS = -g -Wall -pedantic -std=gnu99
LIBS   = -lpthread
NAME   = bounded_buffer
SUBMIT = buffer_driver.c buffer.c buffer.h Makefile shortlist longlist
SUBMIT_SUFFIX = -mt

all: buffer_driver.c buffer.h buffer.c
	$(CC) $(CFLAGS) -o $(NAME) $^ $(LIBS)

.PHONY: check-username
check-username:
	@if [ -z "$(GT_USERNAME)" ]; then \
        echo "Before running 'make submit', please set your GT Username in the environment"; \
        echo "Run the following to set your username: \"export GT_USERNAME=<your username>\""; \
        exit 1; \
        fi

submit:	check-username
	@(tar zcfh $(GT_USERNAME)$(SUBMIT_SUFFIX).tar.gz $(SUBMIT) && \
        echo "Created submission archive $$(tput bold)$(GT_USERNAME)$(SUBMIT_SUFFIX).tar.gz$$(tput sgr0).") || \
        (echo "$$(tput bold)$$(tput setaf 1)Error:$$(tput sgr0) Failed to create submission archive." && \
        rm -f $$name$(SUBMIT_SUFFIX).tar.gz)

clean:
	rm -f $(NAME) *.o *-mt.tar.gz
