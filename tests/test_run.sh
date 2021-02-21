#!/bin/sh

if [[ ! -z $1 ]]; then
	while [ $# -gt 0 ]; do
		echo "Arg: $1"
		shift
	done
	exit 0
fi

echo "Test works"
echo "Error works too" >&2

exit 42
