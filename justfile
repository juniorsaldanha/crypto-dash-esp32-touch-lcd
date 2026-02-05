default:
  just --list --unsorted

alias b := build
alias f := flash
alias bf := build-flash
alias t := test
alias m := monitor
alias c := clean

build:
  pio run

flash:
  pio run --target upload

build-flash: build flash

test:
  pio test

monitor:
  pio device monitor

clean:
  pio run --target clean
