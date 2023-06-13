package pa

/*
#cgo CFLAGS: -I/usr/local/include
#cgo LDFLAGS: -L/usr/local/lib
#cgo LDFLAGS: -lpulse -lpulse-simple

#include <stdlib.h>

#include <pulse/error.h>
#include <pulse/simple.h>
*/
import "C"
import (
	"errors"
	"unsafe"
)

type Simple C.pa_simple
type SampleSpec struct {
	Format   int
	Rate     uint32
	Channels uint8
}

const (
	SampleS16LE = C.PA_SAMPLE_S16LE
)

func SimpleNew(streamName string, name string, ss SampleSpec) (*Simple, error) {
	var errCode C.int

	cname := C.CString(name)
	defer C.free(unsafe.Pointer(cname))

	cstreamName := C.CString(streamName)
	defer C.free(unsafe.Pointer(cstreamName))

	var css C.pa_sample_spec
	css.format = C.pa_sample_format_t(ss.Format)
	css.rate = C.uint32_t(ss.Rate)
	css.channels = C.uint8_t(ss.Channels)

	s := (*Simple)(C.pa_simple_new(nil, (*C.char)(cname), C.PA_STREAM_RECORD, nil, (*C.char)(cstreamName), &css, nil, nil, &errCode))
	if s == nil {
		return nil, errors.New(C.GoString(C.pa_strerror(errCode)))
	}

	return s, nil
}

func SimpleFree(s *Simple) {
	C.pa_simple_free((*C.pa_simple)(s))
}

func (s *Simple) Read(buf []byte) (int, error) {
	var errCode C.int
	res := C.pa_simple_read((*C.pa_simple)(s), unsafe.Pointer(&buf[0]), (C.size_t)(len(buf)), &errCode)
	if res < 0 {
		return 0, errors.New(C.GoString(C.pa_strerror(errCode)))
	}

	return len(buf), nil
}
