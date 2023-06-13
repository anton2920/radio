package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"unsafe"

	"github.com/anton2920/radio/pa"
)

type WAVEHeader struct {
	RIFF struct {
		ChunkID   [4]byte /* ASCII "RIFF". */
		ChunkSize uint32  /* 36 + SubChunk2Size. */
		Format    [4]byte /* ASCII "WAVE". */
	}

	fmt struct {
		SubChunk1ID   [4]byte /* ASCII "fmt ". */
		SubChunk1Size uint32  /* 16. */
		AudioFormat   uint16  /* 1. */
		NumChannels   uint16  /* Mono = 1, Stereo = 2, etc. */
		SampleRate    uint32  /* 8000, 44100, etc. */
		ByteRate      uint32  /* SampleRate * BlockAlign. */
		BlockAlign    uint16  /* NumChannels * BitsPerSample / 8. */
		BitsPerSample uint16  /* 8, 16, etc. */
	}

	data struct {
		SubChunk2ID   [4]byte /* ASCII "data". */
		SubChunk2Size uint32  /* NumSamples * BlockAlign. */
	}
}

var lock chan struct{}

func RadioHandler(w http.ResponseWriter, r *http.Request) {
	const sampleRate = 48000
	const nchannels = 2
	const nbytes = 1920
	const bps = 16

	select {
	case lock <- struct{}{}:
	default:
		w.WriteHeader(http.StatusServiceUnavailable)
		return
	}
	defer func() { <-lock }()

	log.Printf("Accepted from %s (%s)", r.RemoteAddr, r.UserAgent())

	s, err := pa.SimpleNew("radio", "radio-record-stream", pa.SampleSpec{
		Format:   pa.SampleS16LE, /* NOTE(anton2920): for 'bps == 16'. */
		Rate:     sampleRate,
		Channels: nchannels,
	})
	if err != nil {
		log.Print("ERROR: failed to create Pulse Audio handler: ", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	defer pa.SimpleFree(s)

	/* Writing RIFF header. */
	var hdr WAVEHeader
	hdr.RIFF.ChunkID = [4]byte{'R', 'I', 'F', 'F'}
	hdr.RIFF.ChunkSize = 36 + nbytes
	hdr.RIFF.Format = [4]byte{'W', 'A', 'V', 'E'}

	hdr.fmt.SubChunk1ID = [4]byte{'f', 'm', 't', ' '}
	hdr.fmt.SubChunk1Size = 16
	hdr.fmt.AudioFormat = 1
	hdr.fmt.NumChannels = nchannels
	hdr.fmt.SampleRate = sampleRate
	hdr.fmt.ByteRate = sampleRate * nchannels * bps / 8
	hdr.fmt.BlockAlign = nchannels * bps / 8
	hdr.fmt.BitsPerSample = bps

	hdr.data.SubChunk2ID = [4]byte{'d', 'a', 't', 'a'}
	hdr.data.SubChunk2Size = nbytes

	w.Header().Set(http.CanonicalHeaderKey("Content-Type"), "audio/wav")
	w.Header().Set(http.CanonicalHeaderKey("Transfer-Encoding"), "identity")
	w.Write((*[44]byte)((unsafe.Pointer)(&hdr))[:])

	/* Writing samples. */
	buf := make([]byte, nbytes)
	for {
		select {
		case <-r.Context().Done():
			log.Printf("Disconnected: %s (%s)", r.RemoteAddr, r.UserAgent())
			return
		default:
			if _, err := s.Read(buf); err != nil {
				log.Print("ERROR: failed to read audio bytes: ", err)
				return
			}
			w.Write(buf)
		}
	}
}

func main() {
	port := flag.Uint("p", 1500, "port value to listen on")
	flag.Parse()
	netAddr := fmt.Sprintf("0.0.0.0:%d", *port)

	http.HandleFunc("/radio", RadioHandler)

	lock = make(chan struct{}, 1)

	log.Print("Listening on ", netAddr)
	log.Fatal(http.ListenAndServe(netAddr, nil))
}
