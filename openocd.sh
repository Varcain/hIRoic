#!/bin/sh

IMAGE_PATH=$1
IMAGE_ADDR=$2

openocd \
	-f board/stm32f7discovery.cfg \
	-c "init" \
	-c "reset init" \
	-c "flash probe 0" \
	-c "flash write_image erase $IMAGE_PATH $IMAGE_ADDR" \
	-c "reset run"
