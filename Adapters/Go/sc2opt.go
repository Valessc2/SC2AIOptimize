package sc2opt

/*
#cgo LDFLAGS: -lsc2opt_c
#include <stdlib.h>
#include "sc2opt/adapters/c/sc2opt_c.h"
*/
import "C"

import "unsafe"

func APIVersion() (uint32, uint32) {
	return uint32(C.sc2opt_c_api_version_major()), uint32(C.sc2opt_c_api_version_minor())
}

func RegistryUnitCount() uint64 {
	return uint64(C.sc2opt_c_registry_unit_count())
}

func DistanceSquaredInto(xy []float32, anchorX, anchorY float32, output []float32) (int, int) {
	var written C.size_t
	var xyPtr *C.float
	var outPtr *C.float
	if len(xy) != 0 {
		xyPtr = (*C.float)(unsafe.Pointer(&xy[0]))
	}
	if len(output) != 0 {
		outPtr = (*C.float)(unsafe.Pointer(&output[0]))
	}
	status := C.sc2opt_c_distance_squared_into(
		xyPtr, C.size_t(len(xy)), C.float(anchorX), C.float(anchorY),
		outPtr, C.size_t(len(output)), &written)
	return int(status), int(written)
}

func WithinRadiusMaskInto(xy []float32, anchorX, anchorY, radius float32, output []byte) (int, int) {
	var written C.size_t
	var xyPtr *C.float
	var outPtr *C.uint8_t
	if len(xy) != 0 {
		xyPtr = (*C.float)(unsafe.Pointer(&xy[0]))
	}
	if len(output) != 0 {
		outPtr = (*C.uint8_t)(unsafe.Pointer(&output[0]))
	}
	status := C.sc2opt_c_within_radius_mask_into(
		xyPtr, C.size_t(len(xy)), C.float(anchorX), C.float(anchorY), C.float(radius),
		outPtr, C.size_t(len(output)), &written)
	return int(status), int(written)
}
