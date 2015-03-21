#!/bin/bash

clean_exec()
{
	FD="$1"

	if [[ -d "$FD" ]]; then
		for child in $(ls "$FD"); do
			clean_exec "$FD/$child"
			FD="$1"
		done
	elif [[ -x "$FD" ]]; then
		if [[ ${FD##*.} != "sh" ]]; then
			rm "$FD"
		else
			echo "skip $FD"
		fi
	fi
}

if [[ "$1" = "" ]]; then
	clean_exec .
else
	for fd in "$@"; do
		clean_exec "$fd"
	done
fi
