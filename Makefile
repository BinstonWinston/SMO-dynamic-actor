LOGGER_IP ?= "127.0.0.1" # put log server IP in string
FTP_IP ?= 10.0.1.72 # put console IP here
.PHONY: all clean

all:
	cmake --toolchain=cmake/toolchain.cmake -DLOGGER_IP=$(LOGGER_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

send:
	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -DLOGGER_IP=$(LOGGER_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

clean:
	rm -r build || true

log: all
	python3 scripts/tcpServer.py 0.0.0.0
