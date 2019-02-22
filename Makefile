

default:
	cd build && make
	@if [ -e src/playdpp.exe ]; then mv src/playdpp.exe playdpp.exe; fi
	@if [ -e src/playdpp ]; then mv src/playdpp .; fi

clean:
	@rm -f build/*.o playdpp
	@echo "Clean!"

install:
	install playdpp /usr/local/bin

