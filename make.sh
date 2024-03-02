#!/bin/sh

PROJECT=radio

VERBOSITY=0
VERBOSITYFLAGS=""
while test "$1" = "-v"; do
	VERBOSITY=$((VERBOSITY+1))
	VERBOSITYFLAGS="$VERBOSITYFLAGS -v"
	shift
done

run()
{
	if test $VERBOSITY -gt 0; then echo "$@"; fi
	"$@" || exit 1
}

# NOTE(anton2920): disable Go 1.11+ package management.
GO111MODULE=off; export GO111MODULE

STARTTIME=`date +%s`

case $1 in
	'' | debug)
		run go build $VERBOSITYFLAGS -o $PROJECT -race -gcflags='all=-N -l' main.go
		;;
	clean)
		run rm -f $PROJECT
		;;
	fmt)
		if which goimports >/dev/null; then
			run goimports -l -w *.go
		else
			run gofmt -l -s -w *.go
		fi
		;;
	release)
		run go build $VERBOSITYFLAGS -o $PROJECT -ldflags='all=-s -w' .
		;;
esac

ENDTIME=`date +%s`

echo Done $1 in $((ENDTIME-STARTTIME))s
